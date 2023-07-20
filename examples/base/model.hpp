#ifndef MODEL_H
#define MODEL_H

#include <string>

#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>

#include "glm.hpp"

#include "lvcore/core/buffer.hpp"
#include "lvcore/core/image.hpp"
#include "lvcore/core/sampler.hpp"
#include "lvcore/core/descriptor_set.hpp"
#include "lvcore/core/pipeline_layout.hpp"
#include "lvcore/core/graphics_pipeline.hpp"
#include "lvcore/core/command_buffer.hpp"

struct Texture {
    std::string filename;
    lv::Image* image;
    lv::Sampler* sampler;

    void init(lv::CommandBuffer* commandBuffer, const char* aFilename, bool isSRGB, bool generateMipmaps = false) {
        filename = std::string(aFilename);
        image = new lv::Image({
            .filename = aFilename,
            .isSRGB = isSRGB,
            .generateMipmaps = generateMipmaps
        }, commandBuffer);
        commandBuffer->cmdTransitionImageLayout(image, 0, lv::ImageLayout::TransferDestinationOptimal, lv::ImageLayout::ShaderReadOnlyOptimal);
        sampler = new lv::Sampler({
            .filter = lv::Filter::Linear,
            .addressMode = lv::SamplerAddressMode::Repeat,
            .maxLod = float(image->mipCount())
        });
    }
};

struct MainVertex {
    glm::vec3 position;
    glm::vec2 texCoord;
    glm::vec3 normal;
    glm::vec4 tangent;
};

struct PCModel {
    glm::mat4 model;
    glm::mat4 normalMatrix;
    glm::mat4 prevModel;
};

//TODO: destroy textures once at mesh destroying
class Mesh {
private:
    lv::DescriptorSet* descriptorSet;
    lv::DescriptorSet* shadowDescriptorSet;

public:
    std::vector<MainVertex> vertices;
    std::vector<uint32_t> indices;

    //Rendering
    lv::Buffer* vertexBuffer;
    lv::Buffer* indexBuffer;

    static Texture neutralTexture;
    static Texture normalNeutralTexture;
    static bool neautralTexturesCreated;

    static std::vector<Texture*> loadedTextures;

    std::vector<Texture*> textures;

    Mesh(uint8_t textureCount) {
        textures.resize(textureCount);
        for (uint8_t i = 0; i < textureCount; i++)
            textures[i] = &neutralTexture;
        if (textureCount >= 3)
            textures[2] = &normalNeutralTexture;
    }

    void init(lv::CommandBuffer* commandBuffer, std::vector<MainVertex>& aVertices, std::vector<uint32_t>& aIndices/*, std::vector<Texture*>& aTextures*/);

    void destroy();

    void initDescriptorSet(lv::PipelineLayout* pipelineLayout, lv::PipelineLayout* shadowPipelineLayout, uint8_t descriptorSetLayoutIndex, uint8_t shadowDescriptorSetLayoutIndex);

    void destroyDescriptorSet();

    void setTexture(Texture* texture, uint8_t index);

    void render(lv::CommandBuffer* commandBuffer, uint16_t instanceCount);

    void renderShadows(lv::CommandBuffer* commandBuffer, uint16_t instanceCount);

    void renderNoTextures(lv::CommandBuffer* commandBuffer, uint16_t instanceCount);

	void createPlane(lv::CommandBuffer* commandBuffer);

    //Static functions
    static Texture* loadTextureFromFile(lv::CommandBuffer* commandBuffer, const char* filename, bool isSRGB);
};

class Model {
public:
	std::vector<Mesh> meshes;

	std::string directory;

	PCModel model;
    glm::vec3 position = glm::vec3(0.0f);
    glm::vec3 rotation = glm::vec3(0.0f);
    glm::vec3 scale = glm::vec3(1.0f);

	lv::PipelineLayout* pipelineLayout;
	lv::PipelineLayout* shadowPipelineLayout;

    uint8_t textureCount;

    uint8_t descriptorSetLayoutIndex;
    uint8_t shadowDescriptorSetLayoutIndex;

	void init(lv::CommandBuffer* commandBuffer, lv::PipelineLayout* aPipelineLayout, lv::PipelineLayout* aShadowPipelineLayout, const char* filename, uint8_t aTextureCount, uint8_t aDescriptorSetLayoutIndex, uint8_t aShadowDescriptorSetLayoutIndex, bool flipUVs = true);

    void destroy();

    void calculateModel();

    void render(lv::CommandBuffer* commandBuffer, uint16_t instanceCount = 1, bool uploadModel = true);

    void renderShadows(lv::CommandBuffer* commandBuffer, uint16_t instanceCount = 1, bool uploadModel = true);

    void renderNoTextures(lv::CommandBuffer* commandBuffer, uint16_t instanceCount = 1, bool uploadModel = true);

private:
	void processNode(lv::CommandBuffer* commandBuffer, aiNode* node, const aiScene* scene);

	void processMesh(lv::CommandBuffer* commandBuffer, aiNode* node, aiMesh* mesh, const aiScene* scene);

	Texture* loadMaterialTextures(lv::CommandBuffer* commandBuffer, aiMaterial* mat, aiTextureType type);
};

#endif
