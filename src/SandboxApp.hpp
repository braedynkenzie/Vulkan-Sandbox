#pragma once

#include "SandboxWindow.hpp"
#include "VulkanPipeline.hpp"
#include "VulkanDevice.hpp"
#include "VulkanSwapChain.hpp"
#include "SandboxObject.hpp"

#include <memory>
#include <vector>

namespace VulkanSandbox {

	class SandboxApp {

	public:
		static constexpr int WIDTH = 1920;
		static constexpr int HEIGHT = 1080;
		const std::string APP_NAME = "Vulkan Sandbox";

		SandboxApp();
		~SandboxApp();

		SandboxApp(const SandboxApp&) = delete;
		SandboxApp& operator=(const SandboxApp&) = delete;

		void run();

	private:
		void createPipelineLayout();
		void createPipeline();
		void createCommandBuffers();
		void freeCommandBuffers();
		void drawFrame();
		void recreateSwapChain();
		void recordCommandBuffer(int imageIndex);
		void renderSandboxObjects(VkCommandBuffer commandBuffer);
		void loadSandboxObjects();

		SandboxWindow appWindow{ WIDTH, HEIGHT, APP_NAME };
		VulkanDevice vulkanDevice{ appWindow };
		std::unique_ptr<VulkanSwapChain> vulkanSwapChain;
		std::unique_ptr<VulkanPipeline> vulkanPipeline;
		VkPipelineLayout pipelineLayout;
		std::vector<VkCommandBuffer> commandBuffers;

		// Temp
		std::vector<SandboxObject> sandboxObjects;
	};


}