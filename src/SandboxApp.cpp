#include "SandboxApp.hpp"

#define GLM_FORCE_DEPTH_ZERO_TO_ONE
#define GLM_FORCE_RADIANS
#include <glm/glm.hpp>

#include <stdexcept>
#include <array>
#include <iostream>

namespace VulkanSandbox {

	// Temp
	struct BasicPushConstantData {
		glm::vec2 offset;
		alignas(16) glm::vec4 colour;
	};

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
		std::cout << "\nMax push constant size: " << vulkanDevice.properties.limits.maxPushConstantsSize << std::endl;

		while (!appWindow.shouldClose()) {
			glfwPollEvents();
			drawFrame();
		}

		vkDeviceWaitIdle(vulkanDevice.device());
	}

	void SandboxApp::createPipelineLayout()
	{
		VkPushConstantRange pushConstantRange{};
		pushConstantRange.size = sizeof(BasicPushConstantData);
		pushConstantRange.offset = 0;
		pushConstantRange.stageFlags = 
			VK_SHADER_STAGE_VERTEX_BIT | VK_SHADER_STAGE_FRAGMENT_BIT; // so that the push constants can be accessed from either shader

		VkPipelineLayoutCreateInfo pipelineLayoutInfo{};
		pipelineLayoutInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
		pipelineLayoutInfo.pSetLayouts = nullptr;
		pipelineLayoutInfo.pushConstantRangeCount = 1;
		pipelineLayoutInfo.pPushConstantRanges = &pushConstantRange;
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
		static int frame = 0;
		frame = (frame + 1) % 10000;

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
		for (int i = 0; i < 4; i++)
		{
			// Create and pass push constants to shaders, then draw
			BasicPushConstantData pushConstantData{};
			pushConstantData.offset = glm::vec2(0.2f * i * ((float)frame / 10000.0f), 0.2f * i * ((float)frame / 10000.0f));
			pushConstantData.colour = glm::vec4(0.2f * i, 0.2f * i, 0.2f * i, 0.5f + 0.2f * i);
			vkCmdPushConstants(
				commandBuffers[imageIndex],
				pipelineLayout,
				VK_SHADER_STAGE_VERTEX_BIT | VK_SHADER_STAGE_FRAGMENT_BIT,
				0,
				sizeof(BasicPushConstantData),
				&pushConstantData);

			testModel1->draw(commandBuffers[imageIndex]);
		}


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
	}
}
