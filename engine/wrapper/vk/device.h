#pragma once

#include "shared.h"
#include "wrapper/vk/physical_device.h"

namespace vk {

class Device {
public:
    Device(const PhysicalDevice& physicalDevice);
    ~Device();

    inline const VkDevice& get() const { return m_device; }
    const VkQueue& getPresentQueue() const { return m_presentQueue; }
    const VkQueue& getGraphicsQueue() const { return m_graphicsQueue; }

    void waitIdle() const;

private:
    VkDevice m_device;
    
    VkQueue m_presentQueue;
    VkQueue m_graphicsQueue;
};

}  // namespace vk