#pragma once

#include "SandboxWindow.hpp"
#include "VulkanPipeline.hpp"
#include "VulkanDevice.hpp"

namespace VulkanSandbox {

	class SandboxApp {

	public:
		static constexpr int WIDTH = 1920;
		static constexpr int HEIGHT = 1080;
		const std::string APP_NAME = "Vulkan Sandbox";

		void run();

	private:
		SandboxWindow appWindow{ WIDTH, HEIGHT, APP_NAME };
		VulkanDevice vulkanDevice{ appWindow };
		VulkanPipeline vulkanPipeline{ vulkanDevice, "src/shaders/VertexShader.vert.spv", "src/shaders/FragmentShader.frag.spv", VulkanPipeline::getDefaultPipelineConfigInfo(WIDTH, HEIGHT) };
	};


}