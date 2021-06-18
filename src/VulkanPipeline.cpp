#include "VulkanPipeline.hpp"

#include <fstream>
#include <iostream>
#include <stdexcept>

namespace VulkanSandbox {

	VulkanPipeline::VulkanPipeline(const std::string& vertexShaderFilepath, const std::string& fragmentShaderFilepath)
	{
		createGraphicsPipeline(vertexShaderFilepath, fragmentShaderFilepath);
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

	void VulkanPipeline::createGraphicsPipeline(const std::string& vertexShaderFilepath, const std::string& fragmentShaderFilepath)
	{
		std::vector<char> vertexShaderSource = readFile(vertexShaderFilepath);
		std::vector<char> fragmentShaderSource = readFile(fragmentShaderFilepath);

		// TODO
		std::cout << "VS file size: " << vertexShaderSource.size() << std::endl;
		std::cout << "FS file size: " << fragmentShaderSource.size() << std::endl;
	}

}