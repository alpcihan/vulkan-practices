#pragma once

#include "shared.h"
#include "wrapper/glfw/window.h"
#include "wrapper/vk/buffer.h"
#include "wrapper/vk/command_buffer.h"
#include "wrapper/vk/device.h"
#include "wrapper/vk/instance.h"
#include "wrapper/vk/physical_device.h"
#include "wrapper/vk/swap_chain.h"
#define GLM_FORCE_RADIANS
#include <chrono>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <stb_image.h>

namespace eng {

struct Vertex {
    glm::vec2 pos;
    glm::vec3 color;

    static VkVertexInputBindingDescription getBindingDescription() {
        VkVertexInputBindingDescription bindingDescription{
            .binding = 0,
            .stride = sizeof(Vertex),
            .inputRate = VK_VERTEX_INPUT_RATE_VERTEX};

        return bindingDescription;
    }

    static std::array<VkVertexInputAttributeDescription, 2> getAttributeDescriptions() {
        std::array<VkVertexInputAttributeDescription, 2> attributeDescriptions{};
        attributeDescriptions[0].binding = 0;
        attributeDescriptions[0].location = 0;
        attributeDescriptions[0].format = VK_FORMAT_R32G32_SFLOAT;
        attributeDescriptions[0].offset = offsetof(Vertex, pos);

        attributeDescriptions[1].binding = 0;
        attributeDescriptions[1].location = 1;
        attributeDescriptions[1].format = VK_FORMAT_R32G32B32_SFLOAT;
        attributeDescriptions[1].offset = offsetof(Vertex, color);

        return attributeDescriptions;
    }
};

struct UniformBufferObject {
    glm::mat4 model;
    glm::mat4 view;
    glm::mat4 proj;
};

const std::vector<Vertex> vertices = {
    {{-0.5f, -0.5f}, {1.0f, 0.0f, 0.0f}},
    {{0.5f, -0.5f}, {0.0f, 1.0f, 0.0f}},
    {{0.5f, 0.5f}, {0.0f, 0.0f, 1.0f}},
    {{-0.5f, 0.5f}, {1.0f, 1.0f, 1.0f}}};

const std::vector<uint16_t> indices = {
    0, 1, 2, 2, 3, 0};

class Application {
public:
    void run() {
        _init();
        _mainLoop();
        _cleanup();
    }

private:
    const int m_MAX_FRAMES_IN_FLIGHT = 2;
    uint32_t m_currentFrame = 0;

    glfw::Window* m_window;
    vk::Instance* m_instance;
    vk::PhysicalDevice* m_physicalDevice;
    vk::Device* m_device;
    vk::SwapChain* m_swapChain;

    vk::Buffer *m_vertexBuffer, *m_indexBuffer;
    std::vector<vk::Buffer> m_uniformBuffers;

    VkRenderPass m_renderPass;
    VkDescriptorPool m_descriptorPool;
    std::vector<VkDescriptorSet> m_descriptorSets;
    VkDescriptorSetLayout m_descriptorSetLayout;
    VkPipelineLayout m_pipelineLayout;
    VkPipeline m_graphicsPipeline;

    std::vector<VkFramebuffer> m_swapChainFramebuffers;
    VkCommandPool m_commandPool;
    std::vector<vk::CommandBuffer> m_commandBuffers;

    std::vector<VkSemaphore> m_imageAvailableSemaphores;
    std::vector<VkSemaphore> m_renderFinishedSemaphores;
    std::vector<VkFence> m_inFlightFences;

    bool m_framebufferResized = false;

private:
    void _init() {
        glfwInit();
        m_instance = new vk::Instance(true);
        m_window = new glfw::Window(*m_instance);
        m_window->setFramebufferResizeCallback(_framebufferResizeCallback, &m_framebufferResized);
        m_physicalDevice = new vk::PhysicalDevice(*m_instance, m_window->getSurface());
        m_device = new vk::Device(*m_physicalDevice);
        m_swapChain = new vk::SwapChain(*m_device, *m_physicalDevice, *m_window);
        _createRenderPass();
        _createFramebuffers();
        _createDescriptorSetLayout();
        _createGraphicsPipeline();
        _createCommandPool();
        _createVertexBuffer();
        _createIndexBuffer();
        _createUniformBuffers();
        _createDescriptorPool();
        _createDescriptorSets();
        _createCommandBuffer();
        _createSyncObjects();
    }

    void _createRenderPass() {
        VkAttachmentDescription colorAttachment{};
        colorAttachment.format = m_swapChain->getImageFormat();
        colorAttachment.samples = VK_SAMPLE_COUNT_1_BIT;
        colorAttachment.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
        colorAttachment.storeOp = VK_ATTACHMENT_STORE_OP_STORE;
        colorAttachment.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
        colorAttachment.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
        colorAttachment.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
        colorAttachment.finalLayout = VK_IMAGE_LAYOUT_PRESENT_SRC_KHR;

        VkAttachmentReference colorAttachmentRef{};
        colorAttachmentRef.attachment = 0;
        colorAttachmentRef.layout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;

        VkSubpassDescription subpass{};
        subpass.pipelineBindPoint = VK_PIPELINE_BIND_POINT_GRAPHICS;
        subpass.colorAttachmentCount = 1;
        subpass.pColorAttachments = &colorAttachmentRef;

        VkRenderPassCreateInfo renderPassInfo{};
        renderPassInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO;
        renderPassInfo.attachmentCount = 1;
        renderPassInfo.pAttachments = &colorAttachment;
        renderPassInfo.subpassCount = 1;
        renderPassInfo.pSubpasses = &subpass;

        VkSubpassDependency dependency{};
        dependency.srcSubpass = VK_SUBPASS_EXTERNAL;
        dependency.dstSubpass = 0;
        dependency.srcStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
        dependency.srcAccessMask = 0;
        dependency.dstStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
        dependency.dstAccessMask = VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT;
        renderPassInfo.dependencyCount = 1;
        renderPassInfo.pDependencies = &dependency;

        if (vkCreateRenderPass(m_device->get(), &renderPassInfo, nullptr, &m_renderPass) != VK_SUCCESS) {
            throw std::runtime_error("failed to create render pass!");
        }
    }

    void _createFramebuffers() {
        m_swapChainFramebuffers.resize(m_swapChain->getImageViews().size());

        for (size_t i = 0; i < m_swapChain->getImageViews().size(); i++) {
            VkImageView attachments[] = {m_swapChain->getImageViews()[i]};

            VkFramebufferCreateInfo framebufferInfo{};
            framebufferInfo.sType = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO;
            framebufferInfo.renderPass = m_renderPass;
            framebufferInfo.attachmentCount = 1;
            framebufferInfo.pAttachments = attachments;
            framebufferInfo.width = m_swapChain->getExtent().width;
            framebufferInfo.height = m_swapChain->getExtent().height;
            framebufferInfo.layers = 1;

            if (vkCreateFramebuffer(m_device->get(), &framebufferInfo, nullptr, &m_swapChainFramebuffers[i]) != VK_SUCCESS) {
                throw std::runtime_error("failed to create framebuffer!");
            }
        }
    }

    void _createDescriptorSetLayout() {
        VkDescriptorSetLayoutBinding uboLayoutBinding{};
        {
            uboLayoutBinding.binding = 0;
            uboLayoutBinding.descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
            uboLayoutBinding.descriptorCount = 1;
            uboLayoutBinding.stageFlags = VK_SHADER_STAGE_VERTEX_BIT;
            uboLayoutBinding.pImmutableSamplers = nullptr;  // Optional
        }

        VkDescriptorSetLayoutCreateInfo layoutInfo{};
        {
            layoutInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;
            layoutInfo.bindingCount = 1;
            layoutInfo.pBindings = &uboLayoutBinding;
        }

        if (vkCreateDescriptorSetLayout(m_device->get(), &layoutInfo, nullptr, &m_descriptorSetLayout) != VK_SUCCESS) {
            throw std::runtime_error("failed to create descriptor set layout!");
        }
    }

    void _createGraphicsPipeline() {
        auto vertShaderCode = help::readResource("shaders/bin/default_vert.spv");
        auto fragShaderCode = help::readResource("shaders/bin/default_frag.spv");

        VkShaderModule vertShaderModule = _createShaderModule(vertShaderCode);
        VkShaderModule fragShaderModule = _createShaderModule(fragShaderCode);

        VkPipelineShaderStageCreateInfo vertShaderStageInfo{};
        {
            vertShaderStageInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
            vertShaderStageInfo.stage = VK_SHADER_STAGE_VERTEX_BIT;
            vertShaderStageInfo.module = vertShaderModule;
            vertShaderStageInfo.pName = "main";
        }

        VkPipelineShaderStageCreateInfo fragShaderStageInfo{};
        {
            fragShaderStageInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
            fragShaderStageInfo.stage = VK_SHADER_STAGE_FRAGMENT_BIT;
            fragShaderStageInfo.module = fragShaderModule;
            fragShaderStageInfo.pName = "main";
        }

        VkPipelineShaderStageCreateInfo shaderStages[] = {vertShaderStageInfo, fragShaderStageInfo};

        std::vector<VkDynamicState> dynamicStates = {
            VK_DYNAMIC_STATE_VIEWPORT,
            VK_DYNAMIC_STATE_SCISSOR};

        VkPipelineDynamicStateCreateInfo dynamicState{};
        dynamicState.sType = VK_STRUCTURE_TYPE_PIPELINE_DYNAMIC_STATE_CREATE_INFO;
        dynamicState.dynamicStateCount = static_cast<uint32_t>(dynamicStates.size());
        dynamicState.pDynamicStates = dynamicStates.data();

        auto bindingDescription = Vertex::getBindingDescription();
        auto attributeDescriptions = Vertex::getAttributeDescriptions();
        VkPipelineVertexInputStateCreateInfo vertexInputInfo{};
        vertexInputInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO;
        vertexInputInfo.vertexBindingDescriptionCount = 1;
        vertexInputInfo.vertexAttributeDescriptionCount = static_cast<uint32_t>(attributeDescriptions.size());
        vertexInputInfo.pVertexBindingDescriptions = &bindingDescription;             // optional
        vertexInputInfo.pVertexAttributeDescriptions = attributeDescriptions.data();  // optional

        VkPipelineInputAssemblyStateCreateInfo inputAssembly{};
        inputAssembly.sType = VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO;
        inputAssembly.topology = VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST;
        inputAssembly.primitiveRestartEnable = VK_FALSE;

        VkViewport viewport{};
        viewport.x = 0.0f;
        viewport.y = 0.0f;
        viewport.width = (float)m_swapChain->getExtent().width;
        viewport.height = (float)m_swapChain->getExtent().height;
        viewport.minDepth = 0.0f;
        viewport.maxDepth = 1.0f;

        VkRect2D scissor{};
        scissor.offset = {0, 0};
        scissor.extent = m_swapChain->getExtent();

        VkPipelineViewportStateCreateInfo viewportState{};
        viewportState.sType = VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_STATE_CREATE_INFO;
        viewportState.viewportCount = 1;
        viewportState.pViewports = &viewport;
        viewportState.scissorCount = 1;
        viewportState.pScissors = &scissor;

        VkPipelineRasterizationStateCreateInfo rasterizer{};
        rasterizer.sType = VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_STATE_CREATE_INFO;
        rasterizer.depthClampEnable = VK_FALSE;
        rasterizer.rasterizerDiscardEnable = VK_FALSE;
        rasterizer.polygonMode = VK_POLYGON_MODE_FILL;
        rasterizer.lineWidth = 1.0f;
        rasterizer.cullMode = VK_CULL_MODE_BACK_BIT;
        rasterizer.frontFace = VK_FRONT_FACE_COUNTER_CLOCKWISE;
        rasterizer.depthBiasEnable = VK_FALSE;
        rasterizer.depthBiasConstantFactor = 0.0f;  // optional
        rasterizer.depthBiasClamp = 0.0f;           // optional
        rasterizer.depthBiasSlopeFactor = 0.0f;     // optional

        VkPipelineMultisampleStateCreateInfo multisampling{};
        multisampling.sType = VK_STRUCTURE_TYPE_PIPELINE_MULTISAMPLE_STATE_CREATE_INFO;
        multisampling.sampleShadingEnable = VK_FALSE;
        multisampling.rasterizationSamples = VK_SAMPLE_COUNT_1_BIT;
        multisampling.minSampleShading = 1.0f;           // optional
        multisampling.pSampleMask = nullptr;             // optional
        multisampling.alphaToCoverageEnable = VK_FALSE;  // optional
        multisampling.alphaToOneEnable = VK_FALSE;       // optional

        VkPipelineColorBlendAttachmentState colorBlendAttachment{};
        colorBlendAttachment.colorWriteMask = VK_COLOR_COMPONENT_R_BIT | VK_COLOR_COMPONENT_G_BIT | VK_COLOR_COMPONENT_B_BIT | VK_COLOR_COMPONENT_A_BIT;
        colorBlendAttachment.blendEnable = VK_FALSE;
        colorBlendAttachment.srcColorBlendFactor = VK_BLEND_FACTOR_ONE;   // optional
        colorBlendAttachment.dstColorBlendFactor = VK_BLEND_FACTOR_ZERO;  // optional
        colorBlendAttachment.colorBlendOp = VK_BLEND_OP_ADD;              // optional
        colorBlendAttachment.srcAlphaBlendFactor = VK_BLEND_FACTOR_ONE;   // optional
        colorBlendAttachment.dstAlphaBlendFactor = VK_BLEND_FACTOR_ZERO;  // optional
        colorBlendAttachment.alphaBlendOp = VK_BLEND_OP_ADD;              // optional

        VkPipelineColorBlendStateCreateInfo colorBlending{};
        colorBlending.sType = VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO;
        colorBlending.logicOpEnable = VK_FALSE;
        colorBlending.logicOp = VK_LOGIC_OP_COPY;  // optional
        colorBlending.attachmentCount = 1;
        colorBlending.pAttachments = &colorBlendAttachment;
        colorBlending.blendConstants[0] = 0.0f;  // optional
        colorBlending.blendConstants[1] = 0.0f;  // optional
        colorBlending.blendConstants[2] = 0.0f;  // optional
        colorBlending.blendConstants[3] = 0.0f;  // optional

        VkPipelineLayoutCreateInfo pipelineLayoutInfo{};
        {
            pipelineLayoutInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
            pipelineLayoutInfo.setLayoutCount = 1;
            pipelineLayoutInfo.pSetLayouts = &m_descriptorSetLayout;
            pipelineLayoutInfo.pushConstantRangeCount = 0;     // optional
            pipelineLayoutInfo.pPushConstantRanges = nullptr;  // optional
        }

        if (vkCreatePipelineLayout(m_device->get(), &pipelineLayoutInfo, nullptr, &m_pipelineLayout) != VK_SUCCESS) {
            throw std::runtime_error("failed to create pipeline layout!");
        }

        VkGraphicsPipelineCreateInfo pipelineInfo{
            .sType = VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO,
            .stageCount = 2,
            .pStages = shaderStages,
            .pVertexInputState = &vertexInputInfo,
            .pInputAssemblyState = &inputAssembly,
            .pViewportState = &viewportState,
            .pRasterizationState = &rasterizer,
            .pMultisampleState = &multisampling,
            .pDepthStencilState = nullptr,  // optional
            .pColorBlendState = &colorBlending,
            .pDynamicState = &dynamicState,
            .layout = m_pipelineLayout,
            .renderPass = m_renderPass,
            .subpass = 0,
            .basePipelineHandle = VK_NULL_HANDLE,  // optional to derive new pipeline from the current one
            .basePipelineIndex = -1                // optional to derive new pipeline from the current one
        };

        if (vkCreateGraphicsPipelines(m_device->get(), VK_NULL_HANDLE, 1, &pipelineInfo, nullptr, &m_graphicsPipeline) != VK_SUCCESS) {
            throw std::runtime_error("failed to create graphics pipeline!");
        }

        // free the shader modules
        vkDestroyShaderModule(m_device->get(), fragShaderModule, nullptr);
        vkDestroyShaderModule(m_device->get(), vertShaderModule, nullptr);
    }

    void _createCommandPool() {
        vk::QueueFamilyIndices queueFamilyIndices = m_physicalDevice->getQueueFamilyIndices();
        VkCommandPoolCreateInfo poolInfo{
            .sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO,
            .flags = VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT,
            .queueFamilyIndex = queueFamilyIndices.graphicsFamily.value()};

        if (vkCreateCommandPool(m_device->get(), &poolInfo, nullptr, &m_commandPool) != VK_SUCCESS) {
            throw std::runtime_error("failed to create command pool!");
        }
    }

    void _createVertexBuffer() {
        VkDeviceSize bufferSize = sizeof(vertices[0]) * vertices.size();

        // staging buffer
        VkBufferCreateInfo bufferInfo = vk::bufferCreateInfo();
        bufferInfo.size = bufferSize;
        bufferInfo.usage = VK_BUFFER_USAGE_TRANSFER_SRC_BIT;
        vk::Buffer stagingBuffer(*m_device, *m_physicalDevice, bufferInfo,
                                 VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT);

        stagingBuffer.setData(vertices.data());

        bufferInfo.usage = VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_VERTEX_BUFFER_BIT;
        m_vertexBuffer = new vk::Buffer(*m_device, *m_physicalDevice, bufferInfo, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT | VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT);

        // copy
        VkCommandBufferAllocateInfo allocInfo = vk::commandBufferAllocateInfo();
        allocInfo.commandPool = m_commandPool;
        vk::CommandBuffer cmd(*m_device, allocInfo);

        VkCommandBufferBeginInfo beginInfo = vk::commandBufferBeginInfo();
        beginInfo.flags = VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT;
        cmd.begin(beginInfo);

        cmd.copyBuffer(stagingBuffer, *m_vertexBuffer, bufferSize);
        cmd.end();

        VkSubmitInfo submitInfo{};
        submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
        submitInfo.commandBufferCount = 1;
        submitInfo.pCommandBuffers = &cmd.get();

        vkQueueSubmit(m_device->getGraphicsQueue(), 1, &submitInfo, VK_NULL_HANDLE);
        vkQueueWaitIdle(m_device->getGraphicsQueue());
    }

    void _createIndexBuffer() {
        VkDeviceSize bufferSize = sizeof(indices[0]) * indices.size();

        VkBufferCreateInfo bufferInfo = vk::bufferCreateInfo();
        bufferInfo.size = bufferSize;
        bufferInfo.usage = VK_BUFFER_USAGE_TRANSFER_SRC_BIT;
        vk::Buffer stagingBuffer(*m_device, *m_physicalDevice, bufferInfo,
                                 VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT);

        stagingBuffer.setData(indices.data());

        bufferInfo.usage = VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_INDEX_BUFFER_BIT;
        m_indexBuffer = new vk::Buffer(*m_device, *m_physicalDevice, bufferInfo, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT | VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT);

        // copy
        VkCommandBufferAllocateInfo allocInfo = vk::commandBufferAllocateInfo();
        allocInfo.commandPool = m_commandPool;
        vk::CommandBuffer cmd(*m_device, allocInfo);

        VkCommandBufferBeginInfo beginInfo = vk::commandBufferBeginInfo();
        beginInfo.flags = VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT;
        cmd.begin(beginInfo);

        cmd.copyBuffer(stagingBuffer, *m_indexBuffer, bufferSize);
        cmd.end();

        VkSubmitInfo submitInfo{};
        submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
        submitInfo.commandBufferCount = 1;
        submitInfo.pCommandBuffers = &cmd.get();

        vkQueueSubmit(m_device->getGraphicsQueue(), 1, &submitInfo, VK_NULL_HANDLE);
        vkQueueWaitIdle(m_device->getGraphicsQueue());
    }

    void _createUniformBuffers() {
        VkDeviceSize bufferSize = sizeof(UniformBufferObject);
        m_uniformBuffers.reserve(m_MAX_FRAMES_IN_FLIGHT);

        VkBufferCreateInfo bufferInfo = vk::bufferCreateInfo();
        bufferInfo.size = bufferSize;
        bufferInfo.usage = VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT;

        // TODO: use resize
        for (int i = 0; i < m_MAX_FRAMES_IN_FLIGHT; i++) {
            m_uniformBuffers.emplace_back(*m_device, *m_physicalDevice, bufferInfo,
                                          VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT);
        }
    }

    void _createDescriptorPool() {
        VkDescriptorPoolSize poolSize{};
        {
            poolSize.type = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
            poolSize.descriptorCount = static_cast<uint32_t>(m_MAX_FRAMES_IN_FLIGHT);
        }

        VkDescriptorPoolCreateInfo poolInfo{};
        {
            poolInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
            poolInfo.poolSizeCount = 1;
            poolInfo.pPoolSizes = &poolSize;
            poolInfo.maxSets = static_cast<uint32_t>(m_MAX_FRAMES_IN_FLIGHT);
        }

        if (vkCreateDescriptorPool(m_device->get(), &poolInfo, nullptr, &m_descriptorPool) != VK_SUCCESS) {
            throw std::runtime_error("failed to create descriptor pool!");
        }
    }

    void _createDescriptorSets() {
        std::vector<VkDescriptorSetLayout> layouts(m_MAX_FRAMES_IN_FLIGHT, m_descriptorSetLayout);
        VkDescriptorSetAllocateInfo allocInfo{};
        {
            allocInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO;
            allocInfo.descriptorPool = m_descriptorPool;
            allocInfo.descriptorSetCount = static_cast<uint32_t>(m_MAX_FRAMES_IN_FLIGHT);
            allocInfo.pSetLayouts = layouts.data();
        }

        m_descriptorSets.resize(m_MAX_FRAMES_IN_FLIGHT);
        if (vkAllocateDescriptorSets(m_device->get(), &allocInfo, m_descriptorSets.data()) != VK_SUCCESS) {
            throw std::runtime_error("failed to allocate descriptor sets!");
        }

        for (size_t i = 0; i < m_MAX_FRAMES_IN_FLIGHT; i++) {
            VkDescriptorBufferInfo bufferInfo{
                .buffer = m_uniformBuffers[i].get(),
                .offset = 0,
                .range = sizeof(UniformBufferObject)};

            VkWriteDescriptorSet descriptorWrite{};
            {
                descriptorWrite.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
                descriptorWrite.dstSet = m_descriptorSets[i];
                descriptorWrite.dstBinding = 0;
                descriptorWrite.dstArrayElement = 0;
                descriptorWrite.descriptorCount = 1;
                descriptorWrite.descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
                descriptorWrite.pBufferInfo = &bufferInfo;
                descriptorWrite.pTexelBufferView = nullptr;  // Optional
                descriptorWrite.pImageInfo = nullptr;        // Optional
            }

            vkUpdateDescriptorSets(m_device->get(), 1, &descriptorWrite, 0, nullptr);
        }
    }

    VkShaderModule _createShaderModule(const std::vector<char>& code) {
        VkShaderModuleCreateInfo createInfo{};
        createInfo.sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO;
        createInfo.codeSize = code.size();
        createInfo.pCode = reinterpret_cast<const uint32_t*>(code.data());

        VkShaderModule shaderModule;
        if (vkCreateShaderModule(m_device->get(), &createInfo, nullptr, &shaderModule) != VK_SUCCESS) {
            throw std::runtime_error("failed to create shader module!");
        }

        return shaderModule;
    }

    void _createCommandBuffer() {
        m_commandBuffers.reserve(m_MAX_FRAMES_IN_FLIGHT);

        VkCommandBufferAllocateInfo allocInfo = vk::commandBufferAllocateInfo();
        allocInfo.commandPool = m_commandPool;

        // TODO: use resize
        for (int i = 0; i < m_MAX_FRAMES_IN_FLIGHT; i++) {
            m_commandBuffers.emplace_back(*m_device, allocInfo);
        }
    }

    void _recordCommandBuffer(const vk::CommandBuffer& cmd, uint32_t imageIndex) {
        cmd.begin(vk::commandBufferBeginInfo());

        VkRenderPassBeginInfo renderPassInfo = vk::renderPassBeginInfo();
        {
            renderPassInfo.renderPass = m_renderPass;
            renderPassInfo.framebuffer = m_swapChainFramebuffers[imageIndex];
            renderPassInfo.renderArea.extent = m_swapChain->getExtent();
        }
        cmd.beginRenderPass(renderPassInfo);
        cmd.bindPipeline(m_graphicsPipeline);

        VkBuffer vertexBuffers[] = {m_vertexBuffer->get()};
        VkDeviceSize offsets[] = {0};
        cmd.bindVertexBuffers(vertexBuffers, offsets);
        cmd.bindIndexBuffer(m_indexBuffer->get(), VK_INDEX_TYPE_UINT16);  // TODO: add index buffer wrapper binding

        VkViewport viewport = vk::viewport();
        {
            viewport.width = static_cast<float>(m_swapChain->getExtent().width);
            viewport.height = static_cast<float>(m_swapChain->getExtent().height);
        }
        cmd.setViewport(viewport);

        VkRect2D scissor = vk::rect2D();
        scissor.extent = m_swapChain->getExtent();
        cmd.setScissor(scissor);

        cmd.bindDescriptorSets(VK_PIPELINE_BIND_POINT_GRAPHICS, m_pipelineLayout, &m_descriptorSets[m_currentFrame]);
        cmd.drawIndexed(static_cast<uint32_t>(indices.size()));
        cmd.endRenderPass();

        cmd.end();
    }

    static void _framebufferResizeCallback(uint32_t width, uint32_t height, void* callbackData) {
        *(bool*)callbackData = true;
    }

    void _mainLoop() {
        while (!glfwWindowShouldClose(m_window->get())) {
            glfwPollEvents();
            _drawFrame();
        }

        m_device->waitIdle();
    }

    void _drawFrame() {
        vkWaitForFences(m_device->get(), 1, &m_inFlightFences[m_currentFrame], VK_TRUE, UINT64_MAX);

        uint32_t imageIndex;
        VkResult result = vkAcquireNextImageKHR(m_device->get(), m_swapChain->get(), UINT64_MAX, m_imageAvailableSemaphores[m_currentFrame], VK_NULL_HANDLE, &imageIndex);

        if (result == VK_ERROR_OUT_OF_DATE_KHR) {
            _recreateSwapChain();
            return;
        } else if (result != VK_SUCCESS && result != VK_SUBOPTIMAL_KHR) {
            throw std::runtime_error("failed to acquire swap chain image!");
        }

        vkResetFences(m_device->get(), 1, &m_inFlightFences[m_currentFrame]);

        _updateUniformBuffer(m_currentFrame);

        m_commandBuffers[m_currentFrame].reset();
        _recordCommandBuffer(m_commandBuffers[m_currentFrame], imageIndex);

        VkSemaphore waitSemaphores[] = {m_imageAvailableSemaphores[m_currentFrame]};
        VkSemaphore signalSemaphores[] = {m_renderFinishedSemaphores[m_currentFrame]};
        VkPipelineStageFlags waitStages[] = {VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT};
        VkSubmitInfo submitInfo{
            .sType = VK_STRUCTURE_TYPE_SUBMIT_INFO,
            .waitSemaphoreCount = 1,
            .pWaitSemaphores = waitSemaphores,
            .pWaitDstStageMask = waitStages,
            .commandBufferCount = 1,
            .pCommandBuffers = &m_commandBuffers[m_currentFrame].get(),
            .signalSemaphoreCount = 1,
            .pSignalSemaphores = signalSemaphores};

        if (vkQueueSubmit(m_device->getGraphicsQueue(), 1, &submitInfo, m_inFlightFences[m_currentFrame]) != VK_SUCCESS) {
            throw std::runtime_error("failed to submit draw command buffer!");
        }

        VkSwapchainKHR swapChains[] = {m_swapChain->get()};
        VkPresentInfoKHR presentInfo{
            .sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR,
            .waitSemaphoreCount = 1,
            .pWaitSemaphores = signalSemaphores,
            .swapchainCount = 1,
            .pSwapchains = swapChains,
            .pImageIndices = &imageIndex,
            .pResults = nullptr  // optional
        };

        result = vkQueuePresentKHR(m_device->getPresentQueue(), &presentInfo);

        if (result == VK_ERROR_OUT_OF_DATE_KHR || result == VK_SUBOPTIMAL_KHR || m_framebufferResized) {
            m_framebufferResized = false;
            _recreateSwapChain();
        } else if (result != VK_SUCCESS) {
            throw std::runtime_error("failed to present swap chain image!");
        }

        m_currentFrame = (m_currentFrame + 1) % m_MAX_FRAMES_IN_FLIGHT;
    }

    void _updateUniformBuffer(uint32_t currentImage) {
        static auto startTime = std::chrono::high_resolution_clock::now();

        auto currentTime = std::chrono::high_resolution_clock::now();
        float time = std::chrono::duration<float, std::chrono::seconds::period>(currentTime - startTime).count();

        UniformBufferObject ubo{};
        ubo.model = glm::rotate(glm::mat4(1.0f), time * glm::radians(90.0f), glm::vec3(0.0f, 0.0f, 1.0f));
        ubo.view = glm::lookAt(glm::vec3(2.0f, 2.0f, 2.0f), glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, 0.0f, 1.0f));
        ubo.proj = glm::perspective(glm::radians(45.0f), m_swapChain->getExtent().width / (float)m_swapChain->getExtent().height, 0.1f, 10.0f);
        ubo.proj[1][1] *= -1;

        m_uniformBuffers[currentImage].setData(&ubo);
    }

    void _createSyncObjects() {
        m_imageAvailableSemaphores.resize(m_MAX_FRAMES_IN_FLIGHT);
        m_renderFinishedSemaphores.resize(m_MAX_FRAMES_IN_FLIGHT);
        m_inFlightFences.resize(m_MAX_FRAMES_IN_FLIGHT);

        VkSemaphoreCreateInfo semaphoreInfo{
            .sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO};

        VkFenceCreateInfo fenceInfo{
            .sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO,
            .flags = VK_FENCE_CREATE_SIGNALED_BIT};

        for (size_t i = 0; i < m_MAX_FRAMES_IN_FLIGHT; i++) {
            if (vkCreateSemaphore(m_device->get(), &semaphoreInfo, nullptr, &m_imageAvailableSemaphores[i]) != VK_SUCCESS ||
                vkCreateSemaphore(m_device->get(), &semaphoreInfo, nullptr, &m_renderFinishedSemaphores[i]) != VK_SUCCESS ||
                vkCreateFence(m_device->get(), &fenceInfo, nullptr, &m_inFlightFences[i]) != VK_SUCCESS) {
                throw std::runtime_error("failed to create synchronization objects for a frame!");
            }
        }
    }

    void _cleanupSwapChain() {
        for (size_t i = 0; i < m_swapChainFramebuffers.size(); i++) {
            vkDestroyFramebuffer(m_device->get(), m_swapChainFramebuffers[i], nullptr);
        }

        m_swapChain->clean();
    }

    void _recreateSwapChain() {
        int width = 0, height = 0;
        m_window->getFramebufferSize(width, height);
        while (width == 0 || height == 0) {
            m_window->getFramebufferSize(width, height);
            glfwWaitEvents();
        }

        m_device->waitIdle();

        _cleanupSwapChain();

        m_physicalDevice->updateSwapChainSupportDetails(m_window->getSurface());
        m_swapChain->create();
        _createFramebuffers();
    }

    void _cleanup() {
        _cleanupSwapChain();

        m_uniformBuffers.clear();

        vkDestroyDescriptorPool(m_device->get(), m_descriptorPool, nullptr);
        vkDestroyDescriptorSetLayout(m_device->get(), m_descriptorSetLayout, nullptr);

        // buffers
        delete m_vertexBuffer;
        delete m_indexBuffer;

        // pipeline
        vkDestroyPipeline(m_device->get(), m_graphicsPipeline, nullptr);
        vkDestroyPipelineLayout(m_device->get(), m_pipelineLayout, nullptr);
        vkDestroyRenderPass(m_device->get(), m_renderPass, nullptr);

        // synchronization objects
        for (size_t i = 0; i < m_MAX_FRAMES_IN_FLIGHT; i++) {
            vkDestroySemaphore(m_device->get(), m_renderFinishedSemaphores[i], nullptr);
            vkDestroySemaphore(m_device->get(), m_imageAvailableSemaphores[i], nullptr);
            vkDestroyFence(m_device->get(), m_inFlightFences[i], nullptr);
        }

        // command pool
        vkDestroyCommandPool(m_device->get(), m_commandPool, nullptr);

        // device
        delete m_swapChain;
        delete m_device;
        delete m_window;
        delete m_instance;
        glfwTerminate();
    }
};

}  // namespace eng