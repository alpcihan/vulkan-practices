#include "wrapper/vk/buffer.h"

namespace vk {

Buffer::Buffer(const Device& device, const PhysicalDevice& physicalDevice, const VkBufferCreateInfo& bufferInfo, VkMemoryPropertyFlags properties)
    : m_device(device) {
    if (vkCreateBuffer(m_device.get(), &bufferInfo, nullptr, &m_buffer) != VK_SUCCESS) {
        throw std::runtime_error("failed to create buffer!");
    }

    VkMemoryRequirements memRequirements;
    vkGetBufferMemoryRequirements(m_device.get(), m_buffer, &memRequirements);

    VkMemoryAllocateInfo allocInfo{
        .sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO,
        .allocationSize = memRequirements.size,
        .memoryTypeIndex = _findMemoryType(physicalDevice, memRequirements.memoryTypeBits, properties)};

    if (vkAllocateMemory(m_device.get(), &allocInfo, nullptr, &m_bufferMemory) != VK_SUCCESS) {
        throw std::runtime_error("failed to allocate buffer memory!");
    }

    vkBindBufferMemory(m_device.get(), m_buffer, m_bufferMemory, 0);
}

uint32_t Buffer::_findMemoryType(const PhysicalDevice& physicalDevice, uint32_t typeFilter, VkMemoryPropertyFlags properties) const {
    VkPhysicalDeviceMemoryProperties memProperties;
    vkGetPhysicalDeviceMemoryProperties(physicalDevice.get(), &memProperties);

    for (uint32_t i = 0; i < memProperties.memoryTypeCount; i++) {
        if ((typeFilter & (1 << i)) && (memProperties.memoryTypes[i].propertyFlags & properties) == properties) {
            return i;
        }
    }

    throw std::runtime_error("failed to find suitable memory type!");
}

Buffer::~Buffer() {
    vkDestroyBuffer(m_device.get(), m_buffer, nullptr);
    vkFreeMemory(m_device.get(), m_bufferMemory, nullptr);
}

}