#include "lvcore/vulkan/descriptor_set.hpp"

#include "lvcore/vulkan/common.hpp"

#include "lvcore/vulkan/device.hpp"
#include "lvcore/vulkan/swap_chain.hpp"
//#include "Core/Renderer.hpp"

namespace lv {

namespace vulkan {

// *************** Descriptor Writer *********************

void DescriptorWriter::writeBuffer(uint32_t binding, VkDescriptorBufferInfo *bufferInfo) {
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

void DescriptorWriter::writeImage(uint32_t binding, VkDescriptorImageInfo *imageInfo) {
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

void DescriptorWriter::build(VkDescriptorSet& set) {
	g_vulkan_descriptorPool->allocateDescriptorSet(pipelineLayout->descriptorSetLayout(layoutIndex).descriptorSetLayout, set);
	overwrite(set);
}

void DescriptorWriter::overwrite(VkDescriptorSet &set) {
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

DescriptorSet::DescriptorSet(internal::DescriptorSetCreateInfo createInfo) : _pipelineLayout(static_cast<PipelineLayout*>(createInfo.pipelineLayout)) {
	_frameCount = (createInfo.frameCount == 0 ? g_vulkan_swapChain->maxFramesInFlight() : createInfo.frameCount);

	_layoutIndex = createInfo.layoutIndex;

	while (true) {
		bool allocated = true;
		for (uint8_t i = 0; i < _frameCount; i++) {
			for (auto* bufferBinding : createInfo.bufferBindings) {
				CAST_FROM_INTERNAL(bufferBinding, BufferDescriptorInfo);
				if (!registerDescriptor(bufferBinding_->descriptorType))
					allocated = false;
			}
			for (auto* imageBinding : createInfo.imageBindings) {
				CAST_FROM_INTERNAL(imageBinding, ImageDescriptorInfo);
				if (!registerDescriptor(imageBinding_->descriptorType))
					allocated = false;
			}
		}
		if (allocated)
			break;
	}
	
	descriptorSets.resize(_frameCount);
	for (uint8_t i = 0; i < _frameCount; i++) {
		DescriptorWriter writer(_pipelineLayout, _layoutIndex);
		for (auto* bufferBinding : createInfo.bufferBindings) {
			CAST_FROM_INTERNAL(bufferBinding, BufferDescriptorInfo);
			uint8_t frameIndex = i < bufferBinding_->infos.size() ? i : 0;
			writer.writeBuffer(bufferBinding_->binding, &bufferBinding_->infos[frameIndex]);
			//free(bufferBinding_);
		}
		for (auto* imageBinding : createInfo.imageBindings) {
			CAST_FROM_INTERNAL(imageBinding, ImageDescriptorInfo);
			uint8_t frameIndex = i < imageBinding_->infos.size() ? i : 0;
			writer.writeImage(imageBinding_->binding, &imageBinding_->infos[frameIndex]);
			//free(imageBinding_);
		}
		writer.build(descriptorSets[i]);
	}

	pool = &g_vulkan_descriptorPool->descriptorPool;
}

DescriptorSet::~DescriptorSet() {
	vkFreeDescriptorSets(g_vulkan_device->device(), *pool, descriptorSets.size(), descriptorSets.data());
}

bool DescriptorSet::registerDescriptor(DescriptorType descriptorType) {
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

} //namespace vulkan

} //namespace lv
