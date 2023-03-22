#pragma once

#include "shared.h"
#include "wrapper/vk/device.h"
#include "wrapper/vk/buffer.h"

namespace vk {

class CommandBuffer {
public:
    CommandBuffer(const Device& device, const VkCommandBufferAllocateInfo& allocInfo);
    ~CommandBuffer();

    inline const VkCommandBuffer& get() const { return m_cmd; }
    
    void begin(const VkCommandBufferBeginInfo& beginInfo) const;
    void beginRenderPass(const VkRenderPassBeginInfo& renderPassInfo) const;

    void bindPipeline(const VkPipeline& pipeline) const;
    void bindVertexBuffers(const VkBuffer (&vertexBuffers)[], const VkDeviceSize (&offsets)[]) const;
    void bindIndexBuffer(const VkBuffer &indexBuffer, VkIndexType type) const;
    void bindDescriptorSets(VkPipelineBindPoint pipelineBindPoint, VkPipelineLayout layout, const VkDescriptorSet *descriptorSets, uint32_t firstSet = 0, uint32_t descriptorSetCount = 1, uint32_t dynamicOffsetCount = 0, const uint32_t *dynamicOffsets = nullptr) const;

    void setScissor(const VkRect2D& scissor) const;
    void setViewport(const VkViewport& viewport) const;

    void draw(uint32_t vertexCount, uint32_t instanceCount = 1, uint32_t firstVertex = 0, uint32_t firstInstance = 0) const;
    void drawIndexed(uint32_t indexCount, uint32_t instanceCount = 1, uint32_t firstIndex = 0, int32_t vertexOffset = 0, uint32_t firstInstance = 0) const;

    void copyBuffer(const Buffer& src, Buffer& dst, VkDeviceSize size) const;

    void reset() const;
    void end() const;
    void endRenderPass() const;

private:
    VkCommandBuffer m_cmd;

    const Device& m_device;
    const VkCommandPool& m_commandPool;
};

}