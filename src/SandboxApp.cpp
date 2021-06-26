#include "SandboxApp.hpp"

#include <stdexcept>
#include <array>

namespace VulkanSandbox {

	SandboxApp::SandboxApp()
	{
		loadSandboxModels();
		createPipelineLayout();
		recreateSwapChain();
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
			drawFrame();
		}

		vkDeviceWaitIdle(vulkanDevice.device());
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
		assert(vulkanSwapChain != nullptr && "Cannot create pipeline before pipeline layout!");
		assert(pipelineLayout != nullptr && "Cannot create pipeline before swap chain!");

		PipelineConfigInfo pipelineConfig{};
		VulkanPipeline::setupDefaultPipelineConfigInfo(pipelineConfig);
		pipelineConfig.renderPass = vulkanSwapChain->getRenderPass();
		pipelineConfig.pipelineLayout = pipelineLayout;

		vulkanPipeline = std::make_unique<VulkanPipeline>(
			vulkanDevice,
			"src/shaders/VertexShader.vert.spv",
			"src/shaders/FragmentShader.frag.spv",
			pipelineConfig);
	}

	void SandboxApp::createCommandBuffers()
	{
		commandBuffers.resize(vulkanSwapChain->imageCount());

		VkCommandBufferAllocateInfo allocInfo{};
		allocInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
		allocInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
		allocInfo.commandPool = vulkanDevice.getCommandPool();
		allocInfo.commandBufferCount = (uint32_t)commandBuffers.size();

		if (vkAllocateCommandBuffers(vulkanDevice.device(), &allocInfo, commandBuffers.data()) != VK_SUCCESS)
			throw std::runtime_error("Failed to allocate command buffers!");
	}

	void SandboxApp::freeCommandBuffers()
	{
		vkFreeCommandBuffers(
			vulkanDevice.device(), 
			vulkanDevice.getCommandPool(), 
			static_cast<uint32_t>(commandBuffers.size()), 
			commandBuffers.data());
		commandBuffers.clear();
	}

	void SandboxApp::drawFrame()
	{
		uint32_t imageIndex;
		auto result = vulkanSwapChain->acquireNextImage(&imageIndex);

		if (result == VK_ERROR_OUT_OF_DATE_KHR)
		{
			recreateSwapChain();
			return;
		}

		if (result != VK_SUCCESS && result != VK_SUBOPTIMAL_KHR)
			throw std::runtime_error("Failed to acquire next image index in the swap chain!");

		recordCommandBuffer(imageIndex);
		result = vulkanSwapChain->submitCommandBuffers(&commandBuffers[imageIndex], &imageIndex);
		if (result == VK_SUBOPTIMAL_KHR || result == VK_ERROR_OUT_OF_DATE_KHR || appWindow.wasResized())
		{
			appWindow.resetSizeChangedFlag();
			recreateSwapChain();
			return; 
		}

		if (result != VK_SUCCESS)
			throw std::runtime_error("Failed to submit command buffer!");
	}

	void SandboxApp::recreateSwapChain()
	{
		auto extent = appWindow.getExtent();
		while (extent.width == 0 || extent.height == 0)
		{
			extent = appWindow.getExtent();
			glfwWaitEvents();
		}

		vkDeviceWaitIdle(vulkanDevice.device());
		
		if (vulkanSwapChain == nullptr)
			vulkanSwapChain = std::make_unique<VulkanSwapChain>(vulkanDevice, extent);
		else
		{
			vulkanSwapChain = std::make_unique<VulkanSwapChain>(vulkanDevice, extent, std::move(vulkanSwapChain));
			if (commandBuffers.size() != vulkanSwapChain->imageCount())
			{
				freeCommandBuffers();
				createCommandBuffers();
			}
		}

		createPipeline();
	}

	void SandboxApp::recordCommandBuffer(int imageIndex)
	{
		VkCommandBufferBeginInfo beginInfo{};
		beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;

		// Begin recording to command buffer
		if (vkBeginCommandBuffer(commandBuffers[imageIndex], &beginInfo) != VK_SUCCESS)
			throw std::runtime_error("Failed to begin recording to command buffer!");

		// Render pass command info 
		VkRenderPassBeginInfo renderPassInfo{};
		renderPassInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
		renderPassInfo.renderPass = vulkanSwapChain->getRenderPass();
		renderPassInfo.framebuffer = vulkanSwapChain->getFrameBuffer(imageIndex);

		renderPassInfo.renderArea.offset = { 0, 0 };
		renderPassInfo.renderArea.extent = vulkanSwapChain->getSwapChainExtent();

		std::array<VkClearValue, 2> clearValues{};
		clearValues[0].color = { 0.4f, 0.8f, 0.6f, 1.0f };
		clearValues[1].depthStencil = { 1.0f, 0 };
		renderPassInfo.clearValueCount = static_cast<uint32_t>(clearValues.size());
		renderPassInfo.pClearValues = clearValues.data();

		// Begin render pass command
		/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
		vkCmdBeginRenderPass(commandBuffers[imageIndex], &renderPassInfo, VK_SUBPASS_CONTENTS_INLINE);

		// Create the dynamic viewport/scissor and pass to the command buffer
		VkViewport viewport{};
		viewport.x = 0.0f;
		viewport.y = 0.0f;
		auto extent = vulkanSwapChain->getSwapChainExtent();
		viewport.width = static_cast<float>(extent.width);
		viewport.height = static_cast<float>(extent.height);
		viewport.minDepth = 0.0f;
		viewport.maxDepth = 1.0f;
		vkCmdSetViewport(commandBuffers[imageIndex], 0, 1, &viewport);
		VkRect2D scissor{ { 0,0 }, extent };
		vkCmdSetScissor(commandBuffers[imageIndex], 0, 1, &scissor);

		vulkanPipeline->bind(commandBuffers[imageIndex]);

		testModel1->bind(commandBuffers[imageIndex]);
		testModel1->draw(commandBuffers[imageIndex]);
		testModel2->bind(commandBuffers[imageIndex]);
		testModel2->draw(commandBuffers[imageIndex]);

		vkCmdEndRenderPass(commandBuffers[imageIndex]);
		/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
		// End render pass command

		if (vkEndCommandBuffer(commandBuffers[imageIndex]) != VK_SUCCESS)
			throw std::runtime_error("Failed to record command buffer!");
	}

	void SandboxApp::loadSandboxModels()
	{
		std::vector<Model::Vertex> vertices1{
			//     Positions         Colours
				{ {  0.0f, -0.5f }, { 1.0f, 0.0f, 0.0f, 1.0f } },
				{ {-0.35f,  0.5f }, { 0.4f, 0.8f, 0.6f, 1.0f } },
				{ { 0.35f,  0.5f }, { 0.0f, 0.0f, 1.0f, 1.0f } }
		};
		testModel1 = std::make_unique<Model>(vulkanDevice, vertices1);

		std::vector<Model::Vertex> vertices2{
			//     Positions        Colours
				{ { 0.0f, -0.8f }, { 1.0f, 0.0f, 0.0f, 1.0f } },
				{ {-0.1f, -0.5f }, { 0.4f, 0.8f, 0.6f, 1.0f } },
				{ { 0.1f, -0.5f }, { 0.0f, 0.0f, 1.0f, 1.0f } }
		};
		testModel2 = std::make_unique<Model>(vulkanDevice, vertices2);
	}
}
