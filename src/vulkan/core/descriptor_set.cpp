#include "vulkan/lvcore/core/descriptor_set.hpp"

#include "vulkan/lvcore/core/common.hpp"

#include "vulkan/lvcore/core/device.hpp"
#include "vulkan/lvcore/core/swap_chain.hpp"
//#include "Core/Renderer.hpp"

namespace lv {

// *************** Descriptor Writer *********************

void Vulkan_DescriptorWriter::writeBuffer(uint32_t binding, VkDescriptorBufferInfo *bufferInfo) {
	//assert(setLayout.bindings.count(binding) == 1 && "Layout does not contain specified binding");

	auto& bindingDescription = pipelineLayout->descriptorSetLayout(layoutIndex).bindings[binding];

	assert(bindingDescription.descriptorCount == 1 && "Binding single descriptor info, but binding expects multiple");

	VkWriteDescriptorSet write{};
	write.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
	write.descriptorType = bindingDescription.descriptorType;
	write.dstBinding = binding;
	write.pBufferInfo = bufferInfo;
	write.descriptorCount = 1;

	writes.push_back(write);
}

void Vulkan_DescriptorWriter::writeImage(uint32_t binding, VkDescriptorImageInfo *imageInfo) {
	//assert(setLayout.bindings.count(binding) == 1 && "Layout does not contain specified binding");

	auto &bindingDescription = pipelineLayout->descriptorSetLayout(layoutIndex).bindings[binding];

	assert(bindingDescription.descriptorCount == 1 && "Binding single descriptor info, but binding expects multiple");

	VkWriteDescriptorSet write{};
	write.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
	write.descriptorType = bindingDescription.descriptorType;
	write.dstBinding = binding;
	write.pImageInfo = imageInfo;
	write.descriptorCount = 1;

	writes.push_back(write);
}

void Vulkan_DescriptorWriter::build(VkDescriptorSet& set) {
	g_vulkan_descriptorPool->allocateDescriptorSet(pipelineLayout->descriptorSetLayout(layoutIndex).descriptorSetLayout, set);
	overwrite(set);
}

void Vulkan_DescriptorWriter::overwrite(VkDescriptorSet &set) {
	for (auto &write : writes) {
		write.dstSet = set;
	}
	//std::cout << "Writes: " << writes.size() << std::endl;
	vkUpdateDescriptorSets(g_vulkan_device->device(), writes.size(), writes.data(), 0, nullptr);
}

// *************** Descriptor Set *********************

/*
void DescriptorSet::destroy() {
  g_descriptorManager->descriptorPool.freeDescriptorSets(descriptorSets);
}
*/

Vulkan_DescriptorSet::Vulkan_DescriptorSet(Vulkan_DescriptorSetCreateInfo createInfo) : _pipelineLayout(createInfo.pipelineLayout), _layoutIndex(createInfo.layoutIndex) {
	_frameCount = (createInfo.frameCount == 0 ? g_vulkan_swapChain->maxFramesInFlight() : createInfo.frameCount);

	while (true) {
		bool allocated = true;
		for (uint8_t i = 0; i < _frameCount; i++) {
			for (auto& imageBinding : createInfo.imageBindings) {
				if (!registerDescriptor(imageBinding.descriptorType))
					allocated = false;
			}
			for (auto& bufferBinding : createInfo.bufferBindings) {
				if (!registerDescriptor(bufferBinding.descriptorType))
					allocated = false;
			}
		}
		if (allocated)
			break;
	}
	
	descriptorSets.resize(_frameCount);
	for (uint8_t i = 0; i < _frameCount; i++) {
		Vulkan_DescriptorWriter writer(_pipelineLayout, _layoutIndex);
		for (uint8_t buff = 0; buff < createInfo.bufferBindings.size(); buff++) {
			uint8_t frameIndex = i < createInfo.bufferBindings[buff].infos.size() ? i : 0;
			writer.writeBuffer(createInfo.bufferBindings[buff].binding, &createInfo.bufferBindings[buff].infos[frameIndex]);
		}
		for (uint8_t img = 0; img < createInfo.imageBindings.size(); img++) {
			uint8_t frameIndex = i < createInfo.imageBindings[img].infos.size() ? i : 0;
			writer.writeImage(createInfo.imageBindings[img].binding, &createInfo.imageBindings[img].infos[frameIndex]);
		}
		writer.build(descriptorSets[i]);
	}

	pool = &g_vulkan_descriptorPool->descriptorPool;
}

Vulkan_DescriptorSet::~Vulkan_DescriptorSet() {
	vkFreeDescriptorSets(g_vulkan_device->device(), *pool, descriptorSets.size(), descriptorSets.data());
}

bool Vulkan_DescriptorSet::registerDescriptor(VkDescriptorType descriptorType) {
	uint32_t& count = g_vulkan_descriptorPool->poolSizes[descriptorType];
	if (count == 0) {
		g_vulkan_descriptorPool->recreate();
		count = g_vulkan_descriptorPool->poolSizes[descriptorType];
		count -= 1;

		return false;
	}
	count -= 1;
	//std::cout << descriptorType << " : " << count << std::endl;

	return true;
}

} //namespace lv
