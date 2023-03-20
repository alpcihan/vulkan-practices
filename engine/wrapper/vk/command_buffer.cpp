#include "wrapper/vk/command_buffer.h"

namespace vk {

CommandBuffer::CommandBuffer(const Device& device, const VkCommandBufferAllocateInfo& allocInfo)
:m_device(device), m_commandPool(allocInfo.commandPool) {
    // TODO: add multiple allocation support
    if (vkAllocateCommandBuffers(device.get(), &allocInfo, &m_commandBuffer) != VK_SUCCESS) {
        throw std::runtime_error("failed to allocate command buffers!");
    }
}

CommandBuffer::~CommandBuffer()
{
    vkFreeCommandBuffers(m_device.get(), m_commandPool, 1, &m_commandBuffer);
}

void CommandBuffer::begin(const VkCommandBufferBeginInfo& beginInfo) const {
    if (vkBeginCommandBuffer(m_commandBuffer, &beginInfo) != VK_SUCCESS) {
        throw std::runtime_error("failed to begin recording command buffer!");
    }
}

void CommandBuffer::beginRenderPass(const VkRenderPassBeginInfo& renderPassInfo) const {
    vkCmdBeginRenderPass(m_commandBuffer, &renderPassInfo, VK_SUBPASS_CONTENTS_INLINE);
}

void CommandBuffer::bindPipeline(const VkPipeline& pipeline) const {
    vkCmdBindPipeline(m_commandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, pipeline);
}

void CommandBuffer::bindVertexBuffers(const VkBuffer (&vertexBuffers)[], const VkDeviceSize (&offsets)[]) const {
    vkCmdBindVertexBuffers(m_commandBuffer, 0, 1, vertexBuffers, offsets);
}

void CommandBuffer::bindIndexBuffer(const VkBuffer &indexBuffer, VkIndexType type) const
{
    vkCmdBindIndexBuffer(m_commandBuffer, indexBuffer, 0, type);
}

void CommandBuffer::setScissor(const VkRect2D& scissor) const {
    vkCmdSetScissor(m_commandBuffer, 0, 1, &scissor);
}

void CommandBuffer::setViewport(const VkViewport& viewport) const {
    vkCmdSetViewport(m_commandBuffer, 0, 1, &viewport);
}

void CommandBuffer::draw(uint32_t vertexCount, uint32_t instanceCount, uint32_t firstVertex, uint32_t firstInstance) const {
    vkCmdDraw(m_commandBuffer, vertexCount, instanceCount, firstVertex, firstInstance);
}

void CommandBuffer::drawIndexed(uint32_t indexCount, uint32_t instanceCount, uint32_t firstIndex, int32_t vertexOffset, uint32_t firstInstance) const
{
    vkCmdDrawIndexed(m_commandBuffer, indexCount, 1, 0, 0, 0);
}

void CommandBuffer::copyBuffer(const Buffer& src, Buffer& dst, VkDeviceSize size) const {
    VkBufferCopy copyRegion{
        copyRegion.srcOffset = 0,  // Optional
        copyRegion.dstOffset = 0,  // Optional
        copyRegion.size = size};

    vkCmdCopyBuffer(m_commandBuffer, src.get(), dst.get(), 1, &copyRegion);
}

void CommandBuffer::reset() const {
    vkResetCommandBuffer(m_commandBuffer, 0);
}

void CommandBuffer::end() const {
    if (vkEndCommandBuffer(m_commandBuffer) != VK_SUCCESS) {
        throw std::runtime_error("failed to record command buffer!");
    }
}

void CommandBuffer::endRenderPass() const {
    vkCmdEndRenderPass(m_commandBuffer);
}

}