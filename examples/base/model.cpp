#include "model.hpp"

#include <iostream>

Texture Mesh::neutralTexture = Texture();
Texture Mesh::normalNeutralTexture = Texture();
bool Mesh::neautralTexturesCreated = false;
std::vector<Texture*> Mesh::loadedTextures = std::vector<Texture*>();

void Mesh::init(lv::CommandBuffer* commandBuffer, std::vector<MainVertex>& aVertices, std::vector<uint32_t>& aIndices) {
    vertices = aVertices;
    indices = aIndices;
	
    vertexBuffer = new lv::Buffer({
		.frameCount = 1,
		.usage = lv::BufferUsageFlags::TransferDestination | lv::BufferUsageFlags::VertexBuffer,
		.size = vertices.size() * sizeof(MainVertex)
	});
    commandBuffer->cmdStagingCopyDataToBuffer(vertexBuffer, vertices.data());

	indexBuffer = new lv::Buffer({
		.frameCount = 1,
		.usage = lv::BufferUsageFlags::TransferDestination | lv::BufferUsageFlags::IndexBuffer,
		.size = indices.size() * sizeof(uint32_t)
	});
    commandBuffer->cmdStagingCopyDataToBuffer(indexBuffer, indices.data());
}

void Mesh::destroy() {
    delete vertexBuffer;
    delete indexBuffer;
}

void Mesh::initDescriptorSet(lv::PipelineLayout* pipelineLayout, lv::PipelineLayout* shadowPipelineLayout, uint8_t descriptorSetLayoutIndex, uint8_t shadowDescriptorSetLayoutIndex) {
	lv::DescriptorSetCeateInfo descriptorSetCreateInfo;
	descriptorSetCreateInfo.pipelineLayout = pipelineLayout;
	descriptorSetCreateInfo.layoutIndex = descriptorSetLayoutIndex;
    for (uint8_t i = 0; i < textures.size(); i++) {
        descriptorSetCreateInfo.imageBindings.push_back(textures[i]->sampler->descriptorInfo(textures[i]->image, i));
    }

	if (shadowPipelineLayout) {
		shadowDescriptorSet = new lv::DescriptorSet({
			.pipelineLayout = shadowPipelineLayout,
			.layoutIndex = shadowDescriptorSetLayoutIndex,
			.imageBindings = {
				textures[0]->sampler->descriptorInfo(textures[0]->image, 0)
			}
		});
	}

    descriptorSet = new lv::DescriptorSet(descriptorSetCreateInfo);
}

void Mesh::destroyDescriptorSet() {
    delete descriptorSet;
}

void Mesh::setTexture(Texture* texture, uint8_t index) {
    if (texture)
        textures[index] = texture;
}

void Mesh::render(lv::CommandBuffer* commandBuffer, uint16_t instanceCount) {
	commandBuffer->cmdBindDescriptorSet(descriptorSet);
    renderNoTextures(commandBuffer, instanceCount);
}

void Mesh::renderShadows(lv::CommandBuffer* commandBuffer, uint16_t instanceCount) {
	commandBuffer->cmdBindDescriptorSet(shadowDescriptorSet);
    renderNoTextures(commandBuffer, instanceCount);
}

void Mesh::renderNoTextures(lv::CommandBuffer* commandBuffer, uint16_t instanceCount) {
    commandBuffer->cmdBindVertexBuffer(vertexBuffer);
    commandBuffer->cmdDrawIndexed(indexBuffer, lv::IndexType::Uint32, indexBuffer->size() / sizeof(uint32_t), instanceCount);
}

void Mesh::createPlane(lv::CommandBuffer* commandBuffer) {
    static std::vector<MainVertex> vertices = {
        {{-0.5f, 0.0f, -0.5f}, {0.0f, 0.0f}, {0.0f, 1.0f, 0.0f}, {0.0f, 0.0f, 0.0f, 1.0f}},
        {{ 0.5f, 0.0f, -0.5f}, {1.0f, 0.0f}, {0.0f, 1.0f, 0.0f}, {0.0f, 0.0f, 0.0f, 1.0f}},
        {{ 0.5f, 0.0f,  0.5f}, {1.0f, 1.0f}, {0.0f, 1.0f, 0.0f}, {0.0f, 0.0f, 0.0f, 1.0f}},
        {{-0.5f, 0.0f,  0.5f}, {0.0f, 1.0f}, {0.0f, 1.0f, 0.0f}, {0.0f, 0.0f, 0.0f, 1.0f}}
    };
    static std::vector<uint32_t> indices = {
        0, 2, 1,
        0, 3, 2
    };

    init(commandBuffer, vertices, indices);
}

Texture* Mesh::loadTextureFromFile(lv::CommandBuffer* commandBuffer, const char* filename, bool isSRGB) {
    std::string strFilename(filename);

    //Check if it has not been loaded yet
	for (unsigned int j = 0; j < loadedTextures.size(); j++) {
		if (loadedTextures[j]->filename == strFilename) {
			return loadedTextures[j];
		}
	}

	Texture* texture = new Texture;
    texture->init(commandBuffer, filename, isSRGB, true);

	loadedTextures.push_back(texture);

	return texture;
}

//Model
void Model::init(lv::CommandBuffer* commandBuffer, lv::PipelineLayout* aPipelineLayout, lv::PipelineLayout* aShadowPipelineLayout, const char* filename, uint8_t aTextureCount, uint8_t aDescriptorSetLayoutIndex, uint8_t aShadowDescriptorSetLayoutIndex, bool flipUVs) {
	pipelineLayout = aPipelineLayout;
	shadowPipelineLayout = aShadowPipelineLayout;
	textureCount = aTextureCount;
	descriptorSetLayoutIndex = aDescriptorSetLayoutIndex;
	shadowDescriptorSetLayoutIndex = aShadowDescriptorSetLayoutIndex;

	//Create neautral textures if needed
	if (!Mesh::neautralTexturesCreated) {
		Texture* neautralTextures[2] = {&Mesh::neutralTexture, &Mesh::normalNeutralTexture};
		glm::u8vec4 neautralColors[2] = {{255, 255, 255, 255}, {128, 128, 255, 0}};
		lv::Format neutralTextureFormats[2] = {lv::Format::RGBA8Unorm_sRGB, lv::Format::RGBA8Unorm};
		for (uint8_t i = 0; i < 2; i++) {
			neautralTextures[i]->image = new lv::Image({
				.frameCount = 1,
				.format = neutralTextureFormats[i],
				.width = 1,
				.height = 1,
				.usage = lv::ImageUsageFlags::Sampled | lv::ImageUsageFlags::TransferDestination
			});
			commandBuffer->cmdStagingCopyDataToImage(neautralTextures[i]->image, &neautralColors[i]);
			commandBuffer->cmdTransitionImageLayout(neautralTextures[i]->image, 0, lv::ImageLayout::TransferDestinationOptimal, lv::ImageLayout::ShaderReadOnlyOptimal);
			neautralTextures[i]->sampler = new lv::Sampler({
				.filter = lv::Filter::Linear,
				.addressMode = lv::SamplerAddressMode::Repeat
			});
		}
	}

	std::cout << "Loading model '" << filename << "'" << std::endl;
	Assimp::Importer import;

	unsigned int flags = aiProcess_Triangulate | aiProcess_CalcTangentSpace | aiProcess_GenSmoothNormals | (flipUVs ? aiProcess_FlipUVs : 0);
	std::string strFilename(filename);
	const aiScene *scene = import.ReadFile(strFilename, flags);

	if(!scene || scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !scene->mRootNode)  {
		std::cout << "Error: Could not load model '" << filename << "' ->\n" << import.GetErrorString() << std::endl;
		return;
	}
	directory = strFilename.substr(0, strFilename.find_last_of("/"));

	if (strFilename.substr(strFilename.find_last_of(".")) == ".fbx") {
		rotation.x = -90.0f;
	}

	processNode(commandBuffer, scene->mRootNode, scene);
}

void Model::destroy() {
    for (auto& mesh : meshes)
        mesh.destroy();
}

void Model::calculateModel() {
	model.prevModel = model.model;
	model.model = glm::translate(glm::mat4(1.0f), position) *
                  glm::toMat4(glm::quat(glm::radians(rotation))) *
                  glm::scale(glm::mat4(1.0f), scale);
    model.normalMatrix = glm::mat4(glm::transpose(glm::inverse(glm::mat3(model.model))));
}

void Model::render(lv::CommandBuffer* commandBuffer, uint16_t instanceCount, bool uploadModel) {
	if (uploadModel)
		commandBuffer->cmdPushConstants(&model, 0);

    for (auto& mesh : meshes) {
        mesh.render(commandBuffer, instanceCount);
    }
}

void Model::renderShadows(lv::CommandBuffer* commandBuffer, uint16_t instanceCount, bool uploadModel) {
	if (uploadModel)
		commandBuffer->cmdPushConstants(&model, 0);

    for (auto& mesh : meshes) {
        mesh.renderShadows(commandBuffer, instanceCount);
    }
}

void Model::renderNoTextures(lv::CommandBuffer* commandBuffer, uint16_t instanceCount, bool uploadModel) {
	if (uploadModel)
		commandBuffer->cmdPushConstants(&model, 0);

    for (auto& mesh : meshes) {
        mesh.renderNoTextures(commandBuffer, instanceCount);
    }
}

void Model::processNode(lv::CommandBuffer* commandBuffer, aiNode* node, const aiScene* scene) {
	// process all the node's meshes (if any)
	for (unsigned int i = 0; i < node->mNumMeshes; i++) {
		aiMesh *mesh = scene->mMeshes[node->mMeshes[i]];
		processMesh(commandBuffer, node, mesh, scene);
	}
	// then do the same for each of its children
	for (unsigned int i = 0; i < node->mNumChildren; i++) {
		processNode(commandBuffer, node->mChildren[i], scene);
	}
}

void Model::processMesh(lv::CommandBuffer* commandBuffer, aiNode* node, aiMesh* mesh, const aiScene* scene) {
	std::vector<MainVertex> vertices;
	std::vector<unsigned int> indices;

	float x = node->mTransformation.a4;
	float y = node->mTransformation.b4;
	float z = node->mTransformation.c4;

	for (unsigned int i = 0; i < mesh->mNumVertices; i++) {
		MainVertex vertex;
		// process vertex positions, normals and texture coordinates
		vertex.position.x = x + mesh->mVertices[i].x;
		vertex.position.y = y + mesh->mVertices[i].y;
		vertex.position.z = z + mesh->mVertices[i].z;

		vertex.normal.x = mesh->mNormals[i].x;
		vertex.normal.y = mesh->mNormals[i].y;
		vertex.normal.z = mesh->mNormals[i].z;

		if (mesh->mTextureCoords[0]) {
			vertex.texCoord.x = mesh->mTextureCoords[0][i].x;
			vertex.texCoord.y = mesh->mTextureCoords[0][i].y;

			vertex.tangent.x = mesh->mTangents[i].x;
			vertex.tangent.y = mesh->mTangents[i].y;
			vertex.tangent.z = mesh->mTangents[i].z;

			glm::vec3 bitangent1 = glm::cross(vertex.normal, glm::vec3(vertex.tangent));
			glm::vec3 bitangent2 = glm::vec3(mesh->mBitangents[i].x, mesh->mBitangents[i].y, mesh->mBitangents[i].z);
			vertex.tangent.w = glm::dot(bitangent1, bitangent2) < 0.0f ? -1.0f : 1.0f;
		} else {
			vertex.texCoord = glm::vec2(0.0f);
		}

		vertices.push_back(vertex);
	}

	// process indices
	for (unsigned int i = 0; i < mesh->mNumFaces; i++) {
		aiFace face = mesh->mFaces[i];
		for (unsigned int j = 0; j < face.mNumIndices; j++) {
			indices.push_back(face.mIndices[j]);
		}
	}

	// process material
	if (mesh->mMaterialIndex >= 0) {
		aiMaterial* material = scene->mMaterials[mesh->mMaterialIndex];

		Mesh& newMesh = meshes.emplace_back(textureCount);
		newMesh.init(commandBuffer, vertices, indices);

        if (textureCount >= 1) {
            Texture* texAlbedo = loadMaterialTextures(commandBuffer, material, aiTextureType_DIFFUSE);
            newMesh.setTexture(texAlbedo, 0);
        }
        if (textureCount >= 2) {
            Texture* texMetallicRoughness = loadMaterialTextures(commandBuffer, material, aiTextureType_DIFFUSE_ROUGHNESS);
            if (!texMetallicRoughness)
                texMetallicRoughness = loadMaterialTextures(commandBuffer, material, aiTextureType_METALNESS);
            newMesh.setTexture(texMetallicRoughness, 1);
        }
        if (textureCount >= 3) {
            Texture* texNormal = loadMaterialTextures(commandBuffer, material, aiTextureType_HEIGHT);
            if (!texNormal)
                texNormal = loadMaterialTextures(commandBuffer, material, aiTextureType_NORMALS);
            newMesh.setTexture(texNormal, 2);
        }

		newMesh.initDescriptorSet(pipelineLayout, shadowPipelineLayout, descriptorSetLayoutIndex, shadowDescriptorSetLayoutIndex);
	}
}

Texture* Model::loadMaterialTextures(lv::CommandBuffer* commandBuffer, aiMaterial *mat, aiTextureType type) {
	aiString str;
	int count = mat->GetTextureCount(type);

	mat->GetTexture(type, 0, &str);
	std::string texStr = std::string(str.C_Str());
	std::string filename = directory + "/" + texStr;
	if (texStr == "")
		return nullptr;

	return Mesh::loadTextureFromFile(commandBuffer, filename.c_str(), type == aiTextureType_DIFFUSE);
}
