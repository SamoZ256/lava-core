#ifndef LV_VULKAN_BUFFER_HELPER_H
#define LV_VULKAN_BUFFER_HELPER_H

#include <vulkan/vulkan.h>
#include <vk_mem_alloc.h>

namespace lv {

class Vulkan_BufferHelper {
public:
  static VmaAllocation createBuffer(VkDeviceSize size, VkBufferUsageFlags usage/*, VmaMemoryUsage memoryUsage*/, VkBuffer &buffer, VmaAllocationInfo* allocInfo = nullptr, VkMemoryPropertyFlags properties = 0, VmaAllocationCreateFlags allocationFlags = 0);

  static void copyBuffer(VkCommandBuffer commandBuffer, VkBuffer srcBuffer, VkBuffer dstBuffer, VkDeviceSize size);

  static void copyBufferToImage(VkCommandBuffer commandBuffer, VkBuffer buffer, VkImage image, uint32_t width, uint32_t height, uint32_t layerCount = 1, uint8_t arrayLayer = 0, VkImageAspectFlagBits aspectMask = VK_IMAGE_ASPECT_COLOR_BIT);

  static void copyImage(VkCommandBuffer commandBuffer, VkImage srcImage, VkImage dstImage, uint32_t width, uint32_t height, uint32_t srcLayerCount = 1, uint8_t srcArrayLayer = 0, uint32_t dstLayerCount = 1, uint8_t dstArrayLayer = 0, VkImageAspectFlagBits aspectMask = VK_IMAGE_ASPECT_COLOR_BIT);
};

} //namespace lv

#endif
