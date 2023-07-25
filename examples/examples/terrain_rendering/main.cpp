#include "PerlinNoise.hpp"

#include "lvcore/threading/thread_pool.hpp"

#include "lvcore/filesystem/filesystem.hpp"

#include "model.hpp"
#include "first_person_camera.hpp"

#include "application.hpp"

//Shadows
#define SHADOW_CASCADE_COUNT 3
#define SHADOW_MAP_SIZE 1024
#define SHADOW_FAR_PLANE 32.0f

const float cascadeLevels[SHADOW_CASCADE_COUNT] = {
    SHADOW_FAR_PLANE * 0.08f, SHADOW_FAR_PLANE * 0.32f, SHADOW_FAR_PLANE
};

//Terrain
#define PATCHES_PER_CHUNK 16
#define PATCH_SIZE 1.0f
#define CHUNK_SIZE (PATCHES_PER_CHUNK * PATCH_SIZE)
#define MAX_TESSELLATION_FACTOR 16
#define CHUNK_TEXTURE_SIZE (PATCHES_PER_CHUNK * MAX_TESSELLATION_FACTOR)
#define CHUNK_VIEW_DISTANCE 6
#define CHUNKS_PER_SIDE (CHUNK_VIEW_DISTANCE * 2 + 1)
#define VIEW_DISTANCE ((CHUNK_VIEW_DISTANCE + 0.5f) * PATCHES_PER_CHUNK * PATCH_SIZE)
#define TERRAIN_HEIGHT 24.0f

//Tree
#define TREE_CELLS_PER_SIDE 6
#define TREE_AO_RADIUS 16.0f
#define TREE_LOD_COUNT 3
const float treeLodDistances[TREE_LOD_COUNT] = {16.0f, 32.0f, 64.0f};

//Grass
#define GRASS_CELLS_PER_SIDE 64
#define GRASS_RENDER_DISTANCE 32.0f
#define GRASS_MIN_SIZE 0.15f
#define GRASS_MAX_SIZE 0.25f
//TODO: calculate this at runtime
#define GRASS_AO_RADIUS 4.0f

//Movement
#define GRAVITY 10.0f
#define JUMP_HEIGHT 4.0f
#define HEAD_LEVEL 0.8f

struct PCDeferredVP {
    glm::mat4 invViewProj;
    glm::mat4 shadowViewProjs[SHADOW_CASCADE_COUNT];
    glm::vec3 viewPos;
};

struct BasicModel {
    glm::mat4 model;
    glm::mat4 normalMatrix;
};

struct ShadowRenderPass {
    lv::Subpass* subpass;
    lv::RenderPass* renderPass;
    lv::Framebuffer* framebuffer;
    lv::CommandBuffer* commandBuffer;

    lv::Image* depthImage;
    lv::Sampler* depthSampler;
};

struct MainRenderPass {
    lv::Subpass* gbufferSubpass;
    lv::Subpass* deferredSubpass;
    lv::RenderPass* renderPass;
    lv::Framebuffer* framebuffer;
    lv::CommandBuffer* commandBuffer;

    lv::Image* colorImage;
    lv::Image* diffuseImage;
    lv::Image* normalImage;
    lv::Image* depthImage;
    lv::Image* depthAsColorImage; //Metal only
};

glm::mat4 createMatrixFromDirection(const glm::vec3& direction) {
    glm::vec3 up(0.0f, 1.0f, 0.0f);  // Up vector (assuming Y-up)

    // Calculate the right vector perpendicular to the direction and up vectors
    glm::vec3 right = glm::normalize(glm::cross(up, direction));

    // Calculate the new up vector based on the right and direction vectors
    up = glm::cross(direction, right);

    // Create the model matrix from the right, up, and direction vectors
    glm::mat4 modelMatrix(1.0f);
    modelMatrix[0] = glm::vec4(right, 0.0f);
    modelMatrix[1] = glm::vec4(up, 0.0f);
    modelMatrix[2] = glm::vec4(direction, 0.0f);

    return modelMatrix;
}

struct Grass {
    glm::vec3 position;
    float angle;
};

class TerrainChunk {
public:
    glm::vec3 position;
    int chunkX, chunkZ;

    glm::vec3 center;
    glm::vec2 centerVec2;

    AABB aabb;

    bool loaded = false;
    bool allocated = false; //For future use
    bool shouldRender = false;

    uint16_t noise[CHUNK_TEXTURE_SIZE + 1][CHUNK_TEXTURE_SIZE + 1];//((CHUNK_TEXTURE_SIZE + 1) * (CHUNK_TEXTURE_SIZE + 1));
    std::vector<BasicModel> grassModels;
    std::vector<glm::vec3> treePositions;

    //lv::Buffer* vertexBuffer;
    //lv::Buffer* indexBuffer;
    lv::Buffer* tessellationFactorBuffer;
    lv::Buffer* grassModelsBuffer;

    lv::Image* noiseImage;
    lv::Image* normalImage;

    lv::DescriptorSet* terrainDescriptorSet;
    lv::DescriptorSet* grassDescriptorSet;

public:
    TerrainChunk(glm::vec3 aPosition, int aChunkX, int aChunkZ) : position(aPosition), chunkX(aChunkX), chunkZ(aChunkZ), center(position + glm::vec3(CHUNK_SIZE / 2.0f, 0.0f, CHUNK_SIZE / 2.0f)), centerVec2(center.x, center.z), aabb{center + glm::vec3(0.0f, TERRAIN_HEIGHT / 2, 0.0f), glm::vec3(CHUNK_SIZE, TERRAIN_HEIGHT, CHUNK_SIZE)} {}

    ~TerrainChunk() {
        if (loaded) {
            delete tessellationFactorBuffer;
            delete noiseImage;
            delete normalImage;
        }
    }

    void load(lv::Device* device, lv::CommandBuffer* commandBuffer, lv::PipelineLayout* tescPipelineLayout, lv::PipelineLayout* terrainPipelineLayout, lv::PipelineLayout* grassPipelineLayout, lv::Sampler* sampler, lv::Sampler* linearSampler, siv::PerlinNoise* perlinNoiseGenerator) {
        //std::vector<MainVertex> vertices((depth + 1) * (width + 1) + 1);
        //std::vector<uint16_t> indices(depth * width * 2 * 3);
        uint16_t noiseWithBorders[CHUNK_TEXTURE_SIZE + 3][CHUNK_TEXTURE_SIZE + 3];//((CHUNK_TEXTURE_SIZE + 3) * (CHUNK_TEXTURE_SIZE + 3));
        glm::vec3 originalNormals[CHUNK_TEXTURE_SIZE + 1][CHUNK_TEXTURE_SIZE + 1];
        glm::i8vec4 normals[CHUNK_TEXTURE_SIZE + 1][CHUNK_TEXTURE_SIZE + 1];//((CHUNK_TEXTURE_SIZE + 1) * (CHUNK_TEXTURE_SIZE + 1));
        float aoMap[CHUNK_TEXTURE_SIZE + 1][CHUNK_TEXTURE_SIZE + 1];// = {1.0f};
        //memset(aoMap, 1.0f, sizeof(aoMap));

//#define SAMPLE(x, z) (z * (CHUNK_TEXTURE_SIZE + 1) + x)
//#define SAMPLE_WITH_BORDERS(x, z) ((z + 1) * (CHUNK_TEXTURE_SIZE + 3) + (x + 1))

        for (int32_t z = -1; z <= CHUNK_TEXTURE_SIZE + 1; z++) {
            for (int32_t x = -1; x <= CHUNK_TEXTURE_SIZE + 1; x++) {
                double noise1 = perlinNoiseGenerator->octave2D_01((chunkX + x / (float)CHUNK_TEXTURE_SIZE) * 0.2f, (chunkZ + z / (float)CHUNK_TEXTURE_SIZE) * 0.2f, 8);
                noiseWithBorders[z + 1][x + 1] = noise1 * 65536;
            }
        }

        for (uint16_t z = 0; z <= CHUNK_TEXTURE_SIZE; z++) {
            for (uint16_t x = 0; x <= CHUNK_TEXTURE_SIZE; x++) {
                noise[z][x] = noiseWithBorders[z + 1][x + 1];
                aoMap[z][x] = 1.0f;
            }
        }

        for (uint16_t z = 0; z <= CHUNK_TEXTURE_SIZE; z++) {
            for (uint16_t x = 0; x <= CHUNK_TEXTURE_SIZE; x++) {
                float hL = noiseWithBorders[z + 1][x + 1 - 1] / 65536.0f * TERRAIN_HEIGHT;
                float hR = noiseWithBorders[z + 1][x + 1 + 1] / 65536.0f * TERRAIN_HEIGHT;
                float hD = noiseWithBorders[z + 1 - 1][x + 1] / 65536.0f * TERRAIN_HEIGHT;
                float hU = noiseWithBorders[z + 1 + 1][x + 1] / 65536.0f * TERRAIN_HEIGHT;

                glm::vec3 normal = glm::vec3((hR - hL) * CHUNK_TEXTURE_SIZE / PATCHES_PER_CHUNK, 2.0f, (hU - hD) * CHUNK_TEXTURE_SIZE / PATCHES_PER_CHUNK);
                originalNormals[z][x] = glm::normalize(normal);
            }
        }
    
#define CREATE_AO_CIRCLE(posX, posZ, radius) \
for (int8_t z2 = -radius; z2 <= radius; z2++) { \
    for (int8_t x2 = -radius; x2 <= radius; x2++) { \
        float ao = glm::length(glm::vec2(x2 / radius, z2 / radius)); \
        int32_t locX = posX + x2; \
        int32_t locZ = posZ + z2; \
        /*TODO: set it for the neighbouring chunk instead*/ \
        if (locZ > 0 && locZ < CHUNK_TEXTURE_SIZE + 1 && locX > 0 && locX < CHUNK_TEXTURE_SIZE + 1) \
            aoMap[locZ][locX] = std::min(aoMap[locZ][locX], 0.3f + ao * 0.7f); \
    } \
}

        for (int32_t z = 0; z < GRASS_CELLS_PER_SIDE; z++) {
            for (int32_t x = 0; x < GRASS_CELLS_PER_SIDE; x++) {
                double grassNoise = perlinNoiseGenerator->octave2D_01((chunkX + x / (float)GRASS_CELLS_PER_SIDE + 0.5f) * 2.0f, (chunkZ + z / (float)GRASS_CELLS_PER_SIDE + 0.5f) * 2.0f, 8);
                double grassNoise2 = perlinNoiseGenerator->octave2D_01((chunkX + x / (float)GRASS_CELLS_PER_SIDE + 0.3f) * 4.0f, (chunkZ + z / (float)GRASS_CELLS_PER_SIDE + 0.3f) * 4.0f, 8);
                if (grassNoise > 0.3f && (grassNoise2 > 0.5f || rand() % 3 == 0)) {
                    int32_t offsetX = rand() % int(CHUNK_TEXTURE_SIZE / GRASS_CELLS_PER_SIDE / 2);
                    int32_t offsetZ = rand() % int(CHUNK_TEXTURE_SIZE / GRASS_CELLS_PER_SIDE / 2);

                    int32_t posX = x / (float)GRASS_CELLS_PER_SIDE * CHUNK_TEXTURE_SIZE + offsetX;
                    int32_t posZ = z / (float)GRASS_CELLS_PER_SIDE * CHUNK_TEXTURE_SIZE + offsetZ;

                    float height = noise[posZ][posX] / 65536.0f * TERRAIN_HEIGHT;
                    //glm::vec3 normal = originalNormals[posZ][posX];
                    //normal = glm::normalize(glm::vec3(normal.x, normal.y / TERRAIN_HEIGHT, normal.z));

                    float size = GRASS_MIN_SIZE + ((rand() % 1000) / 1000.0f) * (GRASS_MAX_SIZE - GRASS_MIN_SIZE);

                    Grass grass{{position.x + x / (float)GRASS_CELLS_PER_SIDE * CHUNK_SIZE + offsetX / (float)CHUNK_TEXTURE_SIZE * CHUNK_SIZE, height + size, position.z + z / (float)GRASS_CELLS_PER_SIDE * CHUNK_SIZE + offsetZ / (float)CHUNK_TEXTURE_SIZE * CHUNK_SIZE}, float(rand() % 360)};

                    BasicModel model;
                    model.model = glm::translate(glm::mat4(1.0f), grass.position) *
                        glm::scale(glm::vec3(size));
                    //model.model *= createMatrixFromDirection(normal);
                    model.model *= glm::rotate(glm::radians(grass.angle), glm::vec3(0.0f, 1.0f, 0.0f));
                    model.normalMatrix = glm::mat4(glm::transpose(glm::inverse(glm::mat3(model.model))));
                    grassModels.push_back(model);

                    CREATE_AO_CIRCLE(posX, posZ, GRASS_AO_RADIUS);
                }
            }
        }

        for (int32_t z = 0; z < TREE_CELLS_PER_SIDE; z++) {
            for (int32_t x = 0; x < TREE_CELLS_PER_SIDE; x++) {
                if (rand() % 3 == 0) {
                    int32_t offsetX = rand() % int(CHUNK_TEXTURE_SIZE / TREE_CELLS_PER_SIDE / 2);
                    int32_t offsetZ = rand() % int(CHUNK_TEXTURE_SIZE / TREE_CELLS_PER_SIDE / 2);

                    int32_t posX = x / (float)TREE_CELLS_PER_SIDE * CHUNK_TEXTURE_SIZE + offsetX;
                    int32_t posZ = z / (float)TREE_CELLS_PER_SIDE * CHUNK_TEXTURE_SIZE + offsetZ;

                    float height = noise[posZ][posX] / 65536.0f * TERRAIN_HEIGHT;

                    glm::vec3 treePos{position.x + x / (float)TREE_CELLS_PER_SIDE * CHUNK_SIZE + offsetX / (float)CHUNK_TEXTURE_SIZE * CHUNK_SIZE, height, position.z + z / (float)TREE_CELLS_PER_SIDE * CHUNK_SIZE + offsetZ / (float)CHUNK_TEXTURE_SIZE * CHUNK_SIZE};

                    treePositions.push_back(treePos);

                    CREATE_AO_CIRCLE(posX, posZ, TREE_AO_RADIUS);
                }
            }
        }

#undef CREATE_AO_CIRCLE

        for (uint16_t z = 0; z <= CHUNK_TEXTURE_SIZE; z++) {
            for (uint16_t x = 0; x <= CHUNK_TEXTURE_SIZE; x++) {
                const glm::vec3& originalNormal = originalNormals[z][x];
                normals[z][x] = glm::u8vec4(originalNormal.x * 127, originalNormal.y * 127, originalNormal.z * 127, aoMap[z][x] * 127);
            }
        }

        //for (uint16_t z = 0; z < depth; z++) {
        //    for (uint16_t x = 0; x < width; x++) {
        //        uint16_t vertexIndex = z * (width + 1) + x;
        //        uint16_t upperVertexIndex = (z + 1) * (width + 1) + x;
        //        uint32_t i = (z * width + x) * 2 * 3;
        //        indices[i + 0] = vertexIndex;
        //        indices[i + 1] = upperVertexIndex;
        //        indices[i + 2] = upperVertexIndex + 1;
        //        indices[i + 3] = vertexIndex;
        //        indices[i + 4] = upperVertexIndex + 1;
        //        indices[i + 5] = vertexIndex + 1;
        //    }
        //}
        
        tessellationFactorBuffer = device->createBuffer({
            .size = PATCHES_PER_CHUNK * PATCHES_PER_CHUNK * size_t(12/* 4 * half (2) + 2 * half (2) */),
            .usage = lv::BufferUsageFlags::StorageBuffer
        });
        
        grassModelsBuffer = device->createBuffer({
            .frameCount = 1,
            .size = grassModels.size() * sizeof(BasicModel),
            .usage = lv::BufferUsageFlags::StorageBuffer
        });
        commandBuffer->cmdStagingCopyDataToBuffer(grassModelsBuffer, grassModels.data());

        noiseImage = device->createImage({
            .frameCount = 1,
            .format = lv::Format::R16Unorm,
            .width = uint16_t(CHUNK_TEXTURE_SIZE + 1),
            .height = uint16_t(CHUNK_TEXTURE_SIZE + 1),
            .usage = lv::ImageUsageFlags::Sampled | lv::ImageUsageFlags::TransferDestination
        });
        commandBuffer->cmdStagingCopyDataToImage(noiseImage, noise, 2);

        normalImage = device->createImage({
            .frameCount = 1,
            .format = lv::Format::RGBA8Snorm,
            .width = uint16_t(CHUNK_TEXTURE_SIZE + 1),
            .height = uint16_t(CHUNK_TEXTURE_SIZE + 1),
            .usage = lv::ImageUsageFlags::Sampled | lv::ImageUsageFlags::TransferDestination
        });
        commandBuffer->cmdStagingCopyDataToImage(normalImage, normals, 4);

        terrainDescriptorSet = device->createDescriptorSet({
            .pipelineLayout = terrainPipelineLayout,
            .layoutIndex = 1,
            .imageBindings = {
                sampler->descriptorInfo(noiseImage, 0),
                sampler->descriptorInfo(normalImage, 1),
                linearSampler->descriptorInfo(normalImage, 2)
            }
        });

        grassDescriptorSet = device->createDescriptorSet({
            .pipelineLayout = grassPipelineLayout,
            .layoutIndex = 1,
            .bufferBindings = {
                grassModelsBuffer->descriptorInfo(0)
            }
        });

        loaded = true;
    }

    float getHeightAtCoords(const glm::i32vec2& coords) {
        return noise[coords.y][coords.x] / 65536.0f * TERRAIN_HEIGHT;
    }

    void dispatchTessellationControl(lv::CommandBuffer* commandBuffer, const glm::vec3& cameraPosition) {
        //commandBuffer->cmdBindDescriptorSet(tescDescriptorSet);
        commandBuffer->cmdPushConstants(&position, 0);

        commandBuffer->cmdBindTessellationFactorBufferForWriting(tessellationFactorBuffer);
        commandBuffer->cmdDispatchTessellationControl(PATCHES_PER_CHUNK * PATCHES_PER_CHUNK);
    }

    void render(lv::CommandBuffer* commandBuffer) {
        commandBuffer->cmdBindDescriptorSet(terrainDescriptorSet);

        commandBuffer->cmdPushConstants(&position, 0);

        commandBuffer->cmdBindTessellationFactorBuffer(tessellationFactorBuffer);

        commandBuffer->cmdDrawPatches(4, PATCHES_PER_CHUNK * PATCHES_PER_CHUNK);
    }

    void writeTreeModelsToBuffer(lv::Buffer* buffers[], uint32_t lodCounts[], const glm::vec3& cameraPosition) {
        if (glm::distance2(center, cameraPosition) < pow(treeLodDistances[2] + sqrt(CHUNK_SIZE * CHUNK_SIZE / 2 + CHUNK_SIZE * CHUNK_SIZE / 2), 2)) {
            for (auto& treePos : treePositions) {
                float dist2 = glm::distance2(treePos, cameraPosition);
                uint8_t lod = 3; //TODO: add impostors
                for (uint8_t i = 0; i < TREE_LOD_COUNT; i++) {
                    if (dist2 < treeLodDistances[i] * treeLodDistances[i]) {
                        lod = i;
                        break;
                    }
                }
                if (lod < 3) {
                    BasicModel model;
                    model.model = glm::translate(glm::mat4(1.0f), treePos) * glm::scale(glm::mat4(1.0f), glm::vec3(0.014f));
                    model.normalMatrix = glm::mat4(glm::transpose(glm::inverse(glm::mat3(model.model))));

                    buffers[lod]->copyDataTo(&model, sizeof(BasicModel), lodCounts[lod]++ * sizeof(BasicModel));
                }
            }
        }
    }

    void renderGrass(lv::CommandBuffer* commandBuffer, lv::Buffer* quadIndexBuffer, const glm::vec3& cameraPosition) {
        commandBuffer->cmdBindDescriptorSet(grassDescriptorSet);
        if (glm::distance2(center, cameraPosition) < GRASS_RENDER_DISTANCE * GRASS_RENDER_DISTANCE) {
            commandBuffer->cmdDrawIndexed(quadIndexBuffer, lv::IndexType::Uint16, quadIndexBuffer->size() / sizeof(uint16_t), grassModels.size());
        }
    }

    //Getters
    inline const glm::vec2& getCenterVec2() {
        return centerVec2;
    }

    inline int x() {
        return chunkX;
    }

    inline int z() {
        return chunkZ;
    }

    inline bool getShouldRender() {
        return shouldRender;
    }

    inline const AABB& getAABB() {
        return aabb;
    }

    inline bool isLoaded() {
        return loaded;
    }

    //Setters
    inline void setShouldRender(bool aShouldRender) {
        shouldRender = aShouldRender;
    }
};

class Terrain {
private:
    glm::i32vec2 cameraChunk = glm::i32vec2(1024); //HACK: some random number so that the chunks update right at the beginning

    std::vector<TerrainChunk*> chunks;
    TerrainChunk* visibleChunks[CHUNKS_PER_SIDE][CHUNKS_PER_SIDE];

    siv::PerlinNoise::seed_type seed;
	siv::PerlinNoise* perlinNoiseGenerator;

    lv::CommandBuffer* newChunkCommandBuffer;

    //Buffers
    lv::Buffer* quadIndexBuffer;

    //Textures
    Texture* terrainGrassTexture;
    Texture* terrainGrassAlphaCardTexture;

    //Tree
    Model treeModels[3];
    lv::Buffer* treeBuffers[3];
    lv::DescriptorSet* treeDescriptorSets[3];
    lv::DescriptorSet* treeShadowDescriptorSets[3];
    uint32_t treeLodCounts[3] = {0};

    //Queues
    std::vector<TerrainChunk*> creationQueue;

    //Arguments
    lv::PipelineLayout* tescPipelineLayout;
    lv::PipelineLayout* terrainPipelineLayout;
    lv::PipelineLayout* grassPipelineLayout;
    lv::Sampler* sampler;
    lv::Sampler* linearSampler;

public:
    Terrain(lv::Device* device, lv::CommandBuffer* commandBuffer, lv::PipelineLayout* aTescPipelineLayout, lv::PipelineLayout* shadowPipelineLayout, lv::PipelineLayout* shadowWithAlphaPipelineLayout, lv::PipelineLayout* aTerrainPipelineLayout, lv::PipelineLayout* gbufferPipelineLayout, lv::PipelineLayout* treePipelineLayout, lv::PipelineLayout* aGrassPipelineLayout, lv::Buffer* mainUniformBuffer, lv::Sampler* aSampler, lv::Sampler* aLinearSampler) : tescPipelineLayout(aTescPipelineLayout), terrainPipelineLayout(aTerrainPipelineLayout), grassPipelineLayout(aGrassPipelineLayout), sampler(aSampler), linearSampler(aLinearSampler) {
        uint16_t quadIndices[] = {
            0, 1, 2,
            0, 2, 3
        };
        quadIndexBuffer = device->createBuffer({
            .frameCount = 1,
            .size = sizeof(quadIndices)
        });
        commandBuffer->cmdStagingCopyDataToBuffer(quadIndexBuffer, quadIndices);

        terrainGrassTexture = new Texture();
        terrainGrassTexture->init(device, commandBuffer, "../examples/assets/textures/grass/grass.jpg", true, true);
        terrainGrassAlphaCardTexture = new Texture();
        terrainGrassAlphaCardTexture->init(device, commandBuffer, "../examples/assets/textures/grass/grass_alpha_card.png", true, true);

        seed = (unsigned int)(rand() % 10000);
        perlinNoiseGenerator = new siv::PerlinNoise{seed};

        //Models

        //LOD 0
        treeModels[0].init(device, commandBuffer, gbufferPipelineLayout, shadowWithAlphaPipelineLayout, "../examples/assets/models/pine/Pine_4m.obj", 1, 1, 2);

        //LOD 1
        treeModels[1].init(device, commandBuffer, gbufferPipelineLayout, shadowWithAlphaPipelineLayout, "../examples/assets/models/pine/Pine_4m_lod1.obj", 1, 1, 2);

        //LOD 2
        treeModels[2].init(device, commandBuffer, gbufferPipelineLayout, shadowWithAlphaPipelineLayout, "../examples/assets/models/pine/Pine_4m_lod2.obj", 1, 1, 2);

        for (uint8_t i = 0; i < 3; i++) {
            treeBuffers[i] = device->createBuffer({
                .size = maxVisibleChunks() * TREE_CELLS_PER_SIDE * TREE_CELLS_PER_SIDE * sizeof(BasicModel),
                .usage = lv::BufferUsageFlags::StorageBuffer,
                .memoryType = lv::MemoryType::Shared
            });

            treeDescriptorSets[i] = device->createDescriptorSet({
                .pipelineLayout = treePipelineLayout,
                .bufferBindings = {
                    mainUniformBuffer->descriptorInfo(0),
                    treeBuffers[i]->descriptorInfo(1)
                }
            });

            treeShadowDescriptorSets[i] = device->createDescriptorSet({
                .pipelineLayout = (i == 0 ? shadowWithAlphaPipelineLayout : shadowPipelineLayout),
                .layoutIndex = 1,
                .bufferBindings = {
                    treeBuffers[i]->descriptorInfo(0)
                }
            });
        }

        newChunkCommandBuffer = device->createCommandBuffer({});
    }

    ~Terrain() {
        for (auto* chunk : chunks)
            delete chunk;
        delete terrainGrassTexture;
        delete terrainGrassAlphaCardTexture;
        delete quadIndexBuffer;

        for (uint8_t i = 0; i < 3; i++)
            treeModels[i].destroy();
    }

    bool chunkIsVisibleRel(const glm::i32vec2& relCoords) {
        return (relCoords.x >= -CHUNK_VIEW_DISTANCE && relCoords.x <= CHUNK_VIEW_DISTANCE && relCoords.y >= -CHUNK_VIEW_DISTANCE && relCoords.y <= CHUNK_VIEW_DISTANCE);
    }

    bool chunkIsVisible(const glm::i32vec2& coords) {
        return chunkIsVisibleRel(coords - cameraChunk);
    }

    TerrainChunk* getChunkByCoords(const glm::i32vec2& coords) {
        glm::i32vec2 relCoords = coords - cameraChunk;

        if (chunkIsVisibleRel(relCoords))
            return visibleChunks[relCoords.y + CHUNK_VIEW_DISTANCE][relCoords.x + CHUNK_VIEW_DISTANCE];

        return nullptr;
    }

#define FOR_EACH_VISIBLE_CHUNK_BEGIN \
for (uint8_t z = 0; z < CHUNKS_PER_SIDE; z++) { \
    for (uint8_t x = 0; x < CHUNKS_PER_SIDE; x++) { \
        TerrainChunk* chunk = visibleChunks[z][x]; \
        if (chunk) { \
            if (chunk->getShouldRender()) {

#define FOR_EACH_VISIBLE_CHUNK_END \
            } \
        } \
    } \
}

    void updateVisibleChunks(lv::Device* device, Camera& camera, const glm::vec2& cameraPositionVec2) {
        glm::i32vec2 crntCameraChunk = glm::round(cameraPositionVec2 / CHUNK_SIZE);

        if (creationQueue.size()) {
            newChunkCommandBuffer->beginRecording();
            newChunkCommandBuffer->beginBlitCommands();

            creationQueue[0]->load(device, newChunkCommandBuffer, tescPipelineLayout, terrainPipelineLayout, grassPipelineLayout, sampler, linearSampler, perlinNoiseGenerator);
            creationQueue.erase(creationQueue.begin());

            newChunkCommandBuffer->endRecording();
            newChunkCommandBuffer->submit();
        }

        if (cameraChunk.x != crntCameraChunk.x || cameraChunk.y != crntCameraChunk.y) {
            cameraChunk = crntCameraChunk;

            for (uint8_t z = 0; z < CHUNKS_PER_SIDE; z++) {
                for (uint8_t x = 0; x < CHUNKS_PER_SIDE; x++) {
                    visibleChunks[z][x] = nullptr;
                }
            }

            for (auto* chunk : chunks) {
                glm::i32vec2 relPos{chunk->x() - cameraChunk.x, chunk->z() - cameraChunk.y};
                if (chunkIsVisibleRel(relPos)) {
                    visibleChunks[relPos.y + CHUNK_VIEW_DISTANCE][relPos.x + CHUNK_VIEW_DISTANCE] = chunk;
                }
            }

            for (uint8_t z = 0; z < CHUNKS_PER_SIDE; z++) {
                for (uint8_t x = 0; x < CHUNKS_PER_SIDE; x++) {
                    //std::cout << (visibleChunks[z][x] ? "1" : "0") << " ";
                    if (!visibleChunks[z][x]) {
                        TerrainChunk* chunk = createNewChunk(cameraChunk.x - CHUNK_VIEW_DISTANCE + x, cameraChunk.y - CHUNK_VIEW_DISTANCE + z);
                        creationQueue.push_back(chunk);
                        visibleChunks[z][x] = chunk;
                        chunks.push_back(chunk);
                    }
                }
                //std::cout << std::endl;
            }
            //std::cout << std::endl;
        }

        for (uint8_t z = 0; z < CHUNKS_PER_SIDE; z++) {
            for (uint8_t x = 0; x < CHUNKS_PER_SIDE; x++) {
                TerrainChunk* chunk = visibleChunks[z][x];
                chunk->setShouldRender(chunk->isLoaded() && glm::distance2(glm::vec2(chunk->x(), chunk->z()), glm::vec2(cameraChunk)) < CHUNK_VIEW_DISTANCE * CHUNK_VIEW_DISTANCE && camera.frustum.intersectsWithAABB(chunk->getAABB()));
            }
        }
    }

    TerrainChunk* createNewChunk(int x, int z) {
        return new TerrainChunk(glm::vec3(x - 0.5f, 0.0f, z - 0.5f) * (float)PATCHES_PER_CHUNK * PATCH_SIZE, x, z);
    }

    float getHeightAtPosition(const glm::vec2& position) {
        //Translate to chunk
        glm::vec2 coords = position / CHUNK_SIZE;
        glm::i32vec2 intCoords = glm::round(coords);

        if (TerrainChunk* chunk = getChunkByCoords(intCoords)) {
            //Translate to chunk texture units
            glm::i32vec2 posInTextureUnits = glm::i32vec2((coords - glm::vec2(cameraChunk)) * (float)CHUNK_TEXTURE_SIZE) + glm::i32vec2(CHUNK_TEXTURE_SIZE / 2);

            return chunk->getHeightAtCoords(posInTextureUnits);
        }

        return -1.0f;
    }

    void dispatchTessellationControl(lv::CommandBuffer* commandBuffer, const glm::vec3& cameraPosition) {
        FOR_EACH_VISIBLE_CHUNK_BEGIN
            chunk->dispatchTessellationControl(commandBuffer, cameraPosition);
        FOR_EACH_VISIBLE_CHUNK_END
    }

    void render(lv::CommandBuffer* commandBuffer) {
        FOR_EACH_VISIBLE_CHUNK_BEGIN
            chunk->render(commandBuffer);
        FOR_EACH_VISIBLE_CHUNK_END
    }

    void writeTreeModelsToBuffer(const glm::vec3& cameraPosition) {
        for (uint8_t i = 0; i < 3; i++)
            treeLodCounts[i] = 0;

        FOR_EACH_VISIBLE_CHUNK_BEGIN
            chunk->writeTreeModelsToBuffer(treeBuffers, treeLodCounts, cameraPosition);
        FOR_EACH_VISIBLE_CHUNK_END
    }

    void renderTrees(lv::CommandBuffer* commandBuffer) {
        for (uint8_t i = 0; i < 3; i++) {
            if (treeLodCounts[i]) {
                commandBuffer->cmdBindDescriptorSet(treeDescriptorSets[i]);
                treeModels[i].render(commandBuffer, treeLodCounts[i], false);
            }
        }
    }

    void renderTreesShadowsWithTextures(lv::CommandBuffer* commandBuffer, const glm::vec3& cameraPosition) {
        if (treeLodCounts[0]) {
            commandBuffer->cmdBindDescriptorSet(treeShadowDescriptorSets[0]);
            treeModels[2].renderShadows(commandBuffer, treeLodCounts[0] * 3, false);
        }
    }

    void renderTreesShadows(lv::CommandBuffer* commandBuffer, const glm::vec3& cameraPosition) {
        for (uint8_t i = 1; i < 3; i++) {
            if (treeLodCounts[i]) {
                commandBuffer->cmdBindDescriptorSet(treeShadowDescriptorSets[i]);
                treeModels[2].renderNoTextures(commandBuffer, treeLodCounts[i] * 3, false);
            }
        }
    }

    void renderGrass(lv::CommandBuffer* commandBuffer, const glm::vec3& cameraPosition) {
        FOR_EACH_VISIBLE_CHUNK_BEGIN
            chunk->renderGrass(commandBuffer, quadIndexBuffer, cameraPosition);
        FOR_EACH_VISIBLE_CHUNK_END
    }

    //Getters
    inline Texture* getTerrainGrassTexture() {
        return terrainGrassTexture;
    }

    inline Texture* getTerrainGrassAlphaCardTexture() {
        return terrainGrassAlphaCardTexture;
    }

    inline uint16_t maxVisibleChunks() {
        return CHUNKS_PER_SIDE * CHUNKS_PER_SIDE;
    }
};


//TODO: destroy shader modules
class LavaCoreExampleApp : public Application {
public:
    lv::ThreadPool* threadPool;
    lv::Instance* instance;
    lv::Device* device;
    lv::SwapChain* swapChain;

    lv::CommandBuffer* tescCommandBuffer;

    lv::Sampler* basicSampler;
    lv::Sampler* linearSampler;

    ShadowRenderPass shadowRenderPass;
    MainRenderPass mainRenderPass;

    lv::PipelineLayout* shadowPipelineLayout;
    lv::PipelineLayout* shadowWithAlphaPipelineLayout;
    lv::PipelineLayout* tescPipelineLayout;
    lv::PipelineLayout* terrainPipelineLayout;
    lv::PipelineLayout* gbufferPipelineLayout;
    lv::PipelineLayout* treePipelineLayout;
    lv::PipelineLayout* grassPipelineLayout;
    lv::PipelineLayout* deferredPipelineLayout;
    lv::PipelineLayout* hdrPipelineLayout;

    lv::ShaderModule* vertShadowShaderModule;
    lv::ShaderModule* fragShadowShaderModule;
    lv::GraphicsPipeline* shadowGraphicsPipeline;

    lv::ShaderModule* vertShadowWithAlphaShaderModule;
    lv::ShaderModule* fragShadowWithAlphaShaderModule;
    lv::GraphicsPipeline* shadowWithAlphaGraphicsPipeline;

    lv::ShaderModule* teseTerrainShaderModule;
    lv::ShaderModule* fragTerrainShaderModule;
    lv::GraphicsPipeline* terrainGraphicsPipeline;

    lv::ShaderModule* vertGBufferShaderModule;
    lv::ShaderModule* fragGBufferShaderModule;
    lv::GraphicsPipeline* gbufferGraphicsPipeline;

    lv::ShaderModule* vertTreeShaderModule;
    lv::GraphicsPipeline* treeGraphicsPipeline;

    lv::ShaderModule* vertQuadShaderModule;
    lv::ShaderModule* fragGrassShaderModule;
    lv::GraphicsPipeline* grassGraphicsPipeline;

    lv::ShaderModule* vertTriangleShaderModule;
    lv::ShaderModule* fragDeferredShaderModule;
    lv::GraphicsPipeline* deferredGraphicsPipeline;

    lv::ShaderModule* fragHdrShaderModule;
    lv::GraphicsPipeline* hdrGraphicsPipeline;

    lv::ShaderModule* tescTerrainShaderModule;
    lv::ComputePipeline* terrainTescComputePipeline;

    lv::VertexDescriptor* shadowVertexDescriptor;
    lv::VertexDescriptor* shadowWithAlphaVertexDescriptor;
    lv::VertexDescriptor* mainVertexDescriptor;
    lv::VertexDescriptor* terrainVertexDescriptor;
    
    lv::Buffer* shadowUniformBuffer;
    lv::Buffer* mainUniformBuffer;
    lv::Buffer* tescUniformBuffer;
    lv::Buffer* controlPointsBuffer;

    lv::DescriptorSet* shadowDescriptorSet;
    lv::DescriptorSet* shadowWithAlphaDescriptorSet;
    lv::DescriptorSet* tescDescriptorSet;
    lv::DescriptorSet* terrainDescriptorSet;
    lv::DescriptorSet* gbufferDescriptorSet;
    lv::DescriptorSet* grassDescriptorSet;
    lv::DescriptorSet* deferredDescriptorSet;
    lv::DescriptorSet* hdrDescriptorSet;

    FirstPersonCamera camera;
    Terrain* terrain;

    PCDeferredVP deferredVP;

    //Movement
    float yMomentum = 0.0f;
    bool isOnGround = true;

    float prevTime = 0.0f;

    LavaCoreExampleApp(int argc, char* argv[]) : Application("terrain_rendering", argc, argv) {
        threadPool = new lv::ThreadPool({});

        instance = new lv::Instance({
            .applicationName = exampleName.c_str(),
            .validationEnable = lv::True
        });

        device = instance->createDevice({
            .window = window,
            .threadPool = threadPool
        });

        swapChain = device->createSwapChain({
            .window = window,
            .vsyncEnable = lv::True,
            .maxFramesInFlight = 2
        });

        uint16_t framebufferWidth, framebufferHeight;
        lvndWindowGetFramebufferSize(window, &framebufferWidth, &framebufferHeight);

        //Vertex descriptor
        shadowVertexDescriptor = device->createVertexDescriptor({
            .size = sizeof(MainVertex),
            .bindings = {
                {0, lv::Format::RGB32Float, offsetof(MainVertex, position)}
            }
        });

        shadowWithAlphaVertexDescriptor = device->createVertexDescriptor({
            .size = sizeof(MainVertex),
            .bindings = {
                {0, lv::Format::RGB32Float, offsetof(MainVertex, position)},
                {1, lv::Format::RG32Float, offsetof(MainVertex, texCoord)}
            }
        });

        mainVertexDescriptor = device->createVertexDescriptor({
            .size = sizeof(MainVertex),
            .bindings = {
                {0, lv::Format::RGB32Float, offsetof(MainVertex, position)},
                {1, lv::Format::RG32Float, offsetof(MainVertex, texCoord)},
                {2, lv::Format::RGB32Float, offsetof(MainVertex, normal)}
            }
        });

        terrainVertexDescriptor = device->createVertexDescriptor({
            .size = sizeof(float) * 4,
            .inputRate = lv::VertexInputRate::PerPatchControlPoint,
            .bindings = {
                {0, lv::Format::RGB32Float, 0}
            }
        });

        //Command buffers
        tescCommandBuffer = device->createCommandBuffer({});

        //Samplers
        basicSampler = device->createSampler({});
        linearSampler = device->createSampler({
            .filter = lv::Filter::Linear
        });

        //Render passes

        //Shadow
        shadowRenderPass.depthImage = device->createImage({
            .format = lv::Format::D32Float,
            .width = SHADOW_MAP_SIZE,
            .height = SHADOW_MAP_SIZE,
            .layerCount = SHADOW_CASCADE_COUNT,
            .imageType = lv::ImageType::_2DArray,
            .usage = lv::ImageUsageFlags::Sampled | lv::ImageUsageFlags::DepthStencilAttachment,
            .aspect = lv::ImageAspectFlags::Depth
        });
        shadowRenderPass.depthSampler = device->createSampler({
            .filter = lv::Filter::Linear,
            .compareEnable = lv::True
        });

        shadowRenderPass.subpass = device->createSubpass({
            .depthAttachment = {0, lv::ImageLayout::DepthStencilAttachmentOptimal}
        });

        shadowRenderPass.renderPass = device->createRenderPass({
            .subpasses = {shadowRenderPass.subpass},
            .attachments = {
                {
                    .index = 0,
                    .format = shadowRenderPass.depthImage->format(),
                    .loadOp = lv::AttachmentLoadOperation::Clear,
                    .storeOp = lv::AttachmentStoreOperation::Store,
                    .finalLayout = lv::ImageLayout::ShaderReadOnlyOptimal
                }
            }
        });


        shadowRenderPass.framebuffer = device->createFramebuffer({
            .renderPass = shadowRenderPass.renderPass,
            .depthAttachment = {0, shadowRenderPass.depthImage}
        });
        
        shadowRenderPass.commandBuffer = device->createCommandBuffer({});

        //Main
        mainRenderPass.colorImage = device->createImage({
            .format = lv::Format::RGBA16Float,
            .width = framebufferWidth,
            .height = framebufferHeight,
            .usage = lv::ImageUsageFlags::Sampled | lv::ImageUsageFlags::ColorAttachment
        });

        mainRenderPass.diffuseImage = device->createImage({
            .format = lv::Format::RGBA8Unorm,
            .width = framebufferWidth,
            .height = framebufferHeight,
            .usage = lv::ImageUsageFlags::Sampled | lv::ImageUsageFlags::ColorAttachment,
            .memoryType = lv::MemoryType::Memoryless
        });

        mainRenderPass.normalImage = device->createImage({
            .format = lv::Format::RGBA16Snorm,
            .width = framebufferWidth,
            .height = framebufferHeight,
            .usage = lv::ImageUsageFlags::Sampled | lv::ImageUsageFlags::ColorAttachment,
            .memoryType = lv::MemoryType::Memoryless
        });

        mainRenderPass.depthImage = device->createImage({
            .format = lv::Format::D32Float,
            .width = framebufferWidth,
            .height = framebufferHeight,
            .usage = lv::ImageUsageFlags::Sampled | lv::ImageUsageFlags::DepthStencilAttachment,
            .aspect = lv::ImageAspectFlags::Depth
        });

        if (instance->getRenderAPI() == lv::RenderAPI::Metal) {
            mainRenderPass.depthAsColorImage = device->createImage({
                .format = lv::Format::R32Float,
                .width = framebufferWidth,
                .height = framebufferHeight,
                .usage = lv::ImageUsageFlags::Sampled | lv::ImageUsageFlags::ColorAttachment,
                .memoryType = lv::MemoryType::Memoryless
            });
        }
        
        lv::SubpassCreateInfo gbufferSubpassCI{
            .colorAttachments = {
                {1, lv::ImageLayout::ColorAttachmentOptimal},
                {2, lv::ImageLayout::ColorAttachmentOptimal}
            },
            .depthAttachment = {3, lv::ImageLayout::DepthStencilAttachmentOptimal}
        };
        if (instance->getRenderAPI() == lv::RenderAPI::Metal)
            gbufferSubpassCI.colorAttachments.push_back({4, lv::ImageLayout::ColorAttachmentOptimal});
        mainRenderPass.gbufferSubpass = device->createSubpass(gbufferSubpassCI);

        lv::SubpassCreateInfo deferredSubpassCI{
            .colorAttachments = {
                {0, lv::ImageLayout::ColorAttachmentOptimal},
            },
            .depthAttachment = {3, lv::ImageLayout::DepthStencilAttachmentOptimal},
            .inputAttachments = {
                {1, lv::ImageLayout::ShaderReadOnlyOptimal},
                {2, lv::ImageLayout::ShaderReadOnlyOptimal}
            }
        };
        if (instance->getRenderAPI() == lv::RenderAPI::Vulkan)
            gbufferSubpassCI.inputAttachments.push_back({3, lv::ImageLayout::DepthStencilReadOnlyOptimal});
        else if (instance->getRenderAPI() == lv::RenderAPI::Metal)
            gbufferSubpassCI.inputAttachments.push_back({4, lv::ImageLayout::ShaderReadOnlyOptimal});
        mainRenderPass.deferredSubpass = device->createSubpass(deferredSubpassCI);

        lv::RenderPassCreateInfo mainRenderPassCI{
            .subpasses = {mainRenderPass.gbufferSubpass, mainRenderPass.deferredSubpass},
            .attachments = {
                {
                    .format = mainRenderPass.colorImage->format(),
                    .index = 0,
                    .loadOp = lv::AttachmentLoadOperation::Clear,
                    .storeOp = lv::AttachmentStoreOperation::Store,
                    .finalLayout = lv::ImageLayout::ShaderReadOnlyOptimal
                },
                {
                    .format = mainRenderPass.diffuseImage->format(),
                    .index = 1,
                    .finalLayout = lv::ImageLayout::ShaderReadOnlyOptimal
                },
                {
                    .format = mainRenderPass.normalImage->format(),
                    .index = 2,
                    .finalLayout = lv::ImageLayout::ShaderReadOnlyOptimal
                },
                {
                    .format = mainRenderPass.depthImage->format(),
                    .index = 3,
                    .loadOp = lv::AttachmentLoadOperation::Clear,
                    .initialLayout = lv::ImageLayout::DepthStencilReadOnlyOptimal,
                    .finalLayout = lv::ImageLayout::DepthStencilReadOnlyOptimal
                }
            }
        };
        if (instance->getRenderAPI() == lv::RenderAPI::Metal) {
            mainRenderPassCI.attachments.push_back({
                .format = mainRenderPass.depthAsColorImage->format(),
                .index = 4,
                .finalLayout = lv::ImageLayout::ShaderReadOnlyOptimal
            });
        }
        mainRenderPass.renderPass = device->createRenderPass(mainRenderPassCI);

        lv::FramebufferCreateInfo mainFramebufferCI{
            .renderPass = mainRenderPass.renderPass,
            .colorAttachments = {
                {
                    .index = 0,
                    .image = mainRenderPass.colorImage,
                    .clearValue = {
                        .color = {
                            .float32 = {110 / 255.0f, 156 / 255.0f, 230 / 255.0f, 1.0f}
                        }
                    }
                },
                {1, mainRenderPass.diffuseImage},
                {2, mainRenderPass.normalImage}
            },
            .depthAttachment = {3, mainRenderPass.depthImage}
        };
        if (instance->getRenderAPI() == lv::RenderAPI::Metal)
            mainFramebufferCI.colorAttachments.push_back({4, mainRenderPass.depthAsColorImage});
        mainRenderPass.framebuffer = device->createFramebuffer(mainFramebufferCI);

        mainRenderPass.commandBuffer = device->createCommandBuffer({});

        //Pipeline layout

        //Shadow
        shadowPipelineLayout = device->createPipelineLayout({
            .descriptorSetLayouts = {
                {
                    {0, lv::DescriptorType::UniformBuffer, lv::ShaderStageFlags::Vertex}
                },
                {
                    {0, lv::DescriptorType::StorageBuffer, lv::ShaderStageFlags::Vertex}
                }
            }
        });

        //Shadow with alpha
        shadowWithAlphaPipelineLayout = device->createPipelineLayout({
            .descriptorSetLayouts = {
                {
                    {0, lv::DescriptorType::UniformBuffer, lv::ShaderStageFlags::Vertex}
                },
                {
                    {0, lv::DescriptorType::StorageBuffer, lv::ShaderStageFlags::Vertex}
                },
                {
                    {0, lv::DescriptorType::CombinedImageSampler, lv::ShaderStageFlags::Fragment}
                }
            }
        });

        //Terrain tessellation control
        tescPipelineLayout = device->createPipelineLayout({
            .pushConstantRanges = {
                {
                    .stageFlags = lv::ShaderStageFlags::Compute,
                    .offset = 0,
                    .size = sizeof(glm::vec3)
                }
            },
            .descriptorSetLayouts = {
                {
                    {0, lv::DescriptorType::UniformBuffer, lv::ShaderStageFlags::Compute},
                }
            }
        });

        //Terrain
        terrainPipelineLayout = device->createPipelineLayout({
            .pushConstantRanges = {
                {
                    .stageFlags = lv::ShaderStageFlags::Vertex,
                    .offset = 0,
                    .size = sizeof(glm::vec3)
                }
            },
            .descriptorSetLayouts = {
                {
                    {0, lv::DescriptorType::UniformBuffer, lv::ShaderStageFlags::Vertex},
                    {1, lv::DescriptorType::CombinedImageSampler, lv::ShaderStageFlags::Fragment}
                },
                {
                    {0, lv::DescriptorType::CombinedImageSampler, lv::ShaderStageFlags::Vertex},
                    {1, lv::DescriptorType::CombinedImageSampler, lv::ShaderStageFlags::Vertex},
                    {2, lv::DescriptorType::CombinedImageSampler, lv::ShaderStageFlags::Fragment}
                }
            }
        });

        //GBuffer
        gbufferPipelineLayout = device->createPipelineLayout({
            .pushConstantRanges = {
                {
                    .stageFlags = lv::ShaderStageFlags::Vertex,
                    .offset = 0,
                    .size = sizeof(PCModel)
                }
            },
            .descriptorSetLayouts = {
                {
                    {0, lv::DescriptorType::UniformBuffer, lv::ShaderStageFlags::Vertex}
                },
                {
                    {0, lv::DescriptorType::CombinedImageSampler, lv::ShaderStageFlags::Fragment}
                }
            }
        });

        //Tree
        treePipelineLayout = device->createPipelineLayout({
            .descriptorSetLayouts = {
                {
                    {0, lv::DescriptorType::UniformBuffer, lv::ShaderStageFlags::Vertex},
                    {1, lv::DescriptorType::StorageBuffer, lv::ShaderStageFlags::Vertex}
                },
                {
                    {0, lv::DescriptorType::CombinedImageSampler, lv::ShaderStageFlags::Fragment}
                }
            }
        });

        //Grass
        grassPipelineLayout = device->createPipelineLayout({
            .descriptorSetLayouts = {
                {
                    {0, lv::DescriptorType::UniformBuffer, lv::ShaderStageFlags::Vertex},
                    {1, lv::DescriptorType::CombinedImageSampler, lv::ShaderStageFlags::Fragment}
                },
                {
                    {0, lv::DescriptorType::StorageBuffer, lv::ShaderStageFlags::Vertex}
                }
            }
        });

        //Deferred
        deferredPipelineLayout = device->createPipelineLayout({
            .pushConstantRanges = {
                {
                    .stageFlags = lv::ShaderStageFlags::Fragment,
                    .offset = 0,
                    .size = sizeof(PCDeferredVP)
                }
            },
            .descriptorSetLayouts = {
                {
                    {0, lv::DescriptorType::InputAttachment, lv::ShaderStageFlags::Fragment},
                    {1, lv::DescriptorType::InputAttachment, lv::ShaderStageFlags::Fragment},
                    {2, lv::DescriptorType::InputAttachment, lv::ShaderStageFlags::Fragment},
                    {3, lv::DescriptorType::CombinedImageSampler, lv::ShaderStageFlags::Fragment}
                }
            }
        });

        //HDR
        hdrPipelineLayout = device->createPipelineLayout({
            .descriptorSetLayouts = {
                {
                    {0, lv::DescriptorType::SampledImage, lv::ShaderStageFlags::Fragment}
                }
            }
        });

        //Graphics pipeline

        //Shadow
        vertShadowShaderModule = device->createShaderModule({
            .shaderStage = lv::ShaderStageFlags::Vertex,
            .source = lv::readFile((assetDir + "/shaders/compiled/shadow.vert.json").c_str())
        });

        fragShadowShaderModule = device->createShaderModule({
            .shaderStage = lv::ShaderStageFlags::Fragment,
            .source = lv::readFile((assetDir + "/shaders/compiled/shadow.frag.json").c_str())
        });;

        shadowGraphicsPipeline = device->createGraphicsPipeline({
            .vertexShaderModule = vertShadowShaderModule,
            .fragmentShaderModule = fragShadowShaderModule,
            .pipelineLayout = shadowPipelineLayout,
            .renderPass = shadowRenderPass.renderPass,
            .vertexDescriptor = shadowVertexDescriptor,
            .depthTestEnable = lv::True,
            .cullMode = lv::CullMode::Back
        });

        //Shadow with alpha
        vertShadowWithAlphaShaderModule = device->createShaderModule({
            .shaderStage = lv::ShaderStageFlags::Vertex,
            .source = lv::readFile((assetDir + "/shaders/compiled/shadow_with_alpha.vert.json").c_str())
        });

        fragShadowWithAlphaShaderModule = device->createShaderModule({
            .shaderStage = lv::ShaderStageFlags::Fragment,
            .source = lv::readFile((assetDir + "/shaders/compiled/shadow_with_alpha.frag.json").c_str())
        });;

        shadowWithAlphaGraphicsPipeline = device->createGraphicsPipeline({
            .vertexShaderModule = vertShadowWithAlphaShaderModule,
            .fragmentShaderModule = fragShadowWithAlphaShaderModule,
            .pipelineLayout = shadowWithAlphaPipelineLayout,
            .renderPass = shadowRenderPass.renderPass,
            .vertexDescriptor = shadowWithAlphaVertexDescriptor,
            .depthTestEnable = lv::True,
            .cullMode = lv::CullMode::Back
        });

        //Terrain
        teseTerrainShaderModule = device->createShaderModule({
            .shaderStage = lv::ShaderStageFlags::Vertex,
            .source = lv::readFile((assetDir + "/shaders/compiled/terrain.tese.json").c_str())
        });

        fragTerrainShaderModule = device->createShaderModule({
            .shaderStage = lv::ShaderStageFlags::Fragment,
            .source = lv::readFile((assetDir + "/shaders/compiled/terrain.frag.json").c_str())
        });

        lv::GraphicsPipelineCreateInfo terrainGraphicsPipelineCI{
            .vertexShaderModule = teseTerrainShaderModule,
            .fragmentShaderModule = fragTerrainShaderModule,
            .pipelineLayout = terrainPipelineLayout,
            .renderPass = mainRenderPass.renderPass,
            .vertexDescriptor = terrainVertexDescriptor,
            .depthTestEnable = lv::True,
            .cullMode = lv::CullMode::Back,
            .colorBlendAttachments = {
                {0},
                {1},
                {2}
            }
        };
        if (instance->getRenderAPI() == lv::RenderAPI::Metal)
            terrainGraphicsPipelineCI.colorBlendAttachments.push_back({4});
        terrainGraphicsPipeline = device->createGraphicsPipeline(terrainGraphicsPipelineCI);

        //GBuffer
        vertGBufferShaderModule = device->createShaderModule({
            .shaderStage = lv::ShaderStageFlags::Vertex,
            .source = lv::readFile((assetDir + "/shaders/compiled/gbuffer.vert.json").c_str())
        });

        fragGBufferShaderModule = device->createShaderModule({
            .shaderStage = lv::ShaderStageFlags::Fragment,
            .source = lv::readFile((assetDir + "/shaders/compiled/gbuffer.frag.json").c_str())
        });

        lv::GraphicsPipelineCreateInfo gbufferGraphicsPipelineCI{
            .vertexShaderModule = vertGBufferShaderModule,
            .fragmentShaderModule = fragGBufferShaderModule,
            .pipelineLayout = gbufferPipelineLayout,
            .renderPass = mainRenderPass.renderPass,
            .vertexDescriptor = mainVertexDescriptor,
            .depthTestEnable = lv::True,
            .cullMode = lv::CullMode::Back,
            .colorBlendAttachments = {
                {0},
                {1},
                {2}
            }
        };
        if (instance->getRenderAPI() == lv::RenderAPI::Metal)
            gbufferGraphicsPipelineCI.colorBlendAttachments.push_back({4});
        gbufferGraphicsPipeline = device->createGraphicsPipeline(gbufferGraphicsPipelineCI);

        //Tree
        vertTreeShaderModule = device->createShaderModule({
            .shaderStage = lv::ShaderStageFlags::Vertex,
            .source = lv::readFile((assetDir + "/shaders/compiled/tree.vert.json").c_str())
        });

        lv::GraphicsPipelineCreateInfo treeGraphicsPipelineCI{
            .vertexShaderModule = vertTreeShaderModule,
            .fragmentShaderModule = fragGBufferShaderModule,
            .pipelineLayout = treePipelineLayout,
            .renderPass = mainRenderPass.renderPass,
            .vertexDescriptor = mainVertexDescriptor,
            .depthTestEnable = lv::True,
            //.cullMode = lv::CullMode::Back,
            .colorBlendAttachments = {
                {0},
                {1},
                {2},
            }
        };
        if (instance->getRenderAPI() == lv::RenderAPI::Metal)
            treeGraphicsPipelineCI.colorBlendAttachments.push_back({4});
        treeGraphicsPipeline = device->createGraphicsPipeline(treeGraphicsPipelineCI);

        //Grass
        vertQuadShaderModule = device->createShaderModule({
            .shaderStage = lv::ShaderStageFlags::Vertex,
            .source = lv::readFile((assetDir + "/shaders/compiled/quad.vert.json").c_str())
        });

        fragGrassShaderModule = device->createShaderModule({
            .shaderStage = lv::ShaderStageFlags::Fragment,
            .source = lv::readFile((assetDir + "/shaders/compiled/grass.frag.json").c_str())
        });

        lv::GraphicsPipelineCreateInfo grassGraphicsPipelineCI{
            .vertexShaderModule = vertQuadShaderModule,
            .fragmentShaderModule = fragGrassShaderModule,
            .pipelineLayout = grassPipelineLayout,
            .renderPass = mainRenderPass.renderPass,
            .depthTestEnable = lv::True,
            .colorBlendAttachments = {
                {0},
                {1},
                {2}
            }
        };
        if (instance->getRenderAPI() == lv::RenderAPI::Metal)
            grassGraphicsPipelineCI.colorBlendAttachments.push_back({4});
        grassGraphicsPipeline = device->createGraphicsPipeline(grassGraphicsPipelineCI);

        //Deferred
        vertTriangleShaderModule = device->createShaderModule({
            .shaderStage = lv::ShaderStageFlags::Vertex,
            .source = lv::readFile((assetDir + "/shaders/compiled/triangle.vert.json").c_str())
        });

        fragDeferredShaderModule = device->createShaderModule({
            .shaderStage = lv::ShaderStageFlags::Fragment,
            .source = lv::readFile((assetDir + "/shaders/compiled/deferred.frag.json").c_str())
        });

        lv::GraphicsPipelineCreateInfo deferredGraphicsPipelineCI{
            .vertexShaderModule = vertTriangleShaderModule,
            .fragmentShaderModule = fragDeferredShaderModule,
            .pipelineLayout = deferredPipelineLayout,
            .renderPass = mainRenderPass.renderPass,
            .subpassIndex = 1,
            .depthTestEnable = lv::True,
            .depthWriteEnable = lv::False,
            .depthOp = lv::CompareOperation::NotEqual,
            .colorBlendAttachments = {
                {
                    .index = 0,
                    .blendEnable = lv::True
                },
                {1},
                {2}
            }
        };
        if (instance->getRenderAPI() == lv::RenderAPI::Metal)
            deferredGraphicsPipelineCI.colorBlendAttachments.push_back({4});
        deferredGraphicsPipeline = device->createGraphicsPipeline(deferredGraphicsPipelineCI);

        //HDR
        fragHdrShaderModule = device->createShaderModule({
            .shaderStage = lv::ShaderStageFlags::Fragment,
            .source = lv::readFile((assetDir + "/shaders/compiled/hdr.frag.json").c_str())
        });

        hdrGraphicsPipeline = device->createGraphicsPipeline({
            .vertexShaderModule = vertTriangleShaderModule,
            .fragmentShaderModule = fragHdrShaderModule,
            .pipelineLayout = hdrPipelineLayout,
            .renderPass = swapChain->getRenderPass(),
            .colorBlendAttachments = {
                {0}
            }
        });

        //Compute pipelines

        //Terrain tessellation control
        tescTerrainShaderModule = device->createShaderModule({
            .shaderStage = lv::ShaderStageFlags::Compute,
            .source = lv::readFile((assetDir + "/shaders/compiled/terrain.tesc.json").c_str())
        });

        terrainTescComputePipeline = device->createComputePipeline({
            .computeShaderModule = tescTerrainShaderModule,
            .pipelineLayout = tescPipelineLayout,
            .threadGroupSizeIsMultipleOfThreadExecutionWidth = lv::False
        });

        //Copy commands
        lv::CommandBuffer* copyCommandBuffer = device->createCommandBuffer({});
        copyCommandBuffer->beginRecording();
        copyCommandBuffer->beginBlitCommands();

        //Buffers
        shadowUniformBuffer = device->createBuffer({
            .usage = lv::BufferUsageFlags::UniformBuffer,
            .memoryType = lv::MemoryType::Shared,
            .size = sizeof(glm::mat4) * SHADOW_CASCADE_COUNT
        });

        tescUniformBuffer = device->createBuffer({
            .size = sizeof(glm::vec2),
            .usage = lv::BufferUsageFlags::UniformBuffer,
            .memoryType = lv::MemoryType::Shared
        });

        mainUniformBuffer = device->createBuffer({
            .size = sizeof(glm::mat4),
            .usage = lv::BufferUsageFlags::UniformBuffer,
            .memoryType = lv::MemoryType::Shared
        });

        controlPointsBuffer = device->createBuffer({
            .frameCount = 1,
            .size = PATCHES_PER_CHUNK * PATCHES_PER_CHUNK * 4 * sizeof(glm::vec4),
            .usage = lv::BufferUsageFlags::UniformBuffer
        });

        glm::vec4 controlPoints[PATCHES_PER_CHUNK][PATCHES_PER_CHUNK][4];
        for (uint8_t y = 0; y < PATCHES_PER_CHUNK; y++) {
            for (uint8_t x = 0; x < PATCHES_PER_CHUNK; x++) {
                controlPoints[y][x][0] = glm::vec4(x       , 0.0f, y       , 1.0f);
                controlPoints[y][x][1] = glm::vec4(x + 1.0f, 0.0f, y       , 1.0f);
                controlPoints[y][x][2] = glm::vec4(x + 1.0f, 0.0f, y + 1.0f, 1.0f);
                controlPoints[y][x][3] = glm::vec4(x       , 0.0f, y + 1.0f, 1.0f);
            }
        }

        copyCommandBuffer->cmdStagingCopyDataToBuffer(controlPointsBuffer, controlPoints);

        //Terrain
        terrain = new Terrain(device, copyCommandBuffer, tescPipelineLayout, shadowPipelineLayout, shadowWithAlphaPipelineLayout, terrainPipelineLayout, gbufferPipelineLayout, treePipelineLayout, grassPipelineLayout, mainUniformBuffer, basicSampler, linearSampler);

        //Descriptor set
        shadowDescriptorSet = device->createDescriptorSet({
            .pipelineLayout = shadowPipelineLayout,
            .bufferBindings = {
                shadowUniformBuffer->descriptorInfo(0)
            }
        });

        shadowWithAlphaDescriptorSet = device->createDescriptorSet({
            .pipelineLayout = shadowWithAlphaPipelineLayout,
            .bufferBindings = {
                shadowUniformBuffer->descriptorInfo(0)
            }
        });

        tescDescriptorSet = device->createDescriptorSet({
            .pipelineLayout = tescPipelineLayout,
            .bufferBindings = {
                tescUniformBuffer->descriptorInfo(0)
            }
        });

        terrainDescriptorSet = device->createDescriptorSet({
            .pipelineLayout = terrainPipelineLayout,
            .bufferBindings = {
                mainUniformBuffer->descriptorInfo(0)
            },
            .imageBindings = {
                terrain->getTerrainGrassTexture()->sampler->descriptorInfo(terrain->getTerrainGrassTexture()->image, 1)
            }
        });

        gbufferDescriptorSet = device->createDescriptorSet({
            .pipelineLayout = gbufferPipelineLayout,
            .bufferBindings = {
                mainUniformBuffer->descriptorInfo(0)
            }
        });

        grassDescriptorSet = device->createDescriptorSet({
            .pipelineLayout = grassPipelineLayout,
            .bufferBindings = {
                mainUniformBuffer->descriptorInfo(0)
            },
            .imageBindings = {
                terrain->getTerrainGrassAlphaCardTexture()->sampler->descriptorInfo(terrain->getTerrainGrassAlphaCardTexture()->image, 1)
            }
        });

        lv::DescriptorSetCreateInfo deferredDescriptorSetCI{
            .pipelineLayout = deferredPipelineLayout,
            .imageBindings = {
                mainRenderPass.diffuseImage->descriptorInfo(0, lv::DescriptorType::InputAttachment),
                mainRenderPass.normalImage->descriptorInfo(1, lv::DescriptorType::InputAttachment),
                shadowRenderPass.depthSampler->descriptorInfo(shadowRenderPass.depthImage, 3)
            }
        };
        if (instance->getRenderAPI() == lv::RenderAPI::Vulkan)
            deferredDescriptorSetCI.imageBindings.push_back(mainRenderPass.depthImage->descriptorInfo(2, lv::DescriptorType::InputAttachment, lv::ImageLayout::DepthStencilReadOnlyOptimal));
        else if (instance->getRenderAPI() == lv::RenderAPI::Metal)
            deferredDescriptorSetCI.imageBindings.push_back(mainRenderPass.depthAsColorImage->descriptorInfo(2, lv::DescriptorType::InputAttachment));
        deferredDescriptorSet = device->createDescriptorSet(deferredDescriptorSetCI);

        hdrDescriptorSet = device->createDescriptorSet({
            .pipelineLayout = hdrPipelineLayout,
            .imageBindings = {
                mainRenderPass.colorImage->descriptorInfo(0)
            }
        });

        //Camera
        uint16_t width, height;
        lvndGetWindowSize(window, &width, &height);
        camera.aspectRatio = (float)width / (float)height;
        camera.farPlane = VIEW_DISTANCE;

        //Commit copy commands
        copyCommandBuffer->endRecording();
        copyCommandBuffer->submit();
        delete copyCommandBuffer;
    }

    ~LavaCoreExampleApp() {
        delete terrain;

        delete gbufferDescriptorSet;
        delete deferredDescriptorSet;

        delete mainUniformBuffer;

        delete gbufferGraphicsPipeline;

        delete deferredGraphicsPipeline;

        delete gbufferPipelineLayout;
        delete deferredPipelineLayout;

        delete swapChain;
        delete device;
        delete instance;
    }

    void update() override {
        float crntTime = lvndGetTime();
        float dt = crntTime - prevTime;
        prevTime = crntTime;

        swapChain->acquireNextImage();

        //Rendering
        camera.inputs(window, dt);

        //Gravity
        yMomentum -= GRAVITY * dt;
        camera.setMovement(glm::vec3(camera.getMovement().x, yMomentum * dt, camera.getMovement().z));

        camera.applyMovement();

        glm::vec2 cameraPosVec2 = glm::vec2(camera.position.x, camera.position.z);
        terrain->updateVisibleChunks(device, camera, cameraPosVec2);

        isOnGround = false;
        float terrainHeight = terrain->getHeightAtPosition(cameraPosVec2);
        if (camera.position.y <= terrainHeight + HEAD_LEVEL) {
            camera.position.y = terrainHeight + HEAD_LEVEL;
            yMomentum = 0.0f;
            isOnGround = true;
        }

        if (isOnGround && lvndGetKeyState(window, LVND_KEY_SPACE) == LVND_STATE_PRESSED) {
            yMomentum = JUMP_HEIGHT;
            isOnGround = false;
        }

        camera.loadViewProj();
        //camera.view = glm::lookAt(glm::vec3(0.0f, 256.0f, 0.0f), glm::vec3(0.0f, 0.0f, 1.0f), glm::vec3(0.0f, 1.0f, 0.0f));
        //camera.viewProj = camera.projection * camera.view;
        //camera.invViewProj = glm::inverse(camera.viewProj);

        getLightMatrices(glm::vec3(2.0, -4.0, 1.0)); //TODO: make this a variable

        //Writing tree models to buffer
        terrain->writeTreeModelsToBuffer(camera.position);

        //Tessellation control pass
        tescCommandBuffer->beginRecording();
        tescCommandBuffer->beginComputeCommands();

        tescCommandBuffer->cmdBindComputePipeline(terrainTescComputePipeline);

        tescUniformBuffer->copyDataTo(&cameraPosVec2);

        //glm::vec3 cp = camera.position;
        //cp.x = (lvndGetKeyState(window, LVND_KEY_SPACE) == LVND_STATE_PRESSED ? 16.0f : 1.0f);
        //tescCommandBuffer->cmdPushConstants(&camera.position, 0);
        tescCommandBuffer->cmdBindDescriptorSet(tescDescriptorSet);

        terrain->dispatchTessellationControl(tescCommandBuffer, camera.position);

        tescCommandBuffer->endRecording();
        tescCommandBuffer->submit();

        //Shadow pass
        shadowRenderPass.commandBuffer->beginRecording();
        shadowRenderPass.commandBuffer->beginRenderCommands(shadowRenderPass.framebuffer);

        //Regular
        shadowRenderPass.commandBuffer->cmdBindGraphicsPipeline(shadowGraphicsPipeline);

        shadowUniformBuffer->copyDataTo(deferredVP.shadowViewProjs);

        shadowRenderPass.commandBuffer->cmdBindDescriptorSet(shadowDescriptorSet);

        terrain->renderTreesShadows(shadowRenderPass.commandBuffer, camera.position);

        //With alpha
        shadowRenderPass.commandBuffer->cmdBindGraphicsPipeline(shadowWithAlphaGraphicsPipeline);

        shadowRenderPass.commandBuffer->cmdBindDescriptorSet(shadowWithAlphaDescriptorSet);

        terrain->renderTreesShadowsWithTextures(shadowRenderPass.commandBuffer, camera.position);
        
        shadowRenderPass.commandBuffer->endRecording();
        shadowRenderPass.commandBuffer->submit();

        //Main pass

        //GBuffer subpass

        //Terrain
        mainRenderPass.commandBuffer->beginRecording();
        mainRenderPass.commandBuffer->beginRenderCommands(mainRenderPass.framebuffer);

        mainRenderPass.commandBuffer->cmdBindGraphicsPipeline(terrainGraphicsPipeline);

        //mainRenderPass.commandBuffer->cmdEnableWireframeMode();

        mainUniformBuffer->copyDataTo(&camera.viewProj);

        mainRenderPass.commandBuffer->cmdBindDescriptorSet(terrainDescriptorSet);

        mainRenderPass.commandBuffer->cmdBindVertexBuffer(controlPointsBuffer);

        terrain->render(mainRenderPass.commandBuffer);

        //GBuffer

        //Nothing yet

        //Tree
        mainRenderPass.commandBuffer->cmdBindGraphicsPipeline(treeGraphicsPipeline);

        terrain->renderTrees(mainRenderPass.commandBuffer);

        //Grass
        mainRenderPass.commandBuffer->cmdBindGraphicsPipeline(grassGraphicsPipeline);

        mainRenderPass.commandBuffer->cmdBindDescriptorSet(grassDescriptorSet);

        terrain->renderGrass(mainRenderPass.commandBuffer, camera.position);

        //Deferred subpass
        mainRenderPass.commandBuffer->cmdNextSubpass();

        mainRenderPass.commandBuffer->cmdBindGraphicsPipeline(deferredGraphicsPipeline);

        deferredVP.invViewProj = glm::inverse(camera.viewProj);
        deferredVP.viewPos = camera.position;

        mainRenderPass.commandBuffer->cmdPushConstants(&deferredVP, 0);

        mainRenderPass.commandBuffer->cmdBindDescriptorSet(deferredDescriptorSet);

        mainRenderPass.commandBuffer->cmdDraw(3);

        mainRenderPass.commandBuffer->endRecording();
        mainRenderPass.commandBuffer->submit();

        //HDR pass
        swapChain->getCommandBuffer()->beginRecording();
        swapChain->getCommandBuffer()->beginRenderCommands(swapChain->getFramebuffer());

        swapChain->getCommandBuffer()->cmdBindGraphicsPipeline(hdrGraphicsPipeline);

        swapChain->getCommandBuffer()->cmdBindDescriptorSet(hdrDescriptorSet);

        swapChain->getCommandBuffer()->cmdDraw(3);

        swapChain->getCommandBuffer()->endRecording();
        swapChain->renderAndPresent();
    }

    static std::vector<glm::vec4> getFrustumCorners(const glm::mat4& proj, const glm::mat4& view) {
        std::vector<glm::vec4> frustumCorners;
        const auto inv = glm::inverse(proj * view);

        for (unsigned int x = 0; x < 2; ++x) {
            for (unsigned int y = 0; y < 2; ++y) {
                for (unsigned int z = 0; z < 2; ++z) {
                    const glm::vec4 pt =
                    inv * glm::vec4(
                        (2.0f * x - 1.0f),
                        (2.0f * y - 1.0f),
                        (2.0f * z - 1.0f),
                        1.0f);
                    frustumCorners.push_back(pt / pt.w);
                }
            }
        }

        return frustumCorners;
    }

    void getLightMatrix(glm::vec3 lightDir, float nearPlane, float farPlane, uint8_t index) {
        glm::mat4 lightProj = glm::perspective(glm::radians(camera.fov), camera.aspectRatio, nearPlane, farPlane);

        std::vector<glm::vec4> frustumCorners = getFrustumCorners(lightProj, camera.view);

        //Get frustum center
        glm::vec3 center = glm::vec3(0.0f, 0.0f, 0.0f);
        for (auto &corner : frustumCorners) {
            center += glm::vec3(corner);
        }
        center /= frustumCorners.size();

        glm::mat4 view = glm::lookAt(center, center + lightDir, glm::vec3(0.0f, 1.0f, 0.0f));

        //Get min/max corner coordinates
        float maxX = std::numeric_limits<float>::min();
        float maxY = std::numeric_limits<float>::min();
        float maxZ = std::numeric_limits<float>::min();
        float minX = std::numeric_limits<float>::max();
        float minY = std::numeric_limits<float>::max();
        float minZ = std::numeric_limits<float>::max();

        for (auto &corner : frustumCorners) {
            auto trf = view * corner;

            minX = std::min(minX, trf.x);
            maxX = std::max(maxX, trf.x);
            minY = std::min(minY, trf.y);
            maxY = std::max(maxY, trf.y);
            minZ = std::min(minZ, trf.z);
            maxZ = std::max(maxZ, trf.z);
        }

        float zAdd = camera.farPlane * 0.8f;
        minZ -= zAdd;
        maxZ += zAdd;

        glm::mat4 projection = glm::ortho(minX, maxX, minY, maxY, minZ, maxZ);

        deferredVP.shadowViewProjs[index] = projection * view;
    }

    void getLightMatrices(glm::vec3 lightDir) {
        for (int i = 0; i < SHADOW_CASCADE_COUNT; i++) {
            if (i == 0) {
                getLightMatrix(lightDir, camera.nearPlane, cascadeLevels[0], 0);
            } else {
                getLightMatrix(lightDir, cascadeLevels[i - 1], cascadeLevels[i], i);
            }
        }
    }
};

int main(int argc, char* argv[]) {
    LavaCoreExampleApp application(argc, argv);
    application.run();

    return 0;
}
