#include "SandboxWindow.hpp"

#include <stdexcept>

namespace VulkanSandbox
{
	SandboxWindow::SandboxWindow(int windowWidth, int windowHeight, std::string name)
		: width(windowWidth), height(windowHeight), windowName(name)
	{ 
		InitWindow();
	}

	SandboxWindow::~SandboxWindow()
	{
		glfwDestroyWindow(window);
		glfwTerminate();
	}

	void SandboxWindow::InitWindow()
	{
		glfwInit();
		glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
		glfwWindowHint(GLFW_RESIZABLE, GLFW_FALSE);

		window = glfwCreateWindow(width, height, windowName.c_str(), nullptr, nullptr);
	}

	bool SandboxWindow::shouldClose() 
	{
		return glfwWindowShouldClose(window);
	}

	void SandboxWindow::createWindowSurface(VkInstance vulkanInstance, VkSurfaceKHR* vulkanSurface)
	{
		if (glfwCreateWindowSurface(vulkanInstance, window, nullptr, vulkanSurface) != VK_SUCCESS)
			throw std::runtime_error("Failed to create a window surface!");
	}
}

