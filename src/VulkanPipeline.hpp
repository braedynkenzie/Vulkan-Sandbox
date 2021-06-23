#pragma once 

#include "VulkanDevice.hpp"

#include <vector>
#include <string>

namespace VulkanSandbox {

	// See VulkanPipeline::getDefaultPipelineConfigInfo(..) for default settings
	struct PipelineConfigInfo { 

		PipelineConfigInfo(const PipelineConfigInfo&) = delete;
		PipelineConfigInfo& operator=(const PipelineConfigInfo&) = delete;

		VkPipelineInputAssemblyStateCreateInfo inputAssemblyInfo;
		VkViewport viewport;
		VkRect2D scissor; 
		VkPipelineViewportStateCreateInfo viewportInfo;
		VkPipelineRasterizationStateCreateInfo rasterizationInfo;
		VkPipelineMultisampleStateCreateInfo multisampleInfo;
		VkPipelineColorBlendAttachmentState colorBlendAttachment;
		VkPipelineColorBlendStateCreateInfo colorBlendInfo;
		VkPipelineDepthStencilStateCreateInfo depthStencilInfo;

		// No default values
		VkPipelineLayout pipelineLayout = nullptr;
		VkRenderPass renderPass = nullptr;
		uint32_t subpass = 0;
	};

	class VulkanPipeline {

	public:

		VulkanPipeline(
			VulkanDevice& vulkanDevice,
			const std::string& vertexShaderFilepath,
			const std::string& fragmentShaderFilepath,
			const PipelineConfigInfo& configInfo);

		~VulkanPipeline();

		VulkanPipeline(const VulkanPipeline&) = delete;
		void operator=(const VulkanPipeline&) = delete;

		void bind(VkCommandBuffer commandBuffer);

		static void setupDefaultPipelineConfigInfo(PipelineConfigInfo& configInfo, uint32_t width, uint32_t height);

	private:

		void createGraphicsPipeline(
			const std::string& vertexShaderFilepath,
			const std::string& fragmentShaderFilepath,
			const PipelineConfigInfo& configInfo);

		std::vector<char> readFile(const std::string& filepath);

		void createShaderModule(const std::vector<char>& shaderSourceCode, VkShaderModule* shaderModule);

		VulkanDevice& vulkanDeviceRef;
		VkPipeline graphicsPipeline;
		VkShaderModule vertexShaderModule;
		VkShaderModule fragmentShaderModule;
	};

}