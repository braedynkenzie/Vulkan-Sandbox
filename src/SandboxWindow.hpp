#pragma once

#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>
#include <string>

namespace VulkanSandbox {

	class SandboxWindow {

	public:
		SandboxWindow(int windowWidth, int windowHeight, std::string name);
		~SandboxWindow();

		// To maintain RAII, don't want to be able to copy a SandboxWindow, since then we will have two pointers to the  
		// same GLFWwindow, and if one SandboxWindow then gets destroyed, the other would be left with a dangling pointer
		SandboxWindow(const SandboxWindow&) = delete;
		SandboxWindow& operator=(const SandboxWindow&) = delete;

		bool shouldClose();

		void createWindowSurface(VkInstance vulkanInstance, VkSurfaceKHR* vulkanSurface);

	private:
		const int width;
		const int height;
		std::string windowName;

		GLFWwindow* window; 

		void InitWindow();

	};

}
