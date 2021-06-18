#include "SandboxApp.hpp"

namespace VulkanSandbox {

void SandboxApp::run()
{
	while ( !appWindow.shouldClose() ) {
		glfwPollEvents();
	}
}

}
