#include "IRenderer.h"
#include "OpenGL4/OpenGLRenderer.h"
#include <glm/gtx/norm.hpp>
namespace Cle::Renderer
{
	
	std::unique_ptr<IRenderer> Cle::Renderer::IRenderer::Create(entt::registry* registry)

	{
		if (Gfx::G_PIPE_LINE == Gfx::Pipeline::OPENGL)
		{
			return std::make_unique<Cle::OPENGL::Renderer>(registry);
		}
		return nullptr;
	}

	void Cle::Renderer::IRenderer::UniformCamMatrix(Cle::Gfx::Camera& camera, std::shared_ptr<Cle::IShader> shader) {
		shader->Bind();
		shader->setMat4("camMatrix", camera.getProjection() * camera.getViewMatrix());
		shader->setVec3("camPos", camera.Position);
		auto& instance = Cle::Lighting::getInstance();
		shader->setFloat("fogStart", instance.fogStart);
		shader->setFloat("fogEnd", std::min({ instance.fogEnd,camera.far }));
	}		


	void Cle::Renderer::IRenderer::lightingPass()
	{
		static auto shader = getDefaultShader();
		auto& instance = Cle::Lighting::getInstance();
		auto& sunDir = instance.sunDirection;
		float ambient = instance.ambient;
		auto& backgroundColor = instance.backgroundColor;
		auto& ambientColor = instance.ambientColor;

		shader->Bind();

		shader->setVec3("sunDirection", sunDir);
		shader->setVec3("ambientColor", ambientColor);
		shader->setVec3("backgroundColor", backgroundColor);
		shader->setFloat("ambientAmount", ambient);
		shader->setFloat("fogStart", instance.fogStart);


	}


	void Cle::Renderer::IRenderer::cleanDirtyMesh(entt::entity entity)
	{


		auto& gmesh = m_registry->get<std::shared_ptr<GenericMesh>>(entity);

		if (gmesh->modelPathDirty || gmesh->modelIndexDirty)
		{

			const auto& ModelLoaded = Cle::AssetHandler::getInstance().LoadModel(gmesh->getModelPath());

			uploadMesh(entity, ModelLoaded.at(gmesh->getMeshIndex()), *m_registry);

			gmesh = m_registry->get<std::shared_ptr<GenericMesh>>(entity);

			gmesh->indicesDirty = false;
			gmesh->verticesDirty = false;
			gmesh->modelPathDirty = false;
			gmesh->modelIndexDirty = false;
			gmesh->m_local_AABB.dirty = true;
			gmesh->m_local_Bounding_Sphere.dirty = true;
		}
		if (gmesh->verticesDirty)
		{
			//glBindBuffer(GL_ARRAY_BUFFER,glmesh->m_VBO);
			//glBufferSubData(GL_ARRAY_BUFFER, 0, gmesh->getVertices().size()*sizeof(Cle::Gfx::Vertex), gmesh->getVertices().data());
			gmesh->verticesDirty = false;
			gmesh->m_local_AABB.updateToWorld(gmesh->getVertices(), glm::mat4(1.0f));
		}
		if (gmesh->indicesDirty)
		{
			//glBindBuffer(GL_ELEMENT_ARRAY_BUFFER,glmesh->m_EBO);
			//glBufferSubData(GL_ELEMENT_ARRAY_BUFFER,0, gmesh->getIndices().size() * sizeof(unsigned int), gmesh->getIndices().data());
			gmesh->indicesDirty = false;
			gmesh->m_local_AABB.updateToWorld(gmesh->getVertices(), glm::mat4(1.0f));
		}

	}

}
bool Cle::Renderer::IRenderer::isWithinFarPlane(entt::entity entity, Cle::Gfx::Camera& camera)
{
	if (!m_registry->valid(entity) ||  !m_registry->all_of<Cle::Components::Bounds, Cle::Components::Transform>(entity)) return false;
	auto& t = m_registry->get<Cle::Components::Transform>(entity);
	auto& b = m_registry->get<Cle::Components::Bounds>(entity);
	return glm::length2(camera.Position - t.getPosition()) < ((camera.far - b.sphere.radius) * (camera.far - b.sphere.radius));
}

void Cle::Renderer::IRenderer::uploadMesh(entt::entity e, std::shared_ptr<Cle::GenericMesh> mesh, entt::registry& registry)
{
	auto m = Cle::AssetHandler::getInstance().getOrMakeMesh(mesh->getModelPath(), mesh->getMeshIndex());
	registry.emplace_or_replace< std::shared_ptr<Cle::GenericMesh>>(e, m);
	auto& bounds = registry.emplace_or_replace<Components::Bounds>(e, m->m_local_AABB, m->m_local_Bounding_Sphere);

	m->verticesDirty = true;
	m->indicesDirty = true;
	m->m_local_AABB.updateToWorld(m->getVertices(), glm::mat4(1.0f));

}

void Cle::Renderer::IRenderer::onSceneLoaded()
{
	auto view = m_registry->view < std::shared_ptr<GenericMesh>, Cle::Components::Transform>();
	Cle::Gfx::Camera cam;

	view.each([&](const entt::entity entity, std::shared_ptr<GenericMesh>& mesh, Cle::Components::Transform& transform)
		{

			//material.m_Shader = getDefaultShader();

			const auto& ModelLoaded = Cle::AssetHandler::getInstance().LoadModel(mesh->getModelPath());

			uploadMesh(entity, ModelLoaded.at(mesh->getMeshIndex()), *m_registry);

			auto& nMesh = m_registry->get<std::shared_ptr<GenericMesh>>(entity);

			//if (nMesh->gMesh->texture) nMesh->gMesh->texture = clientAssetHandler.getOrMakeTexture(nMesh->gMesh->texture->getPath());
		//	if (material.getColorMap()) material.setColorMap(getOrMakeTexture(material.getColorMap()->getPath()));
			drawMesh(entity, *m_registry, cam);


		});
}