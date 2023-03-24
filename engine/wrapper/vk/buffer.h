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
    inline const VkDeviceMemory& getMemory () const { return m_memory; }
    
    void setData(const void* data); // TODO: support different sizes

private:
    VkBuffer m_buffer;
    VkDeviceMemory m_memory;
    void* m_data;
    uint32_t m_size;

    const Device& m_device;
};

}