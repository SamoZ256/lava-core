#include "vulkan/lvcore/core/pipeline_layout.hpp"

#include "vulkan/lvcore/core/common.hpp"

#include "vulkan/lvcore/core/device.hpp"

namespace lv {

// *************** Descriptor Set Layout *********************

Vulkan_DescriptorSetLayout::Vulkan_DescriptorSetLayout(std::vector<Vulkan_DescriptorSetLayoutBinding> aBindings) {
	bindings.resize(aBindings.size());
	for (uint32_t i = 0; i < bindings.size(); i++) {
		VkDescriptorType vkDescriptorType;
		GET_VK_DESCRIPTOR_TYPE(aBindings[i].descriptorType, vkDescriptorType);

		bindings[i].binding = aBindings[i].binding;
		bindings[i].descriptorType = vkDescriptorType;
		bindings[i].descriptorCount = 1;
		bindings[i].stageFlags = vulkan::getVKShaderStageFlags(aBindings[i].shaderStage);
	}

	VkDescriptorSetLayoutCreateInfo descriptorSetLayoutInfo{};
	descriptorSetLayoutInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;
	descriptorSetLayoutInfo.bindingCount = bindings.size();
	descriptorSetLayoutInfo.pBindings = bindings.data();

	VK_CHECK_RESULT(vkCreateDescriptorSetLayout(g_vulkan_device->device(), &descriptorSetLayoutInfo, nullptr, &descriptorSetLayout));
}

void Vulkan_DescriptorSetLayout::destroy() {
	vkDestroyDescriptorSetLayout(g_vulkan_device->device(), descriptorSetLayout, nullptr);
	//descriptorSetLayout = VK_NULL_HANDLE;
}

// *************** Pipeline Layout *********************

Vulkan_PipelineLayout::Vulkan_PipelineLayout(Vulkan_PipelineLayoutCreateInfo createInfo) : descriptorSetLayouts(createInfo.descriptorSetLayouts), pushConstantRanges(createInfo.pushConstantRanges.size()) {
	for (uint32_t i = 0; i < pushConstantRanges.size(); i++) {
		pushConstantRanges[i].stageFlags = vulkan::getVKShaderStageFlags(createInfo.pushConstantRanges[i].stageFlags);
		pushConstantRanges[i].offset = createInfo.pushConstantRanges[i].offset;
		pushConstantRanges[i].size = createInfo.pushConstantRanges[i].size;
	}

	std::vector<VkDescriptorSetLayout> descLayouts;
	for (uint8_t i = 0; i < descriptorSetLayouts.size(); i++) {
		descLayouts.push_back(descriptorSetLayouts[i].descriptorSetLayout);
	}

	VkPipelineLayoutCreateInfo pipelineLayoutInfo{};
	pipelineLayoutInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
	pipelineLayoutInfo.setLayoutCount = descLayouts.size();
	pipelineLayoutInfo.pSetLayouts = descLayouts.data();
	pipelineLayoutInfo.pushConstantRangeCount = pushConstantRanges.size();
	pipelineLayoutInfo.pPushConstantRanges = pushConstantRanges.data();

	VK_CHECK_RESULT(vkCreatePipelineLayout(g_vulkan_device->device(), &pipelineLayoutInfo, nullptr, &_pipelineLayout));
}

Vulkan_PipelineLayout::~Vulkan_PipelineLayout() {
	for (auto& descriptorSetLayout : descriptorSetLayouts)
		descriptorSetLayout.destroy();
	vkDestroyPipelineLayout(g_vulkan_device->device(), _pipelineLayout, nullptr);
}

} //namespace lv
