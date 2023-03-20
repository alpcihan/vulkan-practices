#pragma once

#include "shared.h"
#include "wrapper/glfw/window.h"
#include "wrapper/vk/device.h"
#include "wrapper/vk/physical_device.h"

namespace vk {

class SwapChain {
public:
    SwapChain(const Device& device, const PhysicalDevice& physicalDevice, const glfw::Window& window);
    ~SwapChain();

    inline const VkSwapchainKHR& get() const { return m_swapChain; }
    inline const std::vector<VkImage>& getImages() const { return m_swapChainImages; }
    inline const std::vector<VkImageView>& getImageViews() const { return m_swapChainImageViews; }
    inline const VkFormat& getImageFormat() const { return m_swapChainImageFormat; }
    inline const VkExtent2D& getExtent() const { return m_swapChainExtent; }

    void create();
    void clean();

private:
    VkSurfaceFormatKHR _chooseSwapSurfaceFormat(const std::vector<VkSurfaceFormatKHR>& availableFormats);
    VkPresentModeKHR _chooseSwapPresentMode(const std::vector<VkPresentModeKHR>& availablePresentModes);
    VkExtent2D _chooseSwapExtent(const VkSurfaceCapabilitiesKHR& capabilities);

private:
    VkSwapchainKHR m_swapChain;

    std::vector<VkImage> m_swapChainImages;
    std::vector<VkImageView> m_swapChainImageViews;

    VkFormat m_swapChainImageFormat;
    VkExtent2D m_swapChainExtent;

    const Device& m_device;
    const PhysicalDevice& m_physicalDevice;
    const glfw::Window& m_window;

    bool m_isClean = true;
};

}