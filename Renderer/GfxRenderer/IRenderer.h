#pragma once
#include "GfxBase.h"
#include "Mesh.h"
#include "Material.h"
#include "CharlieEngine/Transform.h"
#include "Camera.h"
#include "CharlieEngine/AssetHandler.h"
#include <glm/gtc/type_ptr.hpp>
#include <entt/entt.hpp>
#include <memory>
#include "Lighting.h"
#include "shared.h"
#include "OPENGL4/OpenGLMesh.h"
#include "CharlieEngine/GenericMesh.h"
namespace Cle::Renderer
{
	class OPENGLRenderer;
	class IRenderer
	{
	protected:
		/*std::unordered_map<
			std::shared_ptr<
			Cle::GenericMesh>,
			std::shared_ptr<Cle::Gfx::IMesh>*/
			std::unordered_map<
			const void*,
			std::shared_ptr<Cle::Gfx::IMesh>
		> gpuMeshCache;
		std::unordered_map<std::string, std::shared_ptr<Cle::Gfx::ITexture>> textureCache;
		//	std::unordered_map<Cle::Components::MaterialRef, std::shared_ptr<Cle::Gfx::Material>> materialCache;
	public:
		int resolutionWidth = 2000;
		int resolutionHeight = 2000;
		int width, height = 1;

		void onDeleteFunction(entt::registry& r, entt::entity e)
		{
			std::cout << "onDeleteFunction fired for entity " << (uint32_t)e << "\n";

			if (!r.any_of<std::shared_ptr<Cle::GenericMesh>>(e)) return;

			auto& mesh = r.get<std::shared_ptr<Cle::GenericMesh>>(e);
			std::string modelPath = mesh->getModelPath();
			const void* geoId = mesh->getGeoID();
			auto index = mesh->geometry->instancedIndex;

			auto& vec = AssetHandler::getInstance().modelCache[modelPath];

			AssetHandler::getInstance().UnloadModel(modelPath, index,mesh);
			mesh.reset();

		//	auto gpuIt = gpuMeshCache.find(geoId);
	//		std::cout << "count " << gpuIt->second.use_count() << std::endl;
		//	std::cout << "cache size assets " << AssetHandler::getInstance().meshCache.size() << std::endl;

			/*if (gpuIt != gpuMeshCache.end() && gpuIt->second.use_count() <= 1)
			{
				std::cout << "needs to be deleted " << gpuMeshCache.size() << std::endl;
				gpuMeshCache.erase(gpuIt);

			}*/
				

			if (r.any_of<std::shared_ptr<Cle::Gfx::ITexture>>(e))
			{
				auto& tex = r.get<std::shared_ptr<Cle::Gfx::ITexture>>(e);
				auto texIt = textureCache.find(tex->getPath());
				if (texIt != textureCache.end() && texIt->second.use_count() <= 1)
					textureCache.erase(texIt);
			}
		//	std::cout << "called " << gpuMeshCache.size() << std::endl;

		}
		std::unordered_map<std::string, std::shared_ptr<IShader>> shaderCache;

		virtual std::shared_ptr < Cle::Gfx::IMesh> getOrMakeMesh(std::shared_ptr<Cle::GenericMesh> mesh) = 0;
		virtual std::shared_ptr<Cle::Gfx::ITexture> getOrMakeTexture(const std::string& path) = 0;
	

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
		virtual void drawRegistry(Cle::Gfx::Camera& m_camera, GLFWwindow* window) = 0;

		virtual std::shared_ptr<Cle::IShader> getDefaultShader() = 0;
		virtual void setSettings() = 0;
		virtual void beginFrame() = 0;
		virtual void clearFrame(GLFWwindow* window) = 0;
		virtual void clearColor(float r, float g, float b, float w) = 0;
		virtual void drawMesh(entt::entity e, entt::registry& registry, Cle::Gfx::Camera& camera) = 0;
		virtual void uploadMesh(entt::entity e, std::shared_ptr<Cle::GenericMesh>
			mesh, entt::registry& registry);
		virtual void lightPass() = 0;
		//virtual void SyncMeshes(entt::registry& registry) = 0;
		virtual void UniformCamMatrix(Cle::Gfx::Camera& camera, std::shared_ptr<Cle::IShader> shader) ;
		virtual void lightingPass() ;
		virtual std::shared_ptr<Cle::Gfx::ITexture> phraseSkybox(std::vector<std::string> skybox) = 0;
		virtual void cleanDirtyMesh(entt::entity entity);
		virtual bool isWithinFarPlane(entt::entity entity, Cle::Gfx::Camera& camera);
		virtual std::shared_ptr<Cle::Gfx::IMesh> assignLOD(entt::entity entity, glm::vec3 viewPosition) = 0;
		virtual unsigned int getImage() const = 0;
		static std::unique_ptr<IRenderer> Create(entt::registry* registry);
		virtual void onSceneLoaded();
		static inline const std::vector<float> cube = {
			-1.0f,  1.0f, -1.0f,
			-1.0f, -1.0f, -1.0f,
			 1.0f, -1.0f, -1.0f,
			 1.0f, -1.0f, -1.0f,
			 1.0f,  1.0f, -1.0f,
			-1.0f,  1.0f, -1.0f,

			-1.0f, -1.0f,  1.0f,
			-1.0f, -1.0f, -1.0f,
			-1.0f,  1.0f, -1.0f,
			-1.0f,  1.0f, -1.0f,
			-1.0f,  1.0f,  1.0f,
			-1.0f, -1.0f,  1.0f,

			 1.0f, -1.0f, -1.0f,
			 1.0f, -1.0f,  1.0f,
			 1.0f,  1.0f,  1.0f,
			 1.0f,  1.0f,  1.0f,
			 1.0f,  1.0f, -1.0f,
			 1.0f, -1.0f, -1.0f,

			-1.0f, -1.0f,  1.0f,
			-1.0f,  1.0f,  1.0f,
			 1.0f,  1.0f,  1.0f,
			 1.0f,  1.0f,  1.0f,
			 1.0f, -1.0f,  1.0f,
			-1.0f, -1.0f,  1.0f,

			-1.0f,  1.0f, -1.0f,
			 1.0f,  1.0f, -1.0f,
			 1.0f,  1.0f,  1.0f,
			 1.0f,  1.0f,  1.0f,
			-1.0f,  1.0f,  1.0f,
			-1.0f,  1.0f, -1.0f,

			-1.0f, -1.0f, -1.0f,
			-1.0f, -1.0f,  1.0f,
			 1.0f, -1.0f, -1.0f,
			 1.0f, -1.0f, -1.0f,
			-1.0f, -1.0f,  1.0f,
			 1.0f, -1.0f,  1.0f
		};
	};
}