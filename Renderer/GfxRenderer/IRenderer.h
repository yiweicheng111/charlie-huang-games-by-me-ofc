#pragma once
#include "GfxBase.h"
#include "Mesh.h"
#include "Material.h"
#include "Components/Transform.h"
#include "Camera.h"
#include <glad/glad.h>
#include <glfw/glfw3.h>
#include <glm/gtc/type_ptr.hpp>
#include <entt/entt.hpp>
#include <memory>
namespace Cle::Renderer
{
	class OPENGLRenderer;
	class IRenderer
	{
	public:
		Cle::Gfx::GenericMeshHandler m_GenericMeshHandler;
		virtual ~IRenderer() = default;
		virtual Cle::Gfx::Material getMaterial() = 0;
		virtual void setSettings() = 0;
		virtual void beginFrame() = 0;
		virtual void clearFrame(GLFWwindow* window) = 0;
		virtual void clearColor(float r, float g, float b, float w) = 0;
		virtual void drawMesh(entt::entity e, entt::registry& registry) = 0;
		virtual void uploadMesh(entt::entity e, entt::registry& registry) = 0;

		virtual void UniformCamMatrix(Cle::Gfx::Camera& camera, Cle::Gfx::Material& material) = 0;

		static std::shared_ptr<IRenderer> Create();
	};
}