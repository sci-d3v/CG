#define GLFW_INCLUDE_NONE
#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>

#include <vulkan/vulkan.hpp>

#include <cstdlib>
#include <fstream>
#include <iostream>
#include <limits>
#include <optional>
#include <set>
#include <stdexcept>
#include <vector>


#define NDEBUG

const uint32_t WIDTH = 800;
const uint32_t HEIGHT = 600;

const int MAX_FRAMES_IN_FLIGHT = 2;

const std::vector<const char *> validationLayers = {
    "VK_LAYER_KHRONOS_validation"};

const std::vector<const char *> deviceExtensions = {
    VK_KHR_SWAPCHAIN_EXTENSION_NAME};

#ifdef NDEBUG
const bool enableValidationLayers = false;
#else
const bool enableValidationLayers = true;
#endif

PFN_vkCreateDebugUtilsMessengerEXT pfnVkCreateDebugUtilsMessengerEXT;
PFN_vkDestroyDebugUtilsMessengerEXT pfnVkDestroyDebugUtilsMessengerEXT;

VKAPI_ATTR VkResult VKAPI_CALL vkCreateDebugUtilsMessengerEXT(
    VkInstance instance, const VkDebugUtilsMessengerCreateInfoEXT *pCreateInfo,
    const VkAllocationCallbacks *pAllocator,
    VkDebugUtilsMessengerEXT *pMessenger) {
  return pfnVkCreateDebugUtilsMessengerEXT(instance, pCreateInfo, pAllocator,
                                           pMessenger);
}

VKAPI_ATTR void VKAPI_CALL vkDestroyDebugUtilsMessengerEXT(
    VkInstance instance, VkDebugUtilsMessengerEXT messenger,
    VkAllocationCallbacks const *pAllocator) {
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

class HelloTriangleApplication {
public:
  void run() {
    initWindow();
    initVulkan();
    mainLoop();
    cleanup();
  }

private:
  GLFWwindow *window = nullptr;

  vk::Instance instance;
  vk::DebugUtilsMessengerEXT debugMessenger;
  vk::SurfaceKHR surface;

  vk::PhysicalDevice physicalDevice;
  vk::Device device;

  vk::Queue graphicsQueue;
  vk::Queue presentQueue;

  vk::SwapchainKHR swapChain;
  std::vector<vk::Image> swapChainImages;
  vk::Format swapChainImageFormat;
  vk::Extent2D swapChainExtent;
  std::vector<vk::ImageView> swapChainImageViews;
  std::vector<vk::Framebuffer> swapChainFramebuffers;

  vk::RenderPass renderPass;
  vk::PipelineLayout pipelineLayout;
  vk::Pipeline graphicsPipeline;

  vk::CommandPool commandPool;
  std::vector<vk::CommandBuffer> commandBuffers;

  std::vector<vk::Semaphore> imageAvailableSemaphores;
  std::vector<vk::Semaphore> renderFinishedSemaphores;
  std::vector<vk::Fence> inFlightFences;
  uint32_t currentFrame = 0;

  bool framebufferResized = false;

  void initWindow() {
    glfwInit();

    glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);

    window = glfwCreateWindow(WIDTH, HEIGHT, "vulkan-hpp", nullptr, nullptr);
    glfwSetWindowUserPointer(window, this);
    glfwSetFramebufferSizeCallback(window, framebufferResizeCallback);
  }

  static void framebufferResizeCallback(GLFWwindow *window, int width,
                                        int height) {
    auto app = reinterpret_cast<HelloTriangleApplication *>(
        glfwGetWindowUserPointer(window));
    app->framebufferResized = true;
  }

  void initVulkan() {
    createInstance();
    setupDebugMessenger();
    createSurface();
    pickPhysicalDevice();
    createLogicalDevice();
    createSwapChain();
    createImageViews();
    createRenderPass();
    createGraphicsPipeline();
    createFramebuffers();
    createCommandPool();
    createCommandBuffers();
    createSyncObjects();
  }

  void mainLoop() {
    while (!glfwWindowShouldClose(window)) {
      glfwPollEvents();
      drawFrame();
    }

    device.waitIdle();
  }

  void cleanupSwapChain() {
    for (auto &swapChainFramebuffer : swapChainFramebuffers) {
      device.destroy(swapChainFramebuffer);
    }

    for (auto &swapChainImageView : swapChainImageViews) {
      device.destroy(swapChainImageView);
    }

    device.destroy(swapChain);
  }

  void cleanup() {
    cleanupSwapChain();

    for (size_t i = 0; i < MAX_FRAMES_IN_FLIGHT; i++) {
      device.destroy(imageAvailableSemaphores[i]);
      device.destroy(renderFinishedSemaphores[i]);
      device.destroy(inFlightFences[i]);
    }

    device.destroy(commandPool);

    device.destroy(graphicsPipeline);
    device.destroy(pipelineLayout);
    device.destroy(renderPass);

    device.destroy();

    if (enableValidationLayers) {
      instance.destroyDebugUtilsMessengerEXT(debugMessenger);
    }

    instance.destroy(surface);
    instance.destroy();

    glfwDestroyWindow(window);

    glfwTerminate();
  }

  void recreateSwapChain() {
    int width = 0, height = 0;
    glfwGetFramebufferSize(window, &width, &height);
    while (width == 0 || height == 0) {
      glfwGetFramebufferSize(window, &width, &height);
      glfwWaitEvents();
    }

    device.waitIdle();

    cleanupSwapChain();

    createSwapChain();
    createImageViews();
    createFramebuffers();
  }

  void createInstance() {
    if (enableValidationLayers && !checkValidationLayerSupport()) {
      throw std::runtime_error(
          "validation layers requested, but not available!");
    }

    vk::ApplicationInfo appInfo{};
    appInfo.pApplicationName = "Hello Triangle";
    appInfo.applicationVersion = VK_MAKE_VERSION(1, 0, 0);
    appInfo.pEngineName = "No Engine";
    appInfo.engineVersion = VK_MAKE_VERSION(1, 0, 0);
    appInfo.apiVersion = VK_API_VERSION_1_0;

    vk::InstanceCreateInfo createInfo{};
    createInfo.pApplicationInfo = &appInfo;
    auto extensions = getRequiredExtensions();
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

    instance = vk::createInstance(createInfo);
  }

  void populateDebugMessengerCreateInfo(
      vk::DebugUtilsMessengerCreateInfoEXT &createInfo) {
    createInfo.messageSeverity =
        vk::DebugUtilsMessageSeverityFlagBitsEXT::eVerbose |
        vk::DebugUtilsMessageSeverityFlagBitsEXT::eWarning |
        vk::DebugUtilsMessageSeverityFlagBitsEXT::eError;
    createInfo.messageType = vk::DebugUtilsMessageTypeFlagBitsEXT::eGeneral |
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
            instance.getProcAddr("vkCreateDebugUtilsMessengerEXT"));
    if (!pfnVkCreateDebugUtilsMessengerEXT) {
      std::cout << "GetInstanceProcAddr: Unable to find "
                   "pfnVkCreateDebugUtilsMessengerEXT function."
                << std::endl;
      exit(1);
    }

    pfnVkDestroyDebugUtilsMessengerEXT =
        reinterpret_cast<PFN_vkDestroyDebugUtilsMessengerEXT>(
            instance.getProcAddr("vkDestroyDebugUtilsMessengerEXT"));
    if (!pfnVkDestroyDebugUtilsMessengerEXT) {
      std::cout << "GetInstanceProcAddr: Unable to find "
                   "pfnVkDestroyDebugUtilsMessengerEXT function."
                << std::endl;
      exit(1);
    }

    vk::DebugUtilsMessengerCreateInfoEXT createInfo{};
    populateDebugMessengerCreateInfo(createInfo);

    debugMessenger = instance.createDebugUtilsMessengerEXT(createInfo);
  }

  void createSurface() {
    VkSurfaceKHR _surface;
    if (glfwCreateWindowSurface(instance, window, nullptr, &_surface) !=
        VK_SUCCESS) {
      throw std::runtime_error("failed to create window surface!");
    }
    surface = vk::SurfaceKHR(_surface);
  }

  void pickPhysicalDevice() {
    std::vector<vk::PhysicalDevice> devices =
        instance.enumeratePhysicalDevices();

    for (const auto &device : devices) {
      if (isDeviceSuitable(device)) {
        physicalDevice = device;
        break;
      }
    }

    if (!physicalDevice) {
      throw std::runtime_error("failed to find a suitable GPU!");
    }
  }

  vk::SurfaceFormatKHR chooseSwapSurfaceFormat(
      const std::vector<vk::SurfaceFormatKHR> &availableFormats) {
    for (const auto &availableFormat : availableFormats) {
      if (availableFormat.format == vk::Format::eB8G8R8A8Srgb &&
          availableFormat.colorSpace == vk::ColorSpaceKHR::eSrgbNonlinear) {
        return availableFormat;
      }
    }

    return availableFormats[0];
  }

  vk::PresentModeKHR chooseSwapPresentMode(
      const std::vector<vk::PresentModeKHR> &availablePresentModes) {
    for (const auto &availablePresentMode : availablePresentModes) {
      if (availablePresentMode == vk::PresentModeKHR::eMailbox) {
        return availablePresentMode;
      }
    }

    return vk::PresentModeKHR::eFifo;
  }

  vk::Extent2D
  chooseSwapExtent(const vk::SurfaceCapabilitiesKHR &capabilities) {
    if (capabilities.currentExtent.width !=
        std::numeric_limits<uint32_t>::max()) {
      return capabilities.currentExtent;
    } else {
      int width, height;
      glfwGetFramebufferSize(window, &width, &height);

      vk::Extent2D actualExtent = {static_cast<uint32_t>(width),
                                   static_cast<uint32_t>(height)};
      actualExtent.width =
          std::clamp(actualExtent.width, capabilities.minImageExtent.width,
                     capabilities.maxImageExtent.width);
      actualExtent.height =
          std::clamp(actualExtent.height, capabilities.minImageExtent.height,
                     capabilities.maxImageExtent.height);

      return actualExtent;
    }
  }

  SwapChainSupportDetails
  querySwapChainSupport(const vk::PhysicalDevice &device) {
    SwapChainSupportDetails details;

    details.capabilities = device.getSurfaceCapabilitiesKHR(surface);
    details.formats = device.getSurfaceFormatsKHR(surface);
    details.presentModes = device.getSurfacePresentModesKHR(surface);

    return details;
  }

  bool isDeviceSuitable(const vk::PhysicalDevice &device) {
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

  bool checkDeviceExtensionSupport(const vk::PhysicalDevice &device) {
    std::vector<vk::ExtensionProperties> availableExtensions =
        device.enumerateDeviceExtensionProperties();

    std::set<std::string> requiredExtensions(deviceExtensions.begin(),
                                             deviceExtensions.end());
    for (const auto &extension : availableExtensions) {
      requiredExtensions.erase(extension.extensionName);
    }

    return requiredExtensions.empty();
  }

  void createLogicalDevice() {
    QueueFamilyIndices indices = findQueueFamilies(physicalDevice);

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

    device = physicalDevice.createDevice(createInfo);
    graphicsQueue = device.getQueue(indices.graphicsFamily.value(), 0);
    presentQueue = device.getQueue(indices.presentFamily.value(), 0);
  }

  void createSwapChain() {
    SwapChainSupportDetails swapChainSupport =
        querySwapChainSupport(physicalDevice);

    vk::SurfaceFormatKHR surfaceFormat =
        chooseSwapSurfaceFormat(swapChainSupport.formats);
    vk::PresentModeKHR presentMode =
        chooseSwapPresentMode(swapChainSupport.presentModes);
    vk::Extent2D extent = chooseSwapExtent(swapChainSupport.capabilities);

    uint32_t imageCount = swapChainSupport.capabilities.minImageCount + 1;
    if (swapChainSupport.capabilities.maxImageCount > 0 &&
        imageCount > swapChainSupport.capabilities.maxImageCount) {
      imageCount = swapChainSupport.capabilities.maxImageCount;
    }

    vk::SwapchainCreateInfoKHR createInfo{};
    createInfo.surface = surface;
    createInfo.minImageCount = imageCount;
    createInfo.imageFormat = surfaceFormat.format;
    createInfo.imageColorSpace = surfaceFormat.colorSpace;
    createInfo.imageExtent = extent;
    createInfo.imageArrayLayers = 1;
    createInfo.imageUsage = vk::ImageUsageFlagBits::eColorAttachment;

    QueueFamilyIndices indices = findQueueFamilies(physicalDevice);
    uint32_t queueFamilyIndices[] = {indices.graphicsFamily.value(),
                                     indices.presentFamily.value()};

    if (indices.graphicsFamily != indices.presentFamily) {
      createInfo.imageSharingMode = vk::SharingMode::eConcurrent;
      createInfo.queueFamilyIndexCount = 2;
      createInfo.pQueueFamilyIndices = queueFamilyIndices;
    } else {
      createInfo.imageSharingMode = vk::SharingMode::eExclusive;
      createInfo.queueFamilyIndexCount = 0;
      createInfo.pQueueFamilyIndices = nullptr;
    }

    createInfo.preTransform = swapChainSupport.capabilities.currentTransform;
    createInfo.compositeAlpha = vk::CompositeAlphaFlagBitsKHR::eOpaque;
    createInfo.presentMode = presentMode;
    createInfo.clipped = VK_TRUE;

    createInfo.oldSwapchain = VK_NULL_HANDLE;

    swapChain = device.createSwapchainKHR(createInfo);
    swapChainImages = device.getSwapchainImagesKHR(swapChain);
    swapChainImageFormat = surfaceFormat.format;
    swapChainExtent = extent;
  }

  void createImageViews() {
    swapChainImageViews.resize(swapChainImages.size());

    for (int i = 0; i < swapChainImages.size(); ++i) {
      vk::ImageViewCreateInfo createInfo{};
      createInfo.image = swapChainImages[i];
      createInfo.viewType = vk::ImageViewType::e2D;
      createInfo.format = swapChainImageFormat;
      createInfo.components.r = vk::ComponentSwizzle::eIdentity;
      createInfo.components.g = vk::ComponentSwizzle::eIdentity;
      createInfo.components.b = vk::ComponentSwizzle::eIdentity;
      createInfo.components.a = vk::ComponentSwizzle::eIdentity;
      createInfo.subresourceRange.aspectMask = vk::ImageAspectFlagBits::eColor;
      createInfo.subresourceRange.baseMipLevel = 0;
      createInfo.subresourceRange.levelCount = 1;
      createInfo.subresourceRange.baseArrayLayer = 0;
      createInfo.subresourceRange.layerCount = 1;

      swapChainImageViews[i] = device.createImageView(createInfo);
    }
  }

  void createRenderPass() {
    vk::AttachmentDescription colorAttachment{};
    colorAttachment.format = swapChainImageFormat;
    colorAttachment.samples = vk::SampleCountFlagBits::e1;
    colorAttachment.loadOp = vk::AttachmentLoadOp::eClear;
    colorAttachment.storeOp = vk::AttachmentStoreOp::eStore;
    colorAttachment.stencilLoadOp = vk::AttachmentLoadOp::eDontCare;
    colorAttachment.stencilStoreOp = vk::AttachmentStoreOp::eDontCare;
    colorAttachment.initialLayout = vk::ImageLayout::eUndefined;
    colorAttachment.finalLayout = vk::ImageLayout::ePresentSrcKHR;

    vk::AttachmentReference colorAttachmentRef{};
    colorAttachmentRef.attachment = 0;
    colorAttachmentRef.layout = vk::ImageLayout::eColorAttachmentOptimal;

    vk::SubpassDescription subpass{};
    subpass.pipelineBindPoint = vk::PipelineBindPoint::eGraphics;
    subpass.colorAttachmentCount = 1;
    subpass.pColorAttachments = &colorAttachmentRef;

    vk::SubpassDependency dependency{};
    dependency.srcSubpass = VK_SUBPASS_EXTERNAL;
    dependency.dstSubpass = 0;
    dependency.srcStageMask = vk::PipelineStageFlagBits::eColorAttachmentOutput;
    dependency.srcAccessMask = vk::AccessFlagBits::eNone;
    dependency.dstStageMask = vk::PipelineStageFlagBits::eColorAttachmentOutput;
    dependency.dstAccessMask = vk::AccessFlagBits::eColorAttachmentWrite;

    vk::RenderPassCreateInfo renderPassInfo{};
    renderPassInfo.attachmentCount = 1;
    renderPassInfo.pAttachments = &colorAttachment;
    renderPassInfo.subpassCount = 1;
    renderPassInfo.pSubpasses = &subpass;
    renderPassInfo.dependencyCount = 1;
    renderPassInfo.pDependencies = &dependency;

    renderPass = device.createRenderPass(renderPassInfo);
  }

  void createGraphicsPipeline() {
    auto vertShaderCode = readFile("shaders/shader_base.vert.spv");
    auto fragShaderCode = readFile("shaders/shader_base.frag.spv");

    vk::ShaderModule vertShaderModule = createShaderModule(vertShaderCode);
    vk::ShaderModule fragShaderModule = createShaderModule(fragShaderCode);

    vk::PipelineShaderStageCreateInfo vertShaderStageInfo{};
    vertShaderStageInfo.stage = vk::ShaderStageFlagBits::eVertex;
    vertShaderStageInfo.module = vertShaderModule;
    vertShaderStageInfo.pName = "main";

    vk::PipelineShaderStageCreateInfo fragShaderStageInfo{};
    fragShaderStageInfo.stage = vk::ShaderStageFlagBits::eFragment;
    fragShaderStageInfo.module = fragShaderModule;
    fragShaderStageInfo.pName = "main";

    vk::PipelineShaderStageCreateInfo shaderStages[] = {vertShaderStageInfo,
                                                        fragShaderStageInfo};

    // Dynamic state
    std::vector<vk::DynamicState> dynamicStates = {
        vk::DynamicState::eViewport,
        vk::DynamicState::eScissor,
    };

    vk::PipelineDynamicStateCreateInfo dynamicState{};
    dynamicState.dynamicStateCount = dynamicStates.size();
    dynamicState.pDynamicStates = dynamicStates.data();

    // Vertex input
    vk::PipelineVertexInputStateCreateInfo vertexInputInfo{};
    vertexInputInfo.vertexBindingDescriptionCount = 0;
    vertexInputInfo.pVertexBindingDescriptions = nullptr;
    vertexInputInfo.vertexAttributeDescriptionCount = 0;
    vertexInputInfo.pVertexAttributeDescriptions = nullptr;

    // Input assembly
    vk::PipelineInputAssemblyStateCreateInfo inputAssembly{};
    inputAssembly.topology = vk::PrimitiveTopology::eTriangleList;
    inputAssembly.primitiveRestartEnable = VK_FALSE;

    // Viewports and scissors
    vk::PipelineViewportStateCreateInfo viewportState{};
    viewportState.viewportCount = 1;
    viewportState.scissorCount = 1;

    // Rasterizer
    vk::PipelineRasterizationStateCreateInfo rasterizer{};
    rasterizer.depthClampEnable = VK_FALSE;
    rasterizer.rasterizerDiscardEnable = VK_FALSE;
    rasterizer.polygonMode = vk::PolygonMode::eFill;
    rasterizer.lineWidth = 1.0f;
    rasterizer.cullMode = vk::CullModeFlagBits::eBack;
    rasterizer.frontFace = vk::FrontFace::eClockwise;
    rasterizer.depthBiasEnable = VK_FALSE;
    rasterizer.depthBiasConstantFactor = 0.0f;
    rasterizer.depthBiasClamp = 0.0f;
    rasterizer.depthBiasSlopeFactor = 0.0f;

    // Multisampling
    vk::PipelineMultisampleStateCreateInfo multisampling{};
    multisampling.sampleShadingEnable = VK_FALSE;
    multisampling.rasterizationSamples = vk::SampleCountFlagBits::e1;
    multisampling.minSampleShading = 1.0f;
    multisampling.pSampleMask = nullptr;
    multisampling.alphaToCoverageEnable = VK_FALSE;
    multisampling.alphaToOneEnable = VK_FALSE;

    // Color blending
    vk::PipelineColorBlendAttachmentState colorBlendAttachment{};
    colorBlendAttachment.colorWriteMask =
        vk::ColorComponentFlagBits::eR | vk::ColorComponentFlagBits::eG |
        vk::ColorComponentFlagBits::eB | vk::ColorComponentFlagBits::eA;
    colorBlendAttachment.blendEnable = VK_FALSE;
    colorBlendAttachment.srcColorBlendFactor = vk::BlendFactor::eOne;
    colorBlendAttachment.dstColorBlendFactor = vk::BlendFactor::eZero;
    colorBlendAttachment.colorBlendOp = vk::BlendOp::eAdd;
    colorBlendAttachment.srcAlphaBlendFactor = vk::BlendFactor::eOne;
    colorBlendAttachment.dstAlphaBlendFactor = vk::BlendFactor::eZero;
    colorBlendAttachment.alphaBlendOp = vk::BlendOp::eAdd;

    vk::PipelineColorBlendStateCreateInfo colorBlending{};
    colorBlending.logicOpEnable = VK_FALSE;
    colorBlending.logicOp = vk::LogicOp::eCopy;
    colorBlending.attachmentCount = 1;
    colorBlending.pAttachments = &colorBlendAttachment;
    colorBlending.blendConstants[0] = 0.0f;
    colorBlending.blendConstants[1] = 0.0f;
    colorBlending.blendConstants[2] = 0.0f;
    colorBlending.blendConstants[3] = 0.0f;

    // Pipeline layout
    vk::PipelineLayoutCreateInfo pipelineLayoutInfo{};
    pipelineLayoutInfo.setLayoutCount = 0;
    pipelineLayoutInfo.pSetLayouts = nullptr;
    pipelineLayoutInfo.pushConstantRangeCount = 0;
    pipelineLayoutInfo.pPushConstantRanges = nullptr;
    pipelineLayout = device.createPipelineLayout(pipelineLayoutInfo);

    vk::GraphicsPipelineCreateInfo pipelineInfo{};
    pipelineInfo.stageCount = 2;
    pipelineInfo.pStages = shaderStages;
    pipelineInfo.pVertexInputState = &vertexInputInfo;
    pipelineInfo.pInputAssemblyState = &inputAssembly;
    pipelineInfo.pViewportState = &viewportState;
    pipelineInfo.pRasterizationState = &rasterizer;
    pipelineInfo.pMultisampleState = &multisampling;
    pipelineInfo.pDepthStencilState = nullptr;
    pipelineInfo.pColorBlendState = &colorBlending;
    pipelineInfo.pDynamicState = &dynamicState;
    pipelineInfo.layout = pipelineLayout;
    pipelineInfo.renderPass = renderPass;
    pipelineInfo.subpass = 0;
    pipelineInfo.basePipelineHandle = VK_NULL_HANDLE;
    pipelineInfo.basePipelineIndex = -1;

    vk::ResultValue<vk::Pipeline> rvPipeline =
        device.createGraphicsPipeline(VK_NULL_HANDLE, pipelineInfo);
    switch (rvPipeline.result) {
    case vk::Result::eSuccess:
      graphicsPipeline = std::move(rvPipeline.value);
      break;
    default:
      assert(false); // should never happen
    }

    device.destroy(vertShaderModule);
    device.destroy(fragShaderModule);
  }

  void createFramebuffers() {
    swapChainFramebuffers.resize(swapChainImageViews.size());

    for (int i = 0; i < swapChainImageViews.size(); ++i) {
      vk::ImageView attachments[] = {swapChainImageViews[i]};

      vk::FramebufferCreateInfo framebufferInfo{};
      framebufferInfo.renderPass = renderPass;
      framebufferInfo.attachmentCount = 1;
      framebufferInfo.pAttachments = attachments;
      framebufferInfo.width = swapChainExtent.width;
      framebufferInfo.height = swapChainExtent.height;
      framebufferInfo.layers = 1;

      swapChainFramebuffers[i] = device.createFramebuffer(framebufferInfo);
    }
  }

  void createCommandPool() {
    QueueFamilyIndices queueFamilyIndices = findQueueFamilies(physicalDevice);

    vk::CommandPoolCreateInfo poolInfo{};
    poolInfo.flags = vk::CommandPoolCreateFlagBits::eResetCommandBuffer;
    poolInfo.queueFamilyIndex = queueFamilyIndices.graphicsFamily.value();

    commandPool = device.createCommandPool(poolInfo);
  }

  void createCommandBuffers() {
    commandBuffers.resize(MAX_FRAMES_IN_FLIGHT);

    vk::CommandBufferAllocateInfo allocInfo{};
    allocInfo.commandPool = commandPool;
    allocInfo.level = vk::CommandBufferLevel::ePrimary;
    allocInfo.commandBufferCount = commandBuffers.size();

    commandBuffers = device.allocateCommandBuffers(allocInfo);
  }

  void recordCommandBuffer(vk::CommandBuffer &commandBuffer,
                           uint32_t imageIndex) {
    vk::CommandBufferBeginInfo beginInfo{};
    beginInfo.flags = vk::CommandBufferUsageFlagBits::eOneTimeSubmit;
    beginInfo.pInheritanceInfo = nullptr;

    commandBuffer.begin(beginInfo);
    {
      vk::RenderPassBeginInfo renderPassInfo{};
      renderPassInfo.renderPass = renderPass;
      renderPassInfo.framebuffer = swapChainFramebuffers[imageIndex];
      renderPassInfo.renderArea =
          vk::Rect2D(vk::Offset2D(0, 0), swapChainExtent);

      vk::ClearValue clearColor = vk::ClearColorValue(0.0f, 0.0f, 0.0f, 1.0f);
      renderPassInfo.clearValueCount = 1;
      renderPassInfo.pClearValues = &clearColor;

      commandBuffer.beginRenderPass(renderPassInfo,
                                    vk::SubpassContents::eInline);
      {
        commandBuffer.bindPipeline(vk::PipelineBindPoint::eGraphics,
                                   graphicsPipeline);

        vk::Viewport viewport{};
        viewport.x = 0.0f;
        viewport.y = 0.0f;
        viewport.width = static_cast<float>(swapChainExtent.width);
        viewport.height = static_cast<float>(swapChainExtent.height);
        viewport.minDepth = 0.0f;
        viewport.maxDepth = 1.0f;
        commandBuffer.setViewport(0, 1, &viewport);

        vk::Rect2D scissor{};
        scissor.offset = vk::Offset2D(0, 0);
        scissor.extent = swapChainExtent;
        commandBuffer.setScissor(0, 1, &scissor);

        commandBuffer.draw(3, 1, 0, 0);
      }
      commandBuffer.endRenderPass();
    }
    commandBuffer.end();
  }

  void createSyncObjects() {
    imageAvailableSemaphores.resize(MAX_FRAMES_IN_FLIGHT);
    renderFinishedSemaphores.resize(MAX_FRAMES_IN_FLIGHT);
    inFlightFences.resize(MAX_FRAMES_IN_FLIGHT);

    vk::SemaphoreCreateInfo semaphoreInfo{};

    vk::FenceCreateInfo fenceInfo{};
    fenceInfo.flags = vk::FenceCreateFlagBits::eSignaled;

    for (size_t i = 0; i < MAX_FRAMES_IN_FLIGHT; i++) {
      imageAvailableSemaphores[i] = device.createSemaphore(semaphoreInfo);
      renderFinishedSemaphores[i] = device.createSemaphore(semaphoreInfo);
      inFlightFences[i] = device.createFence(fenceInfo);
    }
  }

  void drawFrame() {
    vk::Result result = device.waitForFences(1, &inFlightFences[currentFrame],
                                             VK_TRUE, UINT64_MAX);
    switch (result) {
    case vk::Result::eErrorOutOfDateKHR:
    case vk::Result::eSuboptimalKHR:
      recreateSwapChain();
      framebufferResized = false;
      break;
    case vk::Result::eSuccess:
      break;
    default:
      throw std::runtime_error("failed to present swap chain image!");
      break;
    }

    uint32_t imageIndex;
    vk::ResultValue<uint32_t> rvImageIndex = device.acquireNextImageKHR(
        swapChain, UINT64_MAX, imageAvailableSemaphores[currentFrame],
        VK_NULL_HANDLE);
    switch (rvImageIndex.result) {
    case vk::Result::eErrorOutOfDateKHR:
      recreateSwapChain();
      return;
    case vk::Result::eSuccess:
    case vk::Result::eSuboptimalKHR:
      imageIndex = rvImageIndex.value;
      break;
    default:
      throw std::runtime_error("failed to acquire swap chain image!");
      break;
    }
    result = device.resetFences(1, &inFlightFences[currentFrame]);
    switch (result) {
    case vk::Result::eErrorOutOfDateKHR:
    case vk::Result::eSuboptimalKHR:
      recreateSwapChain();
      framebufferResized = false;
      break;
    case vk::Result::eSuccess:
      break;
    default:
      throw std::runtime_error("failed to present swap chain image!");
      break;
    }

    commandBuffers[currentFrame].reset();
    recordCommandBuffer(commandBuffers[currentFrame], imageIndex);

    vk::Semaphore waitSemaphores[] = {imageAvailableSemaphores[currentFrame]};
    vk::Semaphore signalSemaphores[] = {renderFinishedSemaphores[currentFrame]};
    vk::PipelineStageFlags waitStages[] = {
        vk::PipelineStageFlagBits::eColorAttachmentOutput};
    vk::SwapchainKHR swapChains[] = {swapChain};

    vk::SubmitInfo submitInfo{};
    submitInfo.waitSemaphoreCount = 1;
    submitInfo.pWaitSemaphores = waitSemaphores;
    submitInfo.pWaitDstStageMask = waitStages;
    submitInfo.commandBufferCount = 1;
    submitInfo.pCommandBuffers = &commandBuffers[currentFrame];
    submitInfo.signalSemaphoreCount = 1;
    submitInfo.pSignalSemaphores = signalSemaphores;

    graphicsQueue.submit(submitInfo, inFlightFences[currentFrame]);

    vk::PresentInfoKHR presentInfo{};
    presentInfo.waitSemaphoreCount = 1;
    presentInfo.pWaitSemaphores = signalSemaphores;
    presentInfo.swapchainCount = 1;
    presentInfo.pSwapchains = swapChains;
    presentInfo.pImageIndices = &imageIndex;
    presentInfo.pResults = nullptr;

    result = presentQueue.presentKHR(&presentInfo);
    switch (result) {
    case vk::Result::eErrorOutOfDateKHR:
    case vk::Result::eSuboptimalKHR:
      recreateSwapChain();
      framebufferResized = false;
      break;
    case vk::Result::eSuccess:
      break;
    default:
      throw std::runtime_error("failed to present swap chain image!");
      break;
    }

    if (framebufferResized) {
      recreateSwapChain();
      framebufferResized = false;
    }

    currentFrame = (currentFrame + 1) % MAX_FRAMES_IN_FLIGHT;
  }

  vk::ShaderModule createShaderModule(const std::vector<char> &code) {
    vk::ShaderModuleCreateInfo createInfo{};
    createInfo.codeSize = code.size();
    createInfo.pCode = reinterpret_cast<const uint32_t *>(code.data());

    vk::ShaderModule shaderModule = device.createShaderModule(createInfo);
    return shaderModule;
  }

  QueueFamilyIndices findQueueFamilies(const vk::PhysicalDevice &device) {
    QueueFamilyIndices indices;

    std::vector<vk::QueueFamilyProperties> queueFamilies =
        device.getQueueFamilyProperties();

    int i = 0;
    for (const auto &queueFamily : queueFamilies) {
      if (queueFamily.queueFlags & vk::QueueFlagBits::eGraphics) {
        indices.graphicsFamily = i;
      }

      vk::Bool32 presentSupport = device.getSurfaceSupportKHR(i, surface);
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

  std::vector<const char *> getRequiredExtensions() {
    uint32_t glfwExtensionCount = 0;
    const char **glfwExtensions;
    glfwExtensions = glfwGetRequiredInstanceExtensions(&glfwExtensionCount);

    std::vector<const char *> extensions(glfwExtensions,
                                         glfwExtensions + glfwExtensionCount);

    if (enableValidationLayers) {
      extensions.push_back(VK_EXT_DEBUG_UTILS_EXTENSION_NAME);
    }

    return extensions;
  }

  bool checkValidationLayerSupport() {
    std::vector<vk::LayerProperties> availableLayers =
        vk::enumerateInstanceLayerProperties();

    for (const char *layerName : validationLayers) {
      bool layerFound = false;

      for (const auto &layerProperties : availableLayers) {
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

  static std::vector<char> readFile(const std::string &filename) {
    std::ifstream file(filename, std::ios::ate | std::ios::binary);

    if (!file.is_open()) {
      throw std::runtime_error("failed to open file!");
    }

    size_t fileSize = (size_t)file.tellg();
    std::vector<char> buffer(fileSize);

    file.seekg(0);
    file.read(buffer.data(), fileSize);

    file.close();

    return buffer;
  }

  static VKAPI_ATTR VkBool32 VKAPI_CALL
  debugCallback(VkDebugUtilsMessageSeverityFlagBitsEXT messageSeverity,
                VkDebugUtilsMessageTypeFlagsEXT messageType,
                const VkDebugUtilsMessengerCallbackDataEXT *pCallbackData,
                void *pUserData) {
    std::cerr << "validation layer: " << pCallbackData->pMessage << std::endl;

    return VK_FALSE;
  }
};

int main() {
  HelloTriangleApplication app{};

  try {
    app.run();
  } catch (const std::exception &e) {
    std::cerr << e.what() << std::endl;
    return EXIT_FAILURE;
  }

  return EXIT_SUCCESS;
}
