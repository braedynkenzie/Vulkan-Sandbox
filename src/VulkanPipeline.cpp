#include "VulkanPipeline.hpp"

#include <fstream>
#include <iostream>
#include <stdexcept>
//#include <assert.h>
#include <cassert>

namespace VulkanSandbox {

	VulkanPipeline::VulkanPipeline(VulkanDevice& vulkanDevice, const std::string& vertexShaderFilepath, const std::string& fragmentShaderFilepath, const PipelineConfigInfo& configInfo)
		: vulkanDeviceRef(vulkanDevice)
	{
		createGraphicsPipeline(vertexShaderFilepath, fragmentShaderFilepath, configInfo);
	}

	VulkanPipeline::~VulkanPipeline()
	{
		vkDestroyShaderModule(vulkanDeviceRef.device(), vertexShaderModule, nullptr);
		vkDestroyShaderModule(vulkanDeviceRef.device(), fragmentShaderModule, nullptr);
		vkDestroyPipeline(vulkanDeviceRef.device(), graphicsPipeline, nullptr);
	}

	void VulkanPipeline::bind(VkCommandBuffer commandBuffer)
	{
		vkCmdBindPipeline(commandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, graphicsPipeline);
	}

	void VulkanPipeline::setupDefaultPipelineConfigInfo(PipelineConfigInfo& configInfo, uint32_t width, uint32_t height)
	{
		// VkPipelineInputAssemblyStateCreateInfo
		configInfo.inputAssemblyInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO;
		configInfo.inputAssemblyInfo.topology = VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST;
		configInfo.inputAssemblyInfo.primitiveRestartEnable = VK_FALSE;

		// VkViewport
		configInfo.viewport.x = 0.0f;
		configInfo.viewport.y = 0.0f;
		configInfo.viewport.width = static_cast<float>(width);
		configInfo.viewport.height = static_cast<float>(height);
		configInfo.viewport.minDepth = 0.0f;
		configInfo.viewport.maxDepth = 1.0f;

		// VkRect2D (crops viewport)
		configInfo.scissor.offset = { 0,0 };
		configInfo.scissor.extent = { width, height };

		// VkPipelineViewportStateCreateInfo (combines the above two state elements)
		VkPipelineViewportStateCreateInfo viewportInfo{};
		viewportInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_STATE_CREATE_INFO;
		viewportInfo.viewportCount = 1;
		viewportInfo.pViewports = &configInfo.viewport; // pointer to properties set above
		viewportInfo.scissorCount = 1;
		viewportInfo.pScissors = &configInfo.scissor; // pointer to properties set above

		// VkPipelineRasterizationStateCreateInfo
		configInfo.rasterizationInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_STATE_CREATE_INFO;
		configInfo.rasterizationInfo.depthClampEnable = VK_FALSE; // clamps depth buffer to [0,1]
		configInfo.rasterizationInfo.rasterizerDiscardEnable = VK_FALSE;
		configInfo.rasterizationInfo.polygonMode = VK_POLYGON_MODE_FILL;
		configInfo.rasterizationInfo.lineWidth = 1.0f;
		configInfo.rasterizationInfo.cullMode = VK_CULL_MODE_NONE;
		configInfo.rasterizationInfo.frontFace = VK_FRONT_FACE_CLOCKWISE;
		configInfo.rasterizationInfo.depthBiasEnable = VK_FALSE;
		configInfo.rasterizationInfo.depthBiasConstantFactor = 0.0f;
		configInfo.rasterizationInfo.depthBiasClamp = 0.0f;
		configInfo.rasterizationInfo.depthBiasSlopeFactor = 0.0f;

		// VkPipelineMultisampleStateCreateInfo
		configInfo.multisampleInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_MULTISAMPLE_STATE_CREATE_INFO;
		configInfo.multisampleInfo.sampleShadingEnable = VK_FALSE;
		configInfo.multisampleInfo.rasterizationSamples = VK_SAMPLE_COUNT_1_BIT;
		configInfo.multisampleInfo.minSampleShading = 1.0f;
		configInfo.multisampleInfo.pSampleMask = nullptr;
		configInfo.multisampleInfo.alphaToCoverageEnable = VK_FALSE;
		configInfo.multisampleInfo.alphaToOneEnable = VK_FALSE;

		// VkPipelineColorBlendAttachmentState
		configInfo.colorBlendAttachment.colorWriteMask =
			VK_COLOR_COMPONENT_R_BIT | VK_COLOR_COMPONENT_G_BIT | VK_COLOR_COMPONENT_B_BIT | VK_COLOR_COMPONENT_A_BIT;
		configInfo.colorBlendAttachment.blendEnable = VK_FALSE;
		configInfo.colorBlendAttachment.srcColorBlendFactor = VK_BLEND_FACTOR_ONE;
		configInfo.colorBlendAttachment.dstColorBlendFactor = VK_BLEND_FACTOR_ZERO;
		configInfo.colorBlendAttachment.colorBlendOp = VK_BLEND_OP_ADD;
		configInfo.colorBlendAttachment.srcAlphaBlendFactor = VK_BLEND_FACTOR_ONE;
		configInfo.colorBlendAttachment.dstAlphaBlendFactor = VK_BLEND_FACTOR_ZERO;
		configInfo.colorBlendAttachment.alphaBlendOp = VK_BLEND_OP_ADD;

		// VkPipelineColorBlendStateCreateInfo
		configInfo.colorBlendInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO;
		configInfo.colorBlendInfo.logicOpEnable = VK_FALSE;
		configInfo.colorBlendInfo.logicOp = VK_LOGIC_OP_COPY;
		configInfo.colorBlendInfo.attachmentCount = 1;
		configInfo.colorBlendInfo.pAttachments = &configInfo.colorBlendAttachment;
		configInfo.colorBlendInfo.blendConstants[0] = 0.0f;
		configInfo.colorBlendInfo.blendConstants[1] = 0.0f;
		configInfo.colorBlendInfo.blendConstants[2] = 0.0f;
		configInfo.colorBlendInfo.blendConstants[3] = 0.0f;

		// VkPipelineDepthStencilStateCreateInfo
		configInfo.depthStencilInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_DEPTH_STENCIL_STATE_CREATE_INFO;
		configInfo.depthStencilInfo.depthTestEnable = VK_TRUE;
		configInfo.depthStencilInfo.depthWriteEnable = VK_TRUE;
		configInfo.depthStencilInfo.depthCompareOp = VK_COMPARE_OP_LESS;
		configInfo.depthStencilInfo.depthBoundsTestEnable = VK_FALSE;
		configInfo.depthStencilInfo.minDepthBounds = 0.0f;
		configInfo.depthStencilInfo.maxDepthBounds = 1.0f;
		configInfo.depthStencilInfo.stencilTestEnable = VK_FALSE;
		configInfo.depthStencilInfo.front = {};
		configInfo.depthStencilInfo.back = {}; 
	}

	void VulkanPipeline::createGraphicsPipeline(const std::string& vertexShaderFilepath, const std::string& fragmentShaderFilepath, const PipelineConfigInfo& configInfo)
	{
		assert(configInfo.pipelineLayout != VK_NULL_HANDLE && "Cannot create graphics pipeline -- missing pipelineLayout in configInfo!");
		assert(configInfo.renderPass != VK_NULL_HANDLE && "Cannot create graphics pipeline -- missing renderPass in configInfo!");

		// Read SPIR-V compiled shaders' source code into vector<char> buffers 
		std::vector<char> vertexShaderSourceCode = readFile(vertexShaderFilepath);
		std::vector<char> fragmentShaderSourceCode = readFile(fragmentShaderFilepath);
		std::cout << "VS file size: " << vertexShaderSourceCode.size() << std::endl;
		std::cout << "FS file size: " << fragmentShaderSourceCode.size() << std::endl;

		// Create Vulkan shader program modules 
		createShaderModule(vertexShaderSourceCode, &vertexShaderModule);
		createShaderModule(fragmentShaderSourceCode, &fragmentShaderModule);

		// Set up info for shader stages (Vertex and Fragment stages only for now)
		VkPipelineShaderStageCreateInfo shaderStagesInfo[2];
		// Vertex shader stage
		shaderStagesInfo[0].sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
		shaderStagesInfo[0].stage = VK_SHADER_STAGE_VERTEX_BIT;
		shaderStagesInfo[0].module = vertexShaderModule;
		shaderStagesInfo[0].pName = "main";
		shaderStagesInfo[0].flags = 0;
		shaderStagesInfo[0].pNext = nullptr;
		shaderStagesInfo[0].pSpecializationInfo = nullptr;
		// Fragment shader stage
		shaderStagesInfo[1].sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
		shaderStagesInfo[1].stage = VK_SHADER_STAGE_FRAGMENT_BIT;
		shaderStagesInfo[1].module = fragmentShaderModule;
		shaderStagesInfo[1].pName = "main";
		shaderStagesInfo[1].flags = 0;
		shaderStagesInfo[1].pNext = nullptr;
		shaderStagesInfo[1].pSpecializationInfo = nullptr;

		// Define how the vertex buffer data is interpreted 
		VkPipelineVertexInputStateCreateInfo vertexShaderInputInfo{}; 
		vertexShaderInputInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO;
		vertexShaderInputInfo.vertexAttributeDescriptionCount = 0; // TODO when shaders have vertex buffers
		vertexShaderInputInfo.vertexBindingDescriptionCount = 0; // TODO when shaders have vertex buffers
		vertexShaderInputInfo.pVertexAttributeDescriptions = nullptr;
		vertexShaderInputInfo.pVertexBindingDescriptions = nullptr;

		// Now, combine all of these settings into a single struct
		VkGraphicsPipelineCreateInfo vulkanPipelineInfo{};
		vulkanPipelineInfo.sType = VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO;
		vulkanPipelineInfo.stageCount = 2; // vertex and fragments shaders
		vulkanPipelineInfo.pStages = shaderStagesInfo;
		vulkanPipelineInfo.pVertexInputState = &vertexShaderInputInfo;
		vulkanPipelineInfo.basePipelineIndex = -1;
		vulkanPipelineInfo.basePipelineHandle = VK_NULL_HANDLE;
		// Use the given configInfo struct for the following settings:
		vulkanPipelineInfo.pInputAssemblyState = &configInfo.inputAssemblyInfo;
		vulkanPipelineInfo.pViewportState = &configInfo.viewportInfo;
		vulkanPipelineInfo.pRasterizationState = &configInfo.rasterizationInfo;
		vulkanPipelineInfo.pMultisampleState = &configInfo.multisampleInfo;
		vulkanPipelineInfo.pColorBlendState = &configInfo.colorBlendInfo;
		vulkanPipelineInfo.pDepthStencilState = &configInfo.depthStencilInfo;
		vulkanPipelineInfo.pDynamicState = nullptr;
		vulkanPipelineInfo.layout = configInfo.pipelineLayout;
		vulkanPipelineInfo.renderPass = configInfo.renderPass;
		vulkanPipelineInfo.subpass = configInfo.subpass;

		// Finally, use the vulkanDeviceRef with this vulkanPipelineInfo to create the graphicsPipeline!
		if (vkCreateGraphicsPipelines(vulkanDeviceRef.device(), VK_NULL_HANDLE, 1, &vulkanPipelineInfo, nullptr, &graphicsPipeline) != VK_SUCCESS)
			throw std::runtime_error("Failed to create a graphics pipeline!");

	}

	std::vector<char> VulkanPipeline::readFile(const std::string& filepath)
	{
		// Open the file, throw an error if it doesn't work
		std::ifstream fileStream = std::ifstream(filepath, std::ios::ate | std::ios::binary);
		if (!fileStream.is_open()) {
			throw std::runtime_error("Failed to open file: " + filepath);
		}

		// Get the file size (note that std::ios::ate bit flag above means the file stream 
		// begins at the end of the file so .tellg() will give us the total file size)
		size_t fileSize = (size_t)fileStream.tellg();
		std::vector<char> fileBuffer(fileSize);

		fileStream.seekg(0);
		fileStream.read(fileBuffer.data(), fileSize);

		fileStream.close();
		return fileBuffer;
	}

	void VulkanPipeline::createShaderModule(const std::vector<char>& shaderSourceCode, VkShaderModule* shaderModule)
	{
		VkShaderModuleCreateInfo shaderModuleCreateInfo{};
		shaderModuleCreateInfo.sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO;
		shaderModuleCreateInfo.codeSize = shaderSourceCode.size();
		shaderModuleCreateInfo.pCode = reinterpret_cast<const uint32_t*>(shaderSourceCode.data()); // allowed since vectors ensure 32 bit alignment

		if (vkCreateShaderModule(vulkanDeviceRef.device(), &shaderModuleCreateInfo, nullptr, shaderModule) != VK_SUCCESS)
			throw std::runtime_error("Failed to create shader module!");
	}

}