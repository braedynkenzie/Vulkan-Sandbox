#include "SandboxApp.hpp"

#include <stdexcept>

namespace VulkanSandbox {

	SandboxApp::SandboxApp()
	{
		createPipelineLayout();
		createPipeline();
		createCommandBuffers();
	}

	SandboxApp::~SandboxApp()
	{
		vkDestroyPipelineLayout(vulkanDevice.device(), pipelineLayout, nullptr);
	}

	void SandboxApp::run()
	{
		while (!appWindow.shouldClose()) {
			glfwPollEvents();
		}
	}

	void SandboxApp::createPipelineLayout()
	{
		VkPipelineLayoutCreateInfo pipelineLayoutInfo{};
		pipelineLayoutInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
		pipelineLayoutInfo.pSetLayouts = nullptr;
		pipelineLayoutInfo.pushConstantRangeCount = 0;
		pipelineLayoutInfo.pPushConstantRanges = nullptr;
		if (vkCreatePipelineLayout(vulkanDevice.device(), &pipelineLayoutInfo, nullptr, &pipelineLayout) != VK_SUCCESS)
			throw std::runtime_error("Failed to create pipeline layout!");
	}

	void SandboxApp::createPipeline()
	{
		auto pipelineConfig = VulkanPipeline::getDefaultPipelineConfigInfo(vulkanSwapChain.width(), vulkanSwapChain.height());
		pipelineConfig.renderPass = vulkanSwapChain.getRenderPass();
		pipelineConfig.pipelineLayout = pipelineLayout;

		vulkanPipeline = std::make_unique<VulkanPipeline>(
			vulkanDevice,
			"src/shaders/VertexShader.vert.spv",
			"src/shaders/FragmentShader.frag.spv",
			pipelineConfig);
	}

	void SandboxApp::createCommandBuffers()
	{
		// TODO
	}

	void SandboxApp::drawFrame()
	{
		// TODO
	}

}
