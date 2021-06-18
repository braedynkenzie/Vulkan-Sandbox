#include <stdexcept>
#include <iostream>
#include <cstdlib>

#include "SandboxApp.hpp"

int main() {
	VulkanSandbox::SandboxApp app;
	try {
		app.run();
	} 
	catch (const std::exception& e) {
		std::cerr << e.what() << std::endl;
		return EXIT_FAILURE;
	}
	return EXIT_SUCCESS;
}