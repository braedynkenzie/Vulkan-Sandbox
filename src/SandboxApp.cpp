#include "SandboxApp.hpp"

#include <stdexcept>
#include <array>

namespace VulkanSandbox {

	SandboxApp::SandboxApp()
	{
		loadSandboxModels();
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
		PipelineConfigInfo pipelineConfig{};
		VulkanPipeline::setupDefaultPipelineConfigInfo(pipelineConfig, vulkanSwapChain.width(), vulkanSwapChain.height());
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
		commandBuffers.resize(vulkanSwapChain.imageCount());

		VkCommandBufferAllocateInfo allocInfo{};
		allocInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
		allocInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
		allocInfo.commandPool = vulkanDevice.getCommandPool();
		allocInfo.commandBufferCount = (uint32_t)commandBuffers.size();

		if (vkAllocateCommandBuffers(vulkanDevice.device(), &allocInfo, commandBuffers.data()) != VK_SUCCESS)
			throw std::runtime_error("Failed to allocate command buffers!");

		// Now iterate through and record commands to the command buffers
		for (int i = 0; i < commandBuffers.size(); i++)
		{
			VkCommandBufferBeginInfo beginInfo{};
			beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;

			// Begin recording to command buffer
			if (vkBeginCommandBuffer(commandBuffers[i], &beginInfo) != VK_SUCCESS)
				throw std::runtime_error("Failed to begin recording to command buffer!");

			// Render pass command info 
			VkRenderPassBeginInfo renderPassInfo{};
			renderPassInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
			renderPassInfo.renderPass = vulkanSwapChain.getRenderPass();
			renderPassInfo.framebuffer = vulkanSwapChain.getFrameBuffer(i);

			renderPassInfo.renderArea.offset = { 0, 0 };
			renderPassInfo.renderArea.extent = vulkanSwapChain.getSwapChainExtent();

			std::array<VkClearValue, 2> clearValues{};
			clearValues[0].color = { 0.4f, 0.8f, 0.6f, 1.0f };
			clearValues[1].depthStencil = { 1.0f, 0}; 
			renderPassInfo.clearValueCount = static_cast<uint32_t>(clearValues.size());
			renderPassInfo.pClearValues = clearValues.data();

			vkCmdBeginRenderPass(commandBuffers[i], &renderPassInfo, VK_SUBPASS_CONTENTS_INLINE);

			vulkanPipeline->bind(commandBuffers[i]);

			testModel1->bind(commandBuffers[i]);
			testModel1->draw(commandBuffers[i]);
			testModel2->bind(commandBuffers[i]);
			testModel2->draw(commandBuffers[i]);

			vkCmdEndRenderPass(commandBuffers[i]);

			if (vkEndCommandBuffer(commandBuffers[i]) != VK_SUCCESS)
				throw std::runtime_error("Failed to record command buffer!");
		}
	}

	void SandboxApp::drawFrame()
	{
		uint32_t imageIndex;
		auto result = vulkanSwapChain.acquireNextImage(&imageIndex);

		if (result != VK_SUCCESS && result != VK_SUBOPTIMAL_KHR)
			throw std::runtime_error("Failed to acquire next image index in the swap chain!");

		result = vulkanSwapChain.submitCommandBuffers(&commandBuffers[imageIndex], &imageIndex);
		if (result != VK_SUCCESS)
			throw std::runtime_error("Failed to submit command buffer!");
	}

	void SandboxApp::loadSandboxModels()
	{
		std::vector<Model::Vertex> vertices1{
			{ glm::vec2( 0.0f, -0.5f) },
			{ glm::vec2(-0.4f,  0.5f) },
			{ glm::vec2( 0.4f,  0.5f) }
		};

		testModel1 = std::make_unique<Model>(vulkanDevice, vertices1);


		std::vector<Model::Vertex> vertices2{
			{ glm::vec2( 0.0f, -0.8f) },
			{ glm::vec2(-0.1f, -0.5f) },
			{ glm::vec2( 0.1f, -0.5f) }
		};
		testModel2 = std::make_unique<Model>(vulkanDevice, vertices2);
	}

}
