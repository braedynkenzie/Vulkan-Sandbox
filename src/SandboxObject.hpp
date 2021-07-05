#pragma once

#include "Model.hpp"

#include <memory>

namespace VulkanSandbox {

	struct Transform2DComponent {
		glm::vec2 translation{ 0.0f, 0.0f };
		float rotation = 0.0f;
		glm::vec2 scale{ 1.0f, 1.0f };

		glm::mat2 mat2() { 
			const float s = glm::sin(rotation);
			const float c = glm::cos(rotation);
			glm::mat2 rotMatrix{ {c, s}, {-s, c} };

			glm::mat2 scaleMatrix{ { scale.x, 0.0f }, { 0.0f, scale.y } };

			glm::mat2 modelMatrix = rotMatrix * scaleMatrix;
			return modelMatrix;
		}
	};

	class SandboxObject {

	public:
		using id_t = unsigned int;

		static SandboxObject createSandboxObject() {
			static id_t currentId = 0;
			return SandboxObject(currentId++);
		}

		SandboxObject(const SandboxObject&) = delete;
		SandboxObject& operator=(const SandboxObject&) = delete;
		SandboxObject(SandboxObject&&) = default;
		SandboxObject& operator=(SandboxObject&&) = default;

		id_t getId() const { return id; }

		const id_t id;
		std::shared_ptr<Model> model;
		glm::vec4 colour;
		Transform2DComponent transform2D;

	private:
		SandboxObject(id_t unique_id) 
			: id(unique_id), colour(glm::vec4(0.0f)), transform2D{ glm::vec2(0.0f, 0.0f) } {}


	};
}
