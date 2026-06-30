#include "Camera.h"
#include "OPENGL4/OpenGLRenderer.h"
#include "OpenGL4/OpenGLMesh.h"
#include "Components/Transform.h"
#include "Mesh.h"
#include "Material.h"
#include "CharlieCore/AssetHandler.h"
#include <iostream>
#include <glm/gtx/norm.hpp>
#define materialColorMap 0
#define meshColorMap 1
#define lightMap 2
#define skyboxSlot 

using namespace Cle;
void Cle::OPENGL::Renderer::beginFrame()
{
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glfwPollEvents();
	auto& lighting = Lighting::getInstance();

	glClearColor(lighting.backgroundColor.x, lighting.backgroundColor.y, lighting.backgroundColor.z, 1.0f);
}
void Cle::OPENGL::Renderer::lightPass()
{
	auto& buffer = m_registry->ctx().get<LightBuffer>();
	buffer.updateVector();
	buffer.shader->Bind();
	buffer.shader->setInt("totalLights", buffer.LightVector.size());
	for (auto& e : m_registry->view<Cle::Components::LightComponent>())
	{
		auto& light = m_registry->get<Cle::Components::LightComponent>(e); 
		auto& transform = m_registry->get<Cle::Components::Transform>(e);
	
		light.setPosition(transform.getPosition());
		
		if (light.dirty) buffer.updateLight(light.ID, light.getLight());

		
	}


	buffer.shader->UniformLights(buffer.lightTexture,lightMap);


}
Cle::OPENGL::Renderer::Renderer(entt::registry* registry) : m_registry(registry) {
	glfwInit();
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 1);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
}
std::shared_ptr<Cle::IShader> Cle::OPENGL::Renderer::getDefaultShader()
{
	return shaderCache["MeshShader"];
}

void Cle::OPENGL::Renderer::clearFrame(GLFWwindow* window)
{
	glfwSwapBuffers(window);
}

void Cle::OPENGL::Renderer::clearColor(float r, float g, float b, float w)
{
	glClearColor(r, g, b, w);
}
void Cle::OPENGL::Renderer::uploadMesh(entt::entity e, std::shared_ptr<Cle::GenericMesh> mesh, entt::registry& registry)
{
	auto m = Cle::AssetHandler::getInstance().getOrMakeMesh(mesh->getModelPath(), mesh->getMeshIndex());
	registry.emplace_or_replace< std::shared_ptr<Cle::GenericMesh>>(e, m);
	auto& bounds = registry.emplace_or_replace<Components::Bounds>(e, m->m_local_AABB,m->m_local_Bounding_Sphere);

	m->verticesDirty = true;
	m->indicesDirty = true;
	m->m_local_AABB.updateToWorld(m->getVertices(), glm::mat4(1.0f));




}
static void onlightadded(entt::registry& registry,entt::entity e)
{	
	auto& lightbuffer = registry.ctx().get<LightBuffer>();
	auto& lightc = registry.get<Cle::Components::LightComponent>(e);
	lightc.ID = lightbuffer.addLight(lightc.getLight());

}
static void onlightremoved(entt::registry& registry, entt::entity e)
{
	auto& lightc = registry.get<Cle::Components::LightComponent>(e);
	auto& lightbuffer = registry.ctx().get<LightBuffer>();
	lightbuffer.removeLight(lightc.ID);

}
void Cle::OPENGL::Renderer::setSettings()
{
	glEnable(GL_DEPTH_TEST);
	glEnable(GL_CULL_FACE);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	glEnable(GL_BLEND);
	glfwSwapInterval(1);
	[&] {
		std::vector<std::string> shaders = {"Shaders/light.frag","Shaders/fog.frag","Shaders/default.frag"};
		Cle::OPENGL::Program program("Shaders/default.vert", shaders);
		shaderCache["MeshShader"] = std::make_shared<OPENGL::Shader>(program.ID);
		}();
	m_registry->ctx().emplace<LightBuffer>(std::static_pointer_cast<OPENGL::Shader>(shaderCache["MeshShader"]));
	m_registry->on_construct<Cle::Components::LightComponent>().connect<&onlightadded>();
	m_registry->on_destroy<Cle::Components::LightComponent>().connect<&onlightremoved>();
	m_registry->ctx().emplace<Cle::Lighting*>(&Lighting::getInstance());

}


void Cle::OPENGL::Renderer::UniformCamMatrix(Cle::Gfx::Camera& camera, std::shared_ptr<Cle::IShader> shader) {
	shader->Bind();
	shader->setMat4("camMatrix", camera.getProjection() * camera.getViewMatrix());
	shader->setVec3("camPos", camera.Position);
	auto& instance = m_registry->ctx().get<Cle::Lighting*>();
	shader->setFloat("fogStart", instance->fogStart);
	shader->setFloat("fogEnd", std::min({ instance->fogEnd,camera.far }));
}

void Cle::OPENGL::Renderer::lightingPass()
{
	static auto shader = shaderCache["MeshShader"];
	auto& instance = m_registry->ctx().get<Cle::Lighting*>();
	auto& sunDir = instance->sunDirection;
	float ambient = instance->ambient;
	auto& backgroundColor = instance->backgroundColor;
	auto& ambientColor = instance->ambientColor;

	shader->Bind();
	shader->setVec3("sunDirection", sunDir);
	shader->setVec3("ambientColor", ambientColor);
	shader->setVec3("backgroundColor", backgroundColor);
	shader->setFloat("ambientAmount", ambient);
	shader->setFloat("fogStart", instance->fogStart);


}


void Cle::OPENGL::Renderer::cleanDirtyMesh(entt::entity entity)
{

	auto& gmesh = m_registry->get<std::shared_ptr<GenericMesh>>(entity);

	const std::shared_ptr<Cle::OPENGL::Mesh>& glmesh = std::static_pointer_cast<Cle::OPENGL::Mesh>(getOrMakeMesh(gmesh));
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

void Cle::OPENGL::Renderer::onSceneLoaded()
{
	auto view = m_registry->view < std::shared_ptr<GenericMesh>, Cle::Components::Transform> ();
	Cle::Gfx::Camera cam;

	view.each([&](const entt::entity entity, std::shared_ptr<GenericMesh>& mesh, Cle::Components::Transform& transform)
		{

				m_registry->emplace_or_replace < std::shared_ptr<OPENGL::Shader>>(entity, std::static_pointer_cast<OPENGL::Shader>(getDefaultShader()));
				//material.m_Shader = getDefaultShader();

				const auto& ModelLoaded = Cle::AssetHandler::getInstance().LoadModel(mesh->getModelPath());

				uploadMesh(entity, ModelLoaded.at(mesh->getMeshIndex()), *m_registry);

				auto& nMesh = m_registry->get<std::shared_ptr<GenericMesh>>(entity);

				//if (nMesh->gMesh->texture) nMesh->gMesh->texture = clientAssetHandler.getOrMakeTexture(nMesh->gMesh->texture->getPath());
			//	if (material.getColorMap()) material.setColorMap(getOrMakeTexture(material.getColorMap()->getPath()));
				drawMesh(entity, *m_registry,cam);


		});
}

std::shared_ptr<Cle::Gfx::IMesh> Cle::OPENGL::Renderer::assignLOD(entt::entity entity, glm::vec3 viewPosition)
{
	if (!m_registry->any_of<std::shared_ptr<GenericMesh>>(entity)) return 0;

	auto gmesh = m_registry->get < std::shared_ptr<GenericMesh>>(entity);
	auto imesh = getOrMakeMesh(gmesh);
	std::shared_ptr<Cle::OPENGL::Mesh> openglmesh = std::static_pointer_cast<Cle::OPENGL::Mesh>(imesh);

	float distance = glm::length2(viewPosition - m_registry->get<Cle::Components::Transform>(entity).getPosition());
	const auto lodlist = openglmesh->getLodMesh();
	if (lodlist.size() < 3)
	{
		return openglmesh;
	}
	else if (distance < 50) return openglmesh;
	else if (distance < 100) return openglmesh->getLodMesh(1);
	else   return openglmesh->getLodMesh(2);

	/*auto lodmesh = std::static_pointer_cast<Cle::OPENGL::Mesh>(getOrMakeMesh(std::make_shared<Cle::GenericMesh>(gmesh->getVertices(), imesh->LODIndicesEBOMap[ebo])));

	lodmesh->m_EBO = openglmesh->m_EBO;
	lodmesh->m_VAO = openglmesh->m_VAO;
	lodmesh->m_VBO = openglmesh->m_VBO;*/

	return openglmesh;

}

std::shared_ptr<Cle::Gfx::ITexture> Cle::OPENGL::Renderer::phraseSkybox(std::vector<std::string> skybox)
{
	unsigned int cubetex;
	glGenTextures(1, &cubetex);
	glBindTexture(GL_TEXTURE_CUBE_MAP, cubetex);

	for (int i = 0; i < 6; i++)
	{
		int width, height, channels;
		unsigned char* bytes = stbi_load(skybox[i].c_str(), &width, &height, &channels, 0);
		auto format = channels == 3 ? GL_RGB : GL_RGBA;
		glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, format, width, height, 0, format,GL_UNSIGNED_BYTE, bytes);
		stbi_image_free(bytes);
	}

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);

	return std::make_shared<Cle::OPENGL::Texture>(cubetex);
}


void Cle::OPENGL::Renderer::drawMesh(entt::entity e, entt::registry& registry, Cle::Gfx::Camera& camera)
{
	if (!registry.any_of < std::shared_ptr<IShader>>(e))
	{
		registry.emplace_or_replace<std::shared_ptr<Cle::OPENGL::Shader>>(e, std::static_pointer_cast<Cle::OPENGL::Shader>(getDefaultShader()));
	}
	
	if (!registry.all_of<Cle::Components::Transform, std::shared_ptr<GenericMesh>, std::shared_ptr<Cle::OPENGL::Shader>,Components::Bounds>(e)) {
		 return;		
	}
	cleanDirtyMesh(e);
	//Cle::Components::MaterialRef, 
//	auto& material = registry.get<Cle::Components::MaterialRef>(e);

	auto& shader = registry.get < std::shared_ptr < Cle::OPENGL::Shader >> (e);
	auto& gmesh = registry.get<std::shared_ptr<GenericMesh>>(e);
	auto& bounds = registry.get<Components::Bounds>(e);

	if (!gmesh->gpuUploaded)
	{
		getOrMakeMesh(gmesh);
	}
	
	auto mesh = std::static_pointer_cast<Cle::OPENGL::Mesh>(getOrMakeMesh(gmesh));

	auto& transform = registry.get<Cle::Components::Transform>(e);


	if (m_registry->any_of<Cle::Components::Color>(e))
	{
		shader->setVec3("color", glm::vec3(m_registry->get<Cle::Components::Color>(e).value));

	}
	else
	{
		shader->setVec3("color", glm::vec3(1,1,1));

	}
	shader->Bind();

	glm::mat4& model = transform.model;
	
	if (transform.dirty)
	{
		bounds.aabb.dirty = true;
		bounds.sphere.dirty = true;
		transform.computeMatrix();
	}
		
	shader->setMat4("model", model);
	shader->setVec3("vPosition", transform.getPosition());
/*if (gmesh->m_AABB.dirty)
	{
		gmesh->m_local_AABB.updateToWorld(gmesh->getVertices(), transform.model);
		gmesh->m_AABB.dirty = false;
	}*/	

	
	/*if (registry.any_of < std::shared_ptr<Cle::Gfx::ITexture>>(e)) {
		auto& tex = registry.get < std::shared_ptr<Cle::Gfx::ITexture>>(e);
		if (!tex->gpuUploaded)
		{
			tex = std::make_shared<Cle::OPENGL::Texture>(tex->getPath());
		}
		glActiveTexture(GL_TEXTURE0 + meshColorMap);
		glBindTexture(GL_TEXTURE_2D, tex->getID());

		shader->setInt("usesMeshColorMap", (int)(true));
		shader->setInt("meshColorMap", meshColorMap);

	}
	else {
		shader->setInt("usesMeshColorMap", (int)(false));
	}*/
	shader->setInt("usesMeshColorMap", (int)(false));

	//const auto gpumaterial = getOrMakeMaterial(registry.get<Cle::Components::MaterialRef>(e));

	/*if (gpumaterial->usesColorMap) {

		glActiveTexture(GL_TEXTURE0 + materialColorMap);
		glBindTexture(GL_TEXTURE_2D, gpumaterial->getColorMap()->getID());
		shader->setInt("usesMaterialColorMap", (int)(true));
		shader->setInt("materialColorMap", materialColorMap);
	}
	else {
	}*/
	shader->setInt("usesMaterialColorMap", (int)(false));

	assignLOD(e, camera.Position)->draw();
}

void Cle::OPENGL::Renderer::clear()
{
	//client.clear();
}

void Cle::OPENGL::Renderer::passSkybox()
{
	glDepthFunc(GL_LEQUAL);
	glDepthFunc(GL_LESS);
}
