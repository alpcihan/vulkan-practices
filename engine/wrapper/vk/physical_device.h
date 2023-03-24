#pragma once

#include "shared.h"
#include "wrapper/vk/instance.h"

namespace vk {

struct QueueFamilyIndices {
    std::optional<uint32_t> graphicsFamily;
    std::optional<uint32_t> presentFamily;

    bool isComplete() {
        return graphicsFamily.has_value() && presentFamily.has_value();
    }
};

struct SwapChainSupportDetails {
    VkSurfaceCapabilitiesKHR capabilities;
    std::vector<VkSurfaceFormatKHR> formats;
    std::vector<VkPresentModeKHR> presentModes;
};

class PhysicalDevice {
public:
    PhysicalDevice(const Instance& instance, const VkSurfaceKHR& surface);

public:
    inline const VkPhysicalDevice& get() const { return m_physicalDevice; }
    inline const QueueFamilyIndices& getQueueFamilyIndices() const { return m_queueFamilyIndices; }
    inline const SwapChainSupportDetails& getSwapChainSupportDetails() const { return m_swapChainSupportDetails; }
    inline const std::vector<const char*>& getExtensions() const { return m_deviceExtensions; }

    uint32_t findMemoryType(uint32_t typeFilter, VkMemoryPropertyFlags properties) const;

    // eg: on windows resize
    void updateSwapChainSupportDetails(const VkSurfaceKHR& surface); // TODO: check if this is possible to automate

private: 
    VkPhysicalDevice m_physicalDevice = VK_NULL_HANDLE;
    QueueFamilyIndices m_queueFamilyIndices;
    SwapChainSupportDetails m_swapChainSupportDetails;

    const std::vector<const char*> m_deviceExtensions = {
        VK_KHR_SWAPCHAIN_EXTENSION_NAME};

private:
    bool _isDeviceSuitable(VkPhysicalDevice physicalDevice, const VkSurfaceKHR& surface);
    bool _checkDeviceExtensionSupport(VkPhysicalDevice device);
    QueueFamilyIndices _findQueueFamilies(VkPhysicalDevice physicalDevice, const VkSurfaceKHR& surface);
    SwapChainSupportDetails _querySwapChainSupport(VkPhysicalDevice physicalDevice, const VkSurfaceKHR& surface);

};

}