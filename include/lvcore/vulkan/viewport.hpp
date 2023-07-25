#ifndef LV_VULKAN_VIEWPORT_H
#define LV_VULKAN_VIEWPORT_H

#include <vulkan/vulkan.h>

#include "lvcore/internal/viewport.hpp"

namespace lv {

namespace vulkan {

class Viewport : public internal::Viewport {
private:
	VkViewport viewport{};
	VkRect2D scissor{};

public:
	Viewport() {}

	Viewport(int32_t x, int32_t y, uint32_t width, uint32_t height);

	void setViewport(int32_t x, int32_t y, uint32_t width, uint32_t height) override;

	//Getters
	inline VkViewport& getViewport() { return viewport; }

	inline VkRect2D& getScissor() { return scissor; }
};

} //namespace vulkan

} //namespace lv

#endif
