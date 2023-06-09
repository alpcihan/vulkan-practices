#include "wrapper/vk/buffer.h"

namespace vk {

Buffer::Buffer(const Device& device, const PhysicalDevice& physicalDevice, const VkBufferCreateInfo& bufferInfo, VkMemoryPropertyFlags properties)
    : m_device(device), m_size(bufferInfo.size) {
    if (vkCreateBuffer(m_device.get(), &bufferInfo, nullptr, &m_buffer) != VK_SUCCESS) {
        throw std::runtime_error("failed to create buffer!");
    }

    VkMemoryRequirements memRequirements;
    vkGetBufferMemoryRequirements(m_device.get(), m_buffer, &memRequirements);

    VkMemoryAllocateInfo allocInfo{
        .sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO,
        .allocationSize = memRequirements.size,
        .memoryTypeIndex = physicalDevice.findMemoryType(memRequirements.memoryTypeBits, properties)};

    if (vkAllocateMemory(m_device.get(), &allocInfo, nullptr, &m_memory) != VK_SUCCESS) {
        throw std::runtime_error("failed to allocate buffer memory!");
    }

    vkBindBufferMemory(m_device.get(), m_buffer, m_memory, 0);
    vkMapMemory(m_device.get(), m_memory, 0, m_size, 0, &m_data);
}

Buffer::~Buffer() {
    vkUnmapMemory(m_device.get(), m_memory); // TODO: check optimal place to call
    vkDestroyBuffer(m_device.get(), m_buffer, nullptr);
    vkFreeMemory(m_device.get(), m_memory, nullptr);
}

void Buffer::setData(const void* data)
{
    memcpy(m_data, data, m_size);
}

}