#pragma once
#include "GfxBase.h"
#include "Mesh.h"
#include "Material.h"
#include "Components/Transform.h"
#include "Camera.h"
#include "CharlieCore/AssetHandler.h"
#include <glm/gtc/type_ptr.hpp>
#include <entt/entt.hpp>
#include <memory>
#include "Lighting.h"
#include "shared.h"
#include "OPENGL4/OpenGLMesh.h"
#include "Shared/GenericMesh.h"
namespace Cle::Renderer
{
	class OPENGLRenderer;
	class IRenderer
	{
	private:
		std::unordered_map<
			std::shared_ptr<
			Cle::GenericMesh>,
			std::shared_ptr<Cle::Gfx::IMesh>
		> gpuMeshCache;
		std::unordered_map<std::string, std::shared_ptr<Cle::Gfx::ITexture>> textureCache;
	//	std::unordered_map<Cle::Components::MaterialRef, std::shared_ptr<Cle::Gfx::Material>> materialCache;
	public:
		std::unordered_map<std::string, std::shared_ptr<IShader>> shaderCache;

		virtual std::shared_ptr < Cle::Gfx::IMesh> getOrMakeMesh(std::shared_ptr<Cle::GenericMesh> mesh)
		{
			if (!gpuMeshCache.contains(mesh))
			{
				if (Cle::Gfx::G_PIPE_LINE == Cle::Gfx::OPENGL)
				{
					gpuMeshCache[mesh] = std::make_shared<OPENGL::Mesh>(mesh);
					gpuMeshCache[mesh]->gpuUploaded = true;
				}
			}

			return gpuMeshCache[mesh];
		}
		virtual std::shared_ptr<Cle::Gfx::ITexture> getOrMakeTexture(const std::string& path)
		{
			if (!textureCache.contains(path))
			{
				if (Cle::Gfx::G_PIPE_LINE == Cle::Gfx::OPENGL)
				{
					textureCache[path] = std::make_shared<OPENGL::Texture>(path);

				}
			}

			return textureCache[path];
		}

		/*
		virtual std::shared_ptr<Cle::Gfx::Material> getOrMakeMaterial(Cle::Components::MaterialRef& mat)
		{
			if (!materialCache.contains(mat))
			{				
				materialCache[mat] = std::make_shared<Cle::Gfx::Material>();
				auto& matc = materialCache[mat];
				matc->setColor(mat.color);
				matc->setColorMap(getOrMakeTexture(mat.colorMapPath));

			}

			return materialCache[mat];
		}*/
	
		entt::registry* m_registry;
		virtual ~IRenderer() = default;
		virtual std::shared_ptr<Cle::IShader> getDefaultShader() = 0;
		virtual void setSettings() = 0;
		virtual void beginFrame() = 0;
		virtual void clearFrame(GLFWwindow* window) = 0;
		virtual void clearColor(float r, float g, float b, float w) = 0;
		virtual void drawMesh(entt::entity e, entt::registry& registry, Cle::Gfx::Camera& camera) = 0;
		virtual void uploadMesh(entt::entity e, std::shared_ptr<Cle::GenericMesh>
mesh, entt::registry& registry) = 0;
		virtual void lightPass() = 0;
		//virtual void SyncMeshes(entt::registry& registry) = 0;
		virtual void UniformCamMatrix(Cle::Gfx::Camera& camera, std::shared_ptr<Cle::IShader> shader) = 0;
		virtual void lightingPass() = 0;
		virtual void clear() = 0;
		virtual void onSceneLoaded() = 0;
		virtual std::shared_ptr<Cle::Gfx::ITexture> phraseSkybox(std::vector<std::string> skybox) = 0;
		virtual void cleanDirtyMesh(entt::entity entity) = 0;
		virtual void passSkybox() = 0;
		virtual std::shared_ptr<Cle::Gfx::IMesh> assignLOD(entt::entity entity, glm::vec3 viewPosition) = 0;
		static std::unique_ptr<IRenderer> Create(entt::registry* registry);
	};
}