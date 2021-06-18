#pragma once 

#include <vector>
#include <string>

namespace VulkanSandbox {

	class VulkanPipeline {
	public:
		VulkanPipeline(const std::string& vertexShaderFilepath, const std::string& fragmentShaderFilepath);

	private:
		std::vector<char> readFile(const std::string& filepath);
		void createGraphicsPipeline(const std::string& vertexShaderFilepath, const std::string& fragmentShaderFilepath);

	};

}