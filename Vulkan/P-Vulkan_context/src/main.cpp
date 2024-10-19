#define GLFW_INCLUDE_NONE
#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>

#include <vulkan/vulkan.hpp>

#include <iostream>
#include <limits>
#include <optional>
#include <set>
#include <stdexcept>
#include <vector>

const uint32_t WIDTH = 800;
const uint32_t HEIGHT = 600;

const std::vector<const char*> validationLayers = {
    "VK_LAYER_KHRONOS_validation"};

const std::vector<const char*> deviceExtensions = {
    VK_KHR_SWAPCHAIN_EXTENSION_NAME,
    // VK_KHR_STORAGE_BUFFER_STORAGE_CLASS_EXTENSION_NAME
};

#ifdef NDEBUG
const bool enableValidationLayers = false;
#else
const bool enableValidationLayers = true;
#endif

PFN_vkCreateDebugUtilsMessengerEXT pfnVkCreateDebugUtilsMessengerEXT;
PFN_vkDestroyDebugUtilsMessengerEXT pfnVkDestroyDebugUtilsMessengerEXT;

VKAPI_ATTR VkResult VKAPI_CALL vkCreateDebugUtilsMessengerEXT(
    VkInstance instance, const VkDebugUtilsMessengerCreateInfoEXT* pCreateInfo,
    const VkAllocationCallbacks* pAllocator,
    VkDebugUtilsMessengerEXT* pMessenger) {
  return pfnVkCreateDebugUtilsMessengerEXT(instance, pCreateInfo, pAllocator,
                                           pMessenger);
}

VKAPI_ATTR void VKAPI_CALL vkDestroyDebugUtilsMessengerEXT(
    VkInstance instance, VkDebugUtilsMessengerEXT messenger,
    VkAllocationCallbacks const* pAllocator) {
  return pfnVkDestroyDebugUtilsMessengerEXT(instance, messenger, pAllocator);
}

struct QueueFamilyIndices {
  std::optional<uint32_t> graphicsFamily;
  std::optional<uint32_t> presentFamily;

  bool isComplete() {
    return graphicsFamily.has_value() && presentFamily.has_value();
  }
};

struct SwapChainSupportDetails {
  vk::SurfaceCapabilitiesKHR capabilities;
  std::vector<vk::SurfaceFormatKHR> formats;
  std::vector<vk::PresentModeKHR> presentModes;
};

class ContextApplication {
 public:
  void run() {
    initWindow();
    initVulkan();
    mainLoop();
    cleanup();
  }

 private:
  GLFWwindow* window_ = nullptr;

  vk::Instance instance_;
  vk::DebugUtilsMessengerEXT debugMessenger_;
  vk::SurfaceKHR surface_;

  vk::PhysicalDevice physicalDevice_;
  vk::Device device_;

  vk::Queue queue_;
  vk::CommandPool commandPool_;

  // Переменные для подсчета FPS
  double previousTime_ = 0.0;
  uint32_t frameCount_ = 0;

  bool framebufferResized_ = false;

  void initWindow() {
    glfwInit();

    glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);

    window_ =
        glfwCreateWindow(WIDTH, HEIGHT, "vulkan_context", nullptr, nullptr);
    glfwSetWindowUserPointer(window_, this);
    glfwSetFramebufferSizeCallback(window_, framebufferResizeCallback);
  }

  static void framebufferResizeCallback(GLFWwindow* window, int width,
                                        int height) {
    auto app =
        reinterpret_cast<ContextApplication*>(glfwGetWindowUserPointer(window));
    app->framebufferResized_ = true;
  }

  void initVulkan() {
    createInstance();
    setupDebugMessenger();
    createSurface();
    pickPhysicalDevice();
    createLogicalDevice();
    createCommandPool();
  }

  void mainLoop() {
    while (!glfwWindowShouldClose(window_)) {
      glfwPollEvents();
      drawFrame();
    }

    device_.waitIdle();
  }

  void cleanup() {

    device_.destroy(commandPool_);
    device_.destroy();

    if (enableValidationLayers) {
      instance_.destroyDebugUtilsMessengerEXT(debugMessenger_);
    }

    instance_.destroy(surface_);
    instance_.destroy();

    glfwDestroyWindow(window_);
    glfwTerminate();
  }

  void recreateSwapChain() {
    int width = 0, height = 0;
    glfwGetFramebufferSize(window_, &width, &height);
    while (width == 0 || height == 0) {
      glfwGetFramebufferSize(window_, &width, &height);
      glfwWaitEvents();
    }

    device_.waitIdle();
  }

  void createInstance() {
    if (enableValidationLayers && !checkValidationLayerSupport()) {
      throw std::runtime_error(
          "validation layers requested, but not available!");
    }

    vk::ApplicationInfo appInfo{};
    appInfo.pApplicationName = "Hello VulkanContext";
    appInfo.applicationVersion = VK_MAKE_VERSION(1, 0, 0);
    appInfo.pEngineName = "No Engine";
    appInfo.engineVersion = VK_MAKE_VERSION(1, 0, 0);
    appInfo.apiVersion = VK_API_VERSION_1_0;

    vk::InstanceCreateInfo createInfo{};
    createInfo.pApplicationInfo = &appInfo;
    auto extensions = getRequiredExtensions();

    // [&]() {
    //   std::cout << "Required Vulkan Extensions:" << std::endl;
    //   for (const auto& extension : extensions) {
    //     std::cout << "\t" << extension << std::endl;
    //   }
    // }();

    createInfo.enabledExtensionCount = extensions.size();
    createInfo.ppEnabledExtensionNames = extensions.data();

    vk::DebugUtilsMessengerCreateInfoEXT debugCreateInfo{};
    if (enableValidationLayers) {
      createInfo.enabledLayerCount = validationLayers.size();
      createInfo.ppEnabledLayerNames = validationLayers.data();

      populateDebugMessengerCreateInfo(debugCreateInfo);
      createInfo.pNext = &debugCreateInfo;
    } else {
      createInfo.enabledLayerCount = 0;
    }

    instance_ = vk::createInstance(createInfo);
  }

  void populateDebugMessengerCreateInfo(
      vk::DebugUtilsMessengerCreateInfoEXT& createInfo) {
    createInfo.messageSeverity =
        vk::DebugUtilsMessageSeverityFlagBitsEXT::eInfo |
        vk::DebugUtilsMessageSeverityFlagBitsEXT::eVerbose |
        vk::DebugUtilsMessageSeverityFlagBitsEXT::eWarning |
        vk::DebugUtilsMessageSeverityFlagBitsEXT::eError;
    createInfo.messageType =
        vk::DebugUtilsMessageTypeFlagBitsEXT::eDeviceAddressBinding |
        vk::DebugUtilsMessageTypeFlagBitsEXT::eGeneral |
        vk::DebugUtilsMessageTypeFlagBitsEXT::eValidation |
        vk::DebugUtilsMessageTypeFlagBitsEXT::ePerformance;
    createInfo.pfnUserCallback = debugCallback;
    createInfo.pUserData = nullptr;
  }

  void setupDebugMessenger() {
    if (!enableValidationLayers)
      return;

    pfnVkCreateDebugUtilsMessengerEXT =
        reinterpret_cast<PFN_vkCreateDebugUtilsMessengerEXT>(
            instance_.getProcAddr("vkCreateDebugUtilsMessengerEXT"));
    if (!pfnVkCreateDebugUtilsMessengerEXT) {
      std::cout << "GetInstanceProcAddr: Unable to find "
                   "pfnVkCreateDebugUtilsMessengerEXT function."
                << std::endl;
      exit(1);
    }

    pfnVkDestroyDebugUtilsMessengerEXT =
        reinterpret_cast<PFN_vkDestroyDebugUtilsMessengerEXT>(
            instance_.getProcAddr("vkDestroyDebugUtilsMessengerEXT"));
    if (!pfnVkDestroyDebugUtilsMessengerEXT) {
      std::cout << "GetInstanceProcAddr: Unable to find "
                   "pfnVkDestroyDebugUtilsMessengerEXT function."
                << std::endl;
      exit(1);
    }

    vk::DebugUtilsMessengerCreateInfoEXT createInfo{};
    populateDebugMessengerCreateInfo(createInfo);

    debugMessenger_ = instance_.createDebugUtilsMessengerEXT(createInfo);
  }

  void createSurface() {
    VkSurfaceKHR _surface;
    if (glfwCreateWindowSurface(instance_, window_, nullptr, &_surface) !=
        VK_SUCCESS) {
      throw std::runtime_error("failed to create window surface!");
    }
    surface_ = vk::SurfaceKHR(_surface);
  }

  void pickPhysicalDevice() {
    std::vector<vk::PhysicalDevice> physicalDevices =
        instance_.enumeratePhysicalDevices();
    // for (const auto& physicalDevice : physicalDevices) {
    //   printDeviceProperties(physicalDevice);
    // }

    for (const auto& physicalDevice : physicalDevices) {
      if (isDeviceSuitable(physicalDevice)) {
        physicalDevice_ = physicalDevice;
        // break;
      }
    }

    if (!physicalDevice_) {
      throw std::runtime_error("failed to find a suitable GPU!");
    }
  }

  void printDeviceProperties(const vk::PhysicalDevice& device) {
    // Получаем свойства устройства с помощью метода getProperties
    vk::PhysicalDeviceProperties deviceProperties = device.getProperties();
    std::cout << "Device Name: " << deviceProperties.deviceName << std::endl;
    std::cout << "Device Type: ";
    switch (deviceProperties.deviceType) {
      case vk::PhysicalDeviceType::eIntegratedGpu:
        std::cout << "Integrated GPU" << std::endl;
        break;
      case vk::PhysicalDeviceType::eDiscreteGpu:
        std::cout << "Discrete GPU" << std::endl;
        break;
      case vk::PhysicalDeviceType::eVirtualGpu:
        std::cout << "Virtual GPU" << std::endl;
        break;
      case vk::PhysicalDeviceType::eCpu:
        std::cout << "CPU" << std::endl;
        break;
      default:
        std::cout << "Unknown Device Type" << std::endl;
        break;
    }
    std::cout << "API Version: "
              << VK_VERSION_MAJOR(deviceProperties.apiVersion) << "."
              << VK_VERSION_MINOR(deviceProperties.apiVersion) << "."
              << VK_VERSION_PATCH(deviceProperties.apiVersion) << std::endl;
    std::cout << "Driver Version: " << deviceProperties.driverVersion
              << std::endl;
    std::cout << "Vendor ID: " << deviceProperties.vendorID << std::endl;
    std::cout << "Device ID: " << deviceProperties.deviceID << std::endl;
  }

  SwapChainSupportDetails querySwapChainSupport(
      const vk::PhysicalDevice& device) {
    SwapChainSupportDetails details;

    details.capabilities = device.getSurfaceCapabilitiesKHR(surface_);
    details.formats = device.getSurfaceFormatsKHR(surface_);
    details.presentModes = device.getSurfacePresentModesKHR(surface_);
    // details.formats[0].colorSpace
    // details.formats[0].formats

    return details;
  }

  bool isDeviceSuitable(const vk::PhysicalDevice& device) {
    QueueFamilyIndices indices = findQueueFamilies(device);

    bool extensionsSupported = checkDeviceExtensionSupport(device);

    bool swapChainAdequate = false;
    if (extensionsSupported) {
      SwapChainSupportDetails swapChainSupport = querySwapChainSupport(device);
      swapChainAdequate = !swapChainSupport.formats.empty() &&
                          !swapChainSupport.presentModes.empty();
    }

    return indices.isComplete() && extensionsSupported && swapChainAdequate;
  }

  bool checkDeviceExtensionSupport(const vk::PhysicalDevice& device) {
    std::vector<vk::ExtensionProperties> availableExtensions =
        device.enumerateDeviceExtensionProperties();

    std::set<std::string> requiredExtensions(deviceExtensions.begin(),
                                             deviceExtensions.end());
    // [&]() {
    //   std::cout << "Required Device Extensions:" << std::endl;
    //   for (const auto& ext : requiredExtensions) {
    //     std::cout << "\t" << ext << std::endl;
    //   }
    // }();

    for (const auto& extension : availableExtensions) {
      requiredExtensions.erase(extension.extensionName);
    }

    return requiredExtensions.empty();
  }

  void createLogicalDevice() {
    QueueFamilyIndices indices = findQueueFamilies(physicalDevice_);

    std::vector<vk::DeviceQueueCreateInfo> queueCreateInfos;
    std::set<uint32_t> uniqueQueueFamilies = {indices.graphicsFamily.value(),
                                              indices.presentFamily.value()};

    float queuePriority = 1.0f;
    for (uint32_t queueFamily : uniqueQueueFamilies) {
      vk::DeviceQueueCreateInfo queueCreateInfo{};
      queueCreateInfo.queueFamilyIndex = queueFamily;
      queueCreateInfo.queueCount = 1;
      queueCreateInfo.pQueuePriorities = &queuePriority;
      queueCreateInfos.push_back(queueCreateInfo);
    }

    vk::PhysicalDeviceFeatures deviceFeatures{};

    vk::DeviceCreateInfo createInfo{};
    createInfo.queueCreateInfoCount = queueCreateInfos.size();
    createInfo.pQueueCreateInfos = queueCreateInfos.data();
    createInfo.pEnabledFeatures = &deviceFeatures;
    createInfo.enabledExtensionCount = deviceExtensions.size();
    createInfo.ppEnabledExtensionNames = deviceExtensions.data();
    if (enableValidationLayers) {
      createInfo.enabledLayerCount = validationLayers.size();
      createInfo.ppEnabledLayerNames = validationLayers.data();
    } else {
      createInfo.enabledLayerCount = 0;
    }

    device_ = physicalDevice_.createDevice(createInfo);
    queue_ = device_.getQueue(indices.graphicsFamily.value(), 0);

    // [&]() {
    // std::cout << "Enabled Extension Count: "
    //           << createInfo.enabledExtensionCount << std::endl;
    // std::cout << "Enabled Extensions:" << std::endl;
    // for (uint32_t i = 0; i < createInfo.enabledExtensionCount; ++i) {
    //   std::cout << "\t" << createInfo.ppEnabledExtensionNames[i] << std::endl;
    // }

    // std::cout << "Enabled Layer Count: " << createInfo.enabledLayerCount
    //           << std::endl;
    // std::cout << "Enabled Layers:" << std::endl;
    // for (uint32_t i = 0; i < createInfo.enabledLayerCount; ++i) {
    //   std::cout << "\t" << createInfo.ppEnabledLayerNames[i] << std::endl;
    // }

    // std::cout << "Queue Create Info Count: "
    //           << createInfo.queueCreateInfoCount << std::endl;
    // for (uint32_t i = 0; i < createInfo.queueCreateInfoCount; ++i) {
    //   std::cout << "Queue Create Info " << i << ":" << std::endl;
    //   const auto& queueInfo = createInfo.pQueueCreateInfos[i];
    //   std::cout << "\tQueue Family Index: " << queueInfo.queueFamilyIndex
    //             << std::endl;
    //   std::cout << "\tQueue Count: " << queueInfo.queueCount << std::endl;
    //   std::cout << "\tQueue Priorities: ";
    //   for (uint32_t j = 0; j < queueInfo.queueCount; ++j) {
    //     std::cout << queueInfo.pQueuePriorities[j] << " ";
    //   }
    //   std::cout << std::endl;
    // }

    // std::cout << "Enabled Features:" << std::endl;
    // std::cout << "\tGeometry Shader: "
    //           << (deviceFeatures.geometryShader ? "Enabled" : "Disabled")
    //           << std::endl;
    // std::cout << "\tTessellation Shader: "
    //           << (deviceFeatures.tessellationShader ? "Enabled" : "Disabled")
    //           << std::endl;
    // // You can check another features
    // }();
  }

  void createCommandPool() {
    QueueFamilyIndices queueFamilyIndices = findQueueFamilies(physicalDevice_);

    vk::CommandPoolCreateInfo poolInfo{};
    poolInfo.flags = vk::CommandPoolCreateFlagBits::eResetCommandBuffer;
    poolInfo.queueFamilyIndex = queueFamilyIndices.graphicsFamily.value();

    commandPool_ = device_.createCommandPool(poolInfo);
  }

  void drawFrame() {
    // device_.waitIdle();

    if (framebufferResized_) {
      recreateSwapChain();
      framebufferResized_ = false;
    }

    // Show FPS
    double currentTime = glfwGetTime();
    double deltaTime = currentTime - previousTime_;
    ++frameCount_;
    if (deltaTime >= 1.0) {
      double fps = double(frameCount_) / deltaTime;
      std::string title = "FPS: " + std::to_string(int(fps));
      glfwSetWindowTitle(window_, title.c_str());
      frameCount_ = 0;
      previousTime_ = currentTime;
    }
  }

  QueueFamilyIndices findQueueFamilies(const vk::PhysicalDevice& device) {
    QueueFamilyIndices indices;

    std::vector<vk::QueueFamilyProperties> queueFamilies =
        device.getQueueFamilyProperties();

    int i = 0;
    for (const auto& queueFamily : queueFamilies) {
      if (queueFamily.queueFlags & vk::QueueFlagBits::eGraphics) {
        indices.graphicsFamily = i;
      }

      vk::Bool32 presentSupport = device.getSurfaceSupportKHR(i, surface_);
      if (presentSupport) {
        indices.presentFamily = i;
      }

      if (indices.isComplete()) {
        break;
      }

      i++;
    }

    return indices;
  }

  std::vector<const char*> getRequiredExtensions() {
    uint32_t glfwExtensionCount = 0;
    const char** glfwExtensions;
    glfwExtensions = glfwGetRequiredInstanceExtensions(&glfwExtensionCount);

    std::vector<const char*> extensions(glfwExtensions,
                                        glfwExtensions + glfwExtensionCount);

    if (enableValidationLayers) {
      extensions.push_back(VK_EXT_DEBUG_UTILS_EXTENSION_NAME);
    }

    return extensions;
  }

  bool checkValidationLayerSupport() {
    std::vector<vk::LayerProperties> availableLayers =
        vk::enumerateInstanceLayerProperties();

    for (const char* layerName : validationLayers) {
      bool layerFound = false;

      for (const auto& layerProperties : availableLayers) {
        if (strcmp(layerName, layerProperties.layerName) == 0) {
          layerFound = true;
          break;
        }
      }

      if (!layerFound) {
        return false;
      }
    }

    return true;
  }

  static VKAPI_ATTR VkBool32 VKAPI_CALL
  debugCallback(VkDebugUtilsMessageSeverityFlagBitsEXT messageSeverity,
                VkDebugUtilsMessageTypeFlagsEXT messageType,
                const VkDebugUtilsMessengerCallbackDataEXT* pCallbackData,
                void* pUserData) {
    std::cerr << "validation layer: " << pCallbackData->pMessage << std::endl;

    return VK_FALSE;
  }
};

int main() {
  ContextApplication app{};

  try {
    app.run();
  } catch (const std::exception& e) {
    std::cerr << e.what() << std::endl;
    return EXIT_FAILURE;
  }

  return EXIT_SUCCESS;
}
