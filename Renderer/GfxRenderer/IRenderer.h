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
#include "Lighting.h"
namespace Cle::Renderer
{
	class OPENGLRenderer;
	class IRenderer
	{
	public:
		Cle::Gfx::AssetManager m_AssetHandler;
		entt::registry* m_registry;
		virtual ~IRenderer() = default;
		virtual unsigned int getDefaultShader() = 0;
		virtual void setSettings() = 0;
		virtual void beginFrame() = 0;
		virtual void clearFrame(GLFWwindow* window) = 0;
		virtual void clearColor(float r, float g, float b, float w) = 0;
		virtual void drawMesh(entt::entity e, entt::registry& registry, Cle::Gfx::Camera& camera) = 0;
		virtual void uploadMesh(entt::entity e, entt::registry& registry) = 0;
		//virtual void SyncMeshes(entt::registry& registry) = 0;
		virtual void UniformCamMatrix(Cle::Gfx::Camera& camera, Cle::Gfx::Material& material) = 0;
		virtual void lightPass() = 0;
		virtual void lightingPass() = 0;
		virtual void clear() = 0;
		virtual void cleanDirtyMesh(entt::entity entity) = 0;
		virtual int assignLOD(entt::entity entity, glm::vec3 viewPosition) = 0;
		static std::shared_ptr<IRenderer> Create(entt::registry* registry);
		virtual std::shared_ptr<Cle::Gfx::ITexture> createTexture(std::string path) = 0;
	};
}