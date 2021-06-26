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
		VkPipelineViewportStateCreateInfo viewportInfo;
		VkPipelineRasterizationStateCreateInfo rasterizationInfo;
		VkPipelineMultisampleStateCreateInfo multisampleInfo;
		VkPipelineColorBlendAttachmentState colorBlendAttachment;
		VkPipelineColorBlendStateCreateInfo colorBlendInfo;
		VkPipelineDepthStencilStateCreateInfo depthStencilInfo;
		std::vector<VkDynamicState> dynamicStateEnables;
		VkPipelineDynamicStateCreateInfo dynamicStateInfo;
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
		VulkanPipeline& operator=(const VulkanPipeline&) = delete;

		void bind(VkCommandBuffer commandBuffer);

		static void setupDefaultPipelineConfigInfo(PipelineConfigInfo& configInfo);

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