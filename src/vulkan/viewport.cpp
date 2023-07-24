#include "vulkan/lvcore/viewport.hpp"

#include "vulkan/lvcore/swap_chain.hpp"

namespace lv {

namespace vulkan {

Viewport::Viewport(int32_t x, int32_t y, uint32_t width, uint32_t height) {
	setViewport(x, y, width, height);
	viewport.minDepth = 0.0f;
	viewport.maxDepth = 1.0f;
}

void Viewport::setViewport(int32_t x, int32_t y, uint32_t width, uint32_t height) {
	uint32_t trueTotalWidth = (totalWidth == 0 ? width : totalWidth);
	uint32_t trueTotalHeight = (totalHeight == 0 ? height : totalHeight);
	//height = std::min(height, trueTotalHeight - y);
	viewport.x = (float)x;
	viewport.y = (float)trueTotalHeight - (float)y;
	viewport.width = (float)width;
	viewport.height = -(float)height;
	if (y + height > trueTotalHeight) {
		viewport.y = float(trueTotalHeight) - float(height) - y;
		viewport.height = (float)height;
	}

	scissor.offset = VkOffset2D{x, int(trueTotalHeight) - int(height) - y};
	scissor.extent = VkExtent2D{(uint32_t)std::max((int)width + std::min(scissor.offset.x, 0), 0), (uint32_t)std::max((int)height + std::min(scissor.offset.y, 0), 0)};
	/*
	if (y + height > trueTotalHeight) {
		std::cout << "Scissor Y: " << scissor.offset.y << std::endl;
		std::cout << "Scissor height: " << scissor.extent.height << std::endl;
		std::cout << "Param height: " << height << std::endl << std::endl;
	} else {
		std::cout << "Correct Y: " << scissor.offset.y << std::endl << std::endl;
	}
	*/
	scissor.offset.x = std::max(scissor.offset.x, 0);
	scissor.offset.y = std::max(scissor.offset.y, 0);
}

} //namespace vulkan

} //namespace lv
