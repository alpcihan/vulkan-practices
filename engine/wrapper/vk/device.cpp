#include "wrapper/vk/device.h"

namespace vk {

Device::Device(const PhysicalDevice& physicalDevice) {
    vk::QueueFamilyIndices indices = physicalDevice.getQueueFamilyIndices();

    std::vector<VkDeviceQueueCreateInfo> queueCreateInfos;
    std::set<uint32_t> uniqueQueueFamilies = {indices.graphicsFamily.value(), indices.presentFamily.value()};

    float queuePriority = 1.0f;
    for (uint32_t queueFamily : uniqueQueueFamilies) {
        VkDeviceQueueCreateInfo queueCreateInfo{};
        queueCreateInfo.sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
        queueCreateInfo.queueFamilyIndex = queueFamily;
        queueCreateInfo.queueCount = 1;
        queueCreateInfo.pQueuePriorities = &queuePriority;
        queueCreateInfos.push_back(queueCreateInfo);
    }

    VkPhysicalDeviceFeatures deviceFeatures{};

    // create logical device
    VkDeviceCreateInfo createInfo{};
    createInfo.sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;
    createInfo.queueCreateInfoCount = static_cast<uint32_t>(queueCreateInfos.size());
    createInfo.pQueueCreateInfos = queueCreateInfos.data();
    createInfo.pEnabledFeatures = &deviceFeatures;

    createInfo.enabledExtensionCount = static_cast<uint32_t>(physicalDevice.getExtensions().size());
    createInfo.ppEnabledExtensionNames = physicalDevice.getExtensions().data();
    createInfo.enabledLayerCount = 0;

    // "Previous implementations of Vulkan made a distinction between instance and device specific validation layers,
    // but this is no longer the case. That means that the enabledLayerCount and ppEnabledLayerNames fields of
    // VkDeviceCreateInfo are ignored by up-to-date implementations.
    // However, it is still a good idea to set them anyway to be compatible with older implementations"
    // if (/* validation layer toggle check*/) {
    // createInfo.enabledLayerCount = static_cast<uint32_t>(m_validationLayers.size());
    // createInfo.ppEnabledLayerNames = m_validationLayers.data();
    // }

    if (vkCreateDevice(physicalDevice.get(), &createInfo, nullptr, &m_device) != VK_SUCCESS) {
        throw std::runtime_error("failed to create logical device!");
    }

    // set queues
    vkGetDeviceQueue(m_device, indices.presentFamily.value(), 0, &m_presentQueue);
    vkGetDeviceQueue(m_device, indices.graphicsFamily.value(), 0, &m_graphicsQueue);
}

Device::~Device() {
    vkDestroyDevice(m_device, nullptr);
}

void Device::waitIdle() const {
    vkDeviceWaitIdle(m_device);
}

}  // namespace vk