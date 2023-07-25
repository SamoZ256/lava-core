#include "lvcore/vulkan/pipeline_layout.hpp"

#include "lvcore/vulkan/common.hpp"

#include "lvcore/vulkan/device.hpp"

namespace lv {

namespace vulkan {

//---------------- Descriptor Set Layout ----------------

DescriptorSetLayout::DescriptorSetLayout(std::vector<internal::DescriptorSetLayoutBinding> aBindings) {
	bindings.resize(aBindings.size());
	for (uint32_t i = 0; i < bindings.size(); i++) {
		VkDescriptorType vkDescriptorType;
		GET_VK_DESCRIPTOR_TYPE(aBindings[i].descriptorType, vkDescriptorType);

		bindings[i].binding = aBindings[i].binding;
		bindings[i].descriptorType = vkDescriptorType;
		bindings[i].descriptorCount = 1;
		bindings[i].stageFlags = getVKShaderStageFlags(aBindings[i].shaderStage);
	}

	VkDescriptorSetLayoutCreateInfo descriptorSetLayoutInfo{};
	descriptorSetLayoutInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;
	descriptorSetLayoutInfo.bindingCount = bindings.size();
	descriptorSetLayoutInfo.pBindings = bindings.data();

	VK_CHECK_RESULT(vkCreateDescriptorSetLayout(g_vulkan_device->device(), &descriptorSetLayoutInfo, nullptr, &descriptorSetLayout));
}

void DescriptorSetLayout::destroy() {
	vkDestroyDescriptorSetLayout(g_vulkan_device->device(), descriptorSetLayout, nullptr);
	//descriptorSetLayout = VK_NULL_HANDLE;
}

//---------------- Pipeline Layout ----------------

PipelineLayout::PipelineLayout(internal::PipelineLayoutCreateInfo createInfo) : pushConstantRanges(createInfo.pushConstantRanges.size()) {
	descriptorSetLayouts.reserve(createInfo.descriptorSetLayouts.size());
	for (uint32_t i = 0; i < createInfo.descriptorSetLayouts.size(); i++)
		descriptorSetLayouts.emplace_back(createInfo.descriptorSetLayouts[i]);

	for (uint32_t i = 0; i < pushConstantRanges.size(); i++) {
		pushConstantRanges[i].stageFlags = getVKShaderStageFlags(createInfo.pushConstantRanges[i].stageFlags);
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

PipelineLayout::~PipelineLayout() {
	for (auto& descriptorSetLayout : descriptorSetLayouts)
		descriptorSetLayout.destroy();
	vkDestroyPipelineLayout(g_vulkan_device->device(), _pipelineLayout, nullptr);
}

} //namespace vulkan

} //namespace lv
