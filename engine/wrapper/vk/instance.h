#pragma once

#include "shared.h"

namespace vk {

class Instance {
public:
    Instance(bool enableValidationLayers = false);
    ~Instance();

    const VkInstance& get() const { return m_instance; }

private:
    VkInstance m_instance;

private:
    const std::vector<const char*> _getRequiredExtensions() const;

    // validation layer
private:
    bool m_isValidationLayersEnabled;
    VkDebugUtilsMessengerEXT m_debugMessenger;
    const std::vector<const char*> m_validationLayers = {"VK_LAYER_KHRONOS_validation"};

private:
    bool _checkValidationLayerSupport();
    VkDebugUtilsMessengerCreateInfoEXT _populateDebugMessengerCreateInfo() const;
    static VKAPI_ATTR VkBool32 VKAPI_CALL _debugCallback(
        VkDebugUtilsMessageSeverityFlagBitsEXT messageSeverity,
        VkDebugUtilsMessageTypeFlagsEXT messageType,
        const VkDebugUtilsMessengerCallbackDataEXT* pCallbackData,
        void* pUserData);
    VkResult _createDebugUtilsMessengerEXT(const VkDebugUtilsMessengerCreateInfoEXT* pCreateInfo, const VkAllocationCallbacks* pAllocator);
    void _destroyDebugUtilsMessengerEXT(const VkAllocationCallbacks* pAllocator);
};

}