#pragma once

#include "shared.h"
#include "wrapper/vk/device.h"
#include "wrapper/vk/physical_device.h"

namespace vk {

class Buffer {
public:
    Buffer(const Device& device, const PhysicalDevice& physicalDevice, const VkBufferCreateInfo& bufferInfo, VkMemoryPropertyFlags properties);
    ~Buffer();

    inline const VkBuffer& get() const { return m_buffer; }
    inline const VkDeviceMemory& getMemory () const { return m_bufferMemory; } 

private:
    VkBuffer m_buffer;
    VkDeviceMemory m_bufferMemory;

    const Device& m_device;

private:
    uint32_t _findMemoryType(const PhysicalDevice& physicalDevice, uint32_t typeFilter, VkMemoryPropertyFlags properties) const;
};

}