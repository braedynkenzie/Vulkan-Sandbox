#include "Model.hpp"

#include <cassert>
#include <cstring>

namespace VulkanSandbox
{
	Model::Model(VulkanDevice& device, std::vector<Vertex>& vertices)
		: vulkanDevice(device)
	{
		createVertexBuffers(vertices);
	}

	Model::~Model()
	{
		vkDestroyBuffer(vulkanDevice.device(), vertexBuffer, nullptr);
		vkFreeMemory(vulkanDevice.device(), vertexBufferMemory, nullptr);
	}

	void Model::bind(VkCommandBuffer commandBuffer)
	{
		VkBuffer buffers[] = { vertexBuffer };
		VkDeviceSize offsets[] = { 0 };
		vkCmdBindVertexBuffers(commandBuffer, 0, 1, buffers, offsets);
	}

	void Model::draw(VkCommandBuffer commandBuffer)
	{
		vkCmdDraw(commandBuffer, vertexCount, 1, 0, 0);
	}

	void Model::createVertexBuffers(std::vector<Vertex>& vertices)
	{
		vertexCount = static_cast<uint32_t>(vertices.size());
		assert(vertexCount >= 3 && "Model's vertex count must be at least 3! ie. needs at least one polygon.");

		// Calculate the vertex buffer size and use that to create the buffer/its associated GPU memory
		VkDeviceSize vertexBufferSize = vertexCount * sizeof(vertices[0]);
		vulkanDevice.createBuffer(
			vertexBufferSize, 
			VK_BUFFER_USAGE_VERTEX_BUFFER_BIT, 
			VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT, 
			vertexBuffer, 
			vertexBufferMemory);

		// Now create a data buffer on the CPUl, map it to the GPU vertexBufferMemory (above 
		// VK_MEMORY_PROPERTY_HOST_COHERENT_BIT ensures changes to this buffer will propagate 
		// to the mapped GPU buffer), and then write our vertices.data() for the vertex shader
		void* vertexBufferData;
		vkMapMemory(vulkanDevice.device(), vertexBufferMemory, 0, vertexBufferSize, 0, &vertexBufferData);
		memcpy(vertexBufferData, vertices.data(), static_cast<size_t>(vertexBufferSize));
		vkUnmapMemory(vulkanDevice.device(), vertexBufferMemory);
	}
	
	std::vector<VkVertexInputBindingDescription> Model::Vertex::getBindingDescriptions()
	{
		std::vector<VkVertexInputBindingDescription> bindingDescriptions(1);
		bindingDescriptions[0].binding = 0;
		bindingDescriptions[0].stride = sizeof(Vertex);
		bindingDescriptions[0].inputRate = VK_VERTEX_INPUT_RATE_VERTEX;
		return bindingDescriptions;

	}
	
	std::vector<VkVertexInputAttributeDescription> Model::Vertex::getAttributeDescriptions()
	{
		std::vector<VkVertexInputAttributeDescription> vertexAttribDescriptions(1);
		vertexAttribDescriptions[0].binding = 0;
		vertexAttribDescriptions[0].location = 0;
		vertexAttribDescriptions[0].offset = 0;
		vertexAttribDescriptions[0].format = VK_FORMAT_R32G32_SFLOAT;
		return vertexAttribDescriptions;
	}
}
