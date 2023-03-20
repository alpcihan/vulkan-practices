#pragma once

#include "shared.h"
#include "wrapper/vk/device.h"

namespace vk {

class CommandBuffer {
public:
    CommandBuffer(const Device& device, const VkCommandBufferAllocateInfo& allocInfo);

    inline const VkCommandBuffer& get() const { return m_commandBuffer; }
    
    void begin() const;
    void beginRenderPass(const VkRenderPassBeginInfo& renderPassInfo) const;

    void bindPipeline(const VkPipeline& pipeline) const;
    void bindVertexBuffers(const VkBuffer (&vertexBuffers)[], const VkDeviceSize (&offsets)[]) const;

    void setScissor(const VkRect2D& scissor) const;
    void setViewport(const VkViewport& viewport) const;

    void draw(uint32_t vertexCount, uint32_t instanceCount = 1, uint32_t firstVertex = 0, uint32_t firstInstance = 0) const;

    void reset() const;
    void end() const;
    void endRenderPass() const;

private:
    VkCommandBuffer m_commandBuffer;
};

}