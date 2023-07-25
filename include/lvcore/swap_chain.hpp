#ifndef LV_SWAP_CHAIN_H
#define LV_SWAP_CHAIN_H

#ifdef LV_BACKEND_VULKAN

#include "lvcore/vulkan/swap_chain.hpp"

#define g_swapChain g_vulkan_swapChain

namespace lv {

typedef vulkan::SwapChain SwapChain;

} //namespace lv

#endif

#ifdef LV_BACKEND_METAL

#include "lvcore/metal/swap_chain.hpp"

#define g_swapChain g_metal_swapChain

namespace lv {

typedef metal::SwapChain SwapChain;

} //namespace lv

#endif

namespace lv {

typedef internal::SwapChainCreateInfo SwapChainCreateInfo;

} //namespace lv

#endif
