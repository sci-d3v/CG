#define VULKAN_HPP_DISPATCH_LOADER_DYNAMIC 1
#include <vulkan/vulkan.hpp>

#define GLFW_INCLUDE_NONE
#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>

VULKAN_HPP_DEFAULT_DISPATCH_LOADER_DYNAMIC_STORAGE

#include <functional>

#include <cstdlib>
#include <fstream>
#include <iostream>
#include <limits>
#include <optional>
#include <set>
#include <stdexcept>
#include <vector>

// #define NDEBUG

constexpr uint32_t WIDTH = 800;
constexpr uint32_t HEIGHT = 600;

struct Context {
  Context() {
    // Create window
    glfwInit();
    glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
    glfwWindowHint(GLFW_RESIZABLE, GLFW_FALSE);
    window_ =
        glfwCreateWindow(WIDTH, HEIGHT, "Vulkan Pathtracing", nullptr, nullptr);

    // Prepase extensions and layers
    uint32_t glfwExtensionCount = 0;
    const char** glfwExtensions =
        glfwGetRequiredInstanceExtensions(&glfwExtensionCount);
    std::vector extensions(glfwExtensions, glfwExtensions + glfwExtensionCount);
    extensions.push_back(VK_EXT_DEBUG_UTILS_EXTENSION_NAME);

    std::vector layers{"VK_LAYER_KHRONOS_validation"};

    auto vkGetInstanceProcAddr =
        dynamicLoader_.getProcAddress<PFN_vkGetInstanceProcAddr>(
            "vkGetInstanceProcAddr");
    VULKAN_HPP_DEFAULT_DISPATCHER.init(vkGetInstanceProcAddr);

    // Create instance
    vk::ApplicationInfo appInfo;
    appInfo.setApiVersion(VK_API_VERSION_1_3);

    vk::InstanceCreateInfo instanceInfo;
    instanceInfo.setPApplicationInfo(&appInfo);
    instanceInfo.setPEnabledLayerNames(layers);
    instanceInfo.setPEnabledExtensionNames(extensions);
    instance_ = vk::createInstanceUnique(instanceInfo);
    VULKAN_HPP_DEFAULT_DISPATCHER.init(*instance_);

    // Pick first gpu
    physicalDevice_ = instance_->enumeratePhysicalDevices().front();

    // Create debug messenger
    vk::DebugUtilsMessengerCreateInfoEXT messengerInfo;
    messengerInfo.setMessageSeverity(
        vk::DebugUtilsMessageSeverityFlagBitsEXT::eError);
    messengerInfo.setMessageType(
        vk::DebugUtilsMessageTypeFlagBitsEXT::eValidation);
    messengerInfo.setPfnUserCallback(&debugUtilsMessengerCallback);
    messenger_ = instance_->createDebugUtilsMessengerEXTUnique(messengerInfo);

    // Create surface
    VkSurfaceKHR _surface;
    VkResult res = glfwCreateWindowSurface(VkInstance(*instance_), window_,
                                           nullptr, &_surface);
    if (res != VK_SUCCESS) {
      throw std::runtime_error("failed to create window surface!");
    }
    surface_ = vk::UniqueSurfaceKHR(vk::SurfaceKHR(_surface), {*instance_});

    // Find queue family
    std::vector queueFamilies = physicalDevice_.getQueueFamilyProperties();
    for (int i = 0; i < queueFamilies.size(); i++) {
      auto supportCompute =
          queueFamilies[i].queueFlags & vk::QueueFlagBits::eCompute;
      auto supportPresent = physicalDevice_.getSurfaceSupportKHR(i, *surface_);
      if (supportCompute && supportPresent) {
        queueFamilyIndex_ = i;
      }
    }

    // Create device
    const float queuePriority = 1.0f;
    vk::DeviceQueueCreateInfo queueCreateInfo;
    queueCreateInfo.setQueueFamilyIndex(queueFamilyIndex_);
    queueCreateInfo.setQueuePriorities(queuePriority);

    const std::vector deviceExtensions{
        VK_KHR_SWAPCHAIN_EXTENSION_NAME,
        VK_KHR_DEDICATED_ALLOCATION_EXTENSION_NAME,
        VK_KHR_GET_MEMORY_REQUIREMENTS_2_EXTENSION_NAME,
        VK_KHR_MAINTENANCE3_EXTENSION_NAME,
        VK_KHR_PIPELINE_LIBRARY_EXTENSION_NAME,
        // VK_KHR_DEFERRED_HOST_OPERATIONS_EXTENSION_NAME,
        VK_KHR_BUFFER_DEVICE_ADDRESS_EXTENSION_NAME,
    };

    if (!checkDeviceExtensionSupport(deviceExtensions)) {
      throw std::runtime_error("Some required extensions are not supported");
    }

    vk::DeviceCreateInfo deviceInfo;
    deviceInfo.setQueueCreateInfos(queueCreateInfo);
    deviceInfo.setPEnabledExtensionNames(deviceExtensions);

    vk::PhysicalDeviceBufferDeviceAddressFeatures bufferDeviceAddressFeatures{
        true};
    // vk::PhysicalDeviceRayTracingPipelineFeaturesKHR rayTracingPipelineFeatures{true};
    // vk::PhysicalDeviceAccelerationStructureFeaturesKHR accelerationStructureFeatures{true};
    vk::StructureChain createInfoChain{
        deviceInfo, bufferDeviceAddressFeatures,
        // rayTracingPipelineFeatures,
        // accelerationStructureFeatures,
    };

    device_ = physicalDevice_.createDeviceUnique(
        createInfoChain.get<vk::DeviceCreateInfo>());
    VULKAN_HPP_DEFAULT_DISPATCHER.init(*device_);

    queue_ = device_->getQueue(queueFamilyIndex_, 0);

    // Create command pool
    vk::CommandPoolCreateInfo commandPoolInfo;
    commandPoolInfo.setFlags(
        vk::CommandPoolCreateFlagBits::eResetCommandBuffer);
    commandPoolInfo.setQueueFamilyIndex(queueFamilyIndex_);
    commandPool_ = device_->createCommandPoolUnique(commandPoolInfo);

    // Create descriptor pool
    std::vector<vk::DescriptorPoolSize> poolSizes{
        // {vk::DescriptorType::eAccelerationStructureKHR, 1},
        // {vk::DescriptorType::eStorageImage, 1},
        // {vk::DescriptorType::eStorageBuffer, 3},
    };

    vk::DescriptorPoolCreateInfo descPoolInfo;
    descPoolInfo.setPoolSizes(poolSizes);
    descPoolInfo.setMaxSets(1);
    descPoolInfo.setFlags(vk::DescriptorPoolCreateFlagBits::eFreeDescriptorSet);
    descriptorPool_ = device_->createDescriptorPoolUnique(descPoolInfo);
  }

  bool checkDeviceExtensionSupport(
      const std::vector<const char*>& requiredExtensions) const {
    std::vector<vk::ExtensionProperties> availableExtensions =
        physicalDevice_.enumerateDeviceExtensionProperties();
    std::vector<std::string> requiredExtensionNames(requiredExtensions.begin(),
                                                    requiredExtensions.end());

    for (const auto& extension : availableExtensions) {
      requiredExtensionNames.erase(
          std::remove(requiredExtensionNames.begin(),
                      requiredExtensionNames.end(), extension.extensionName),
          requiredExtensionNames.end());
    }

    if (requiredExtensionNames.empty()) {
      std::cout << "All required extensions are supported by the device."
                << std::endl;
      return true;
    } else {
      std::cout << "The following required extensions are not supported by the "
                   "device:"
                << std::endl;
      for (const auto& name : requiredExtensionNames) {
        std::cout << "\t" << name << std::endl;
      }
      return false;
    }
  }

  uint32_t findMemoryType(uint32_t typeFilter,
                          vk::MemoryPropertyFlags properties) const {
    vk::PhysicalDeviceMemoryProperties memProperties =
        physicalDevice_.getMemoryProperties();
    for (uint32_t i = 0; i != memProperties.memoryTypeCount; ++i) {
      if ((typeFilter & (1 << i)) &&
          (memProperties.memoryTypes[i].propertyFlags & properties) ==
              properties) {
        return i;
      }
    }
    throw std::runtime_error("failed to find suitable memory type");
  }

  void oneTimeSubmit(const std::function<void(vk::CommandBuffer)>& func) const {
    vk::CommandBufferAllocateInfo commandBufferInfo;
    commandBufferInfo.setCommandPool(*commandPool_);
    commandBufferInfo.setCommandBufferCount(1);

    vk::UniqueCommandBuffer commandBuffer = std::move(
        device_->allocateCommandBuffersUnique(commandBufferInfo).front());
    commandBuffer->begin({vk::CommandBufferUsageFlagBits::eOneTimeSubmit});
    func(*commandBuffer);
    commandBuffer->end();

    vk::SubmitInfo submitInfo;
    submitInfo.setCommandBuffers(*commandBuffer);
    queue_.submit(submitInfo);
    queue_.waitIdle();
  }

  vk::UniqueDescriptorSet allocateDescSet(
      vk::DescriptorSetLayout descSetLayout) {
    vk::DescriptorSetAllocateInfo descSetInfo;
    descSetInfo.setDescriptorPool(*descriptorPool_);
    descSetInfo.setSetLayouts(descSetLayout);
    return std::move(
        device_->allocateDescriptorSetsUnique(descSetInfo).front());
  }

  static VKAPI_ATTR VkBool32 VKAPI_CALL debugUtilsMessengerCallback(
      VkDebugUtilsMessageSeverityFlagBitsEXT messageSeverity,
      VkDebugUtilsMessageTypeFlagsEXT messageTypes,
      VkDebugUtilsMessengerCallbackDataEXT const* pCallbackData,
      void* pUserData) {
    std::cerr << pCallbackData->pMessage << std::endl;
    return VK_FALSE;
  }

  GLFWwindow* window_;
  vk::DynamicLoader dynamicLoader_;
  vk::UniqueInstance instance_;
  vk::UniqueDebugUtilsMessengerEXT messenger_;
  vk::UniqueSurfaceKHR surface_;
  vk::UniqueDevice device_;
  vk::PhysicalDevice physicalDevice_;
  uint32_t queueFamilyIndex_;
  vk::Queue queue_;
  vk::UniqueCommandPool commandPool_;
  vk::UniqueDescriptorPool descriptorPool_;
};

int main() {
  Context context;
  context.device_->waitIdle();
  glfwDestroyWindow(context.window_);
  glfwTerminate();
}
