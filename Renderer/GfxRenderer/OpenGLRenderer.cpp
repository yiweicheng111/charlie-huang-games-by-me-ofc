#include "Camera.h"
#include "OPENGL4/OpenGLRenderer.h"
#include "OpenGL4/OpenGLMesh.h"
#include "Components/Transform.h"
#include "Mesh.h"
#include "Material.h"
#include <iostream>
#define materialColorMap 0
#define meshColorMap 1
#define lightMap 2


void Cle::OPENGL::Renderer::beginFrame()
{
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glfwPollEvents();
}
void Cle::OPENGL::Renderer::lightPass()
{
	auto& buffer = m_registry->ctx().get<LightBuffer>();
	buffer.updateVector();
	glUseProgram(buffer.m_Program);
	glUniform1i(glGetUniformLocation(buffer.m_Program, "totalLights"), buffer.LightVector.size());
	for (auto& e : m_registry->view<Cle::Components::LightComponent>())
	{
		auto& light = m_registry->get<Cle::Components::LightComponent>(e); 
		auto& transform = m_registry->get<Cle::Components::Transform>(e);
	
		light.setPosition(transform.getPosition());
		
		if (light.dirty) buffer.updateLight(light.ID, light.getLight());

		
	}
	glActiveTexture(GL_TEXTURE0+ lightMap);
	glBindTexture(GL_TEXTURE_2D, buffer.lightTexture);
	glUniform1i(glGetUniformLocation(buffer.m_Program, "lightTexture"), lightMap);

}
Cle::OPENGL::Renderer::Renderer(entt::registry* registry) : m_registry(registry) {
	glfwInit();
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 1);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
}
unsigned int Cle::OPENGL::Renderer::getDefaultShader()
{
	return programMap["MeshShader"];
}

void Cle::OPENGL::Renderer::clearFrame(GLFWwindow* window)
{
	glfwSwapBuffers(window);
}

void Cle::OPENGL::Renderer::clearColor(float r, float g, float b, float w)
{
	glClearColor(r, g, b, w);
}
void Cle::OPENGL::Renderer::uploadMesh(entt::entity e, entt::registry& registry)
{
	Cle::Gfx::GenericMesh mesh = registry.get<Cle::Gfx::GenericMesh>(e);
	registry.emplace<std::shared_ptr<Cle::OPENGL::Mesh>>(e, std::make_shared<Cle::OPENGL::Mesh>(mesh));
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
	[&] {
		std::vector<std::string> shaders = {"Shaders/light.frag","Shaders/default.frag"};
		Cle::OPENGL::Program program("Shaders/default.vert", shaders);
		programMap["MeshShader"] = program.ID;
		}();
	m_registry->ctx().emplace<LightBuffer>(programMap["MeshShader"]);
	m_registry->on_construct<Cle::Components::LightComponent>().connect<&onlightadded>();
	m_registry->on_destroy<Cle::Components::LightComponent>().connect<&onlightremoved>();
}


void Cle::OPENGL::Renderer::UniformCamMatrix(Cle::Gfx::Camera& camera, Cle::Gfx::Material& material) {
	glUseProgram(material.m_Shader.programID);
	glUniformMatrix4fv(glGetUniformLocation(material.m_Shader.programID, "camMatrix"), 1, GL_FALSE, glm::value_ptr(camera.getProjection() * camera.getViewMatrix()));
	glUniform3fv(glGetUniformLocation(material.m_Shader.programID, "camPos"),1,glm::value_ptr(camera.Position));

}

void Cle::OPENGL::Renderer::lightingPass()
{
	static GLuint pID = programMap["MeshShader"];
	auto& instance = Lighting::getInstance();
	auto& sunDir = instance.sunDirection;
	float ambient = instance.ambient;

	glUseProgram(pID);
	glUniform3fv(glGetUniformLocation(pID, "sunDirection"),1,glm::value_ptr(sunDir));
	glUniform1f(glGetUniformLocation(pID, "ambientAmount"), ambient);

}


void Cle::OPENGL::Renderer::cleanDirtyMesh(entt::entity entity)
{
	if (!m_registry->any_of< Cle::Gfx::GenericMesh>(entity) || !m_registry->get<std::shared_ptr<Cle::OPENGL::Mesh>>(entity)) return;
	Cle::Gfx::GenericMesh& gmesh = m_registry->get< Cle::Gfx::GenericMesh>(entity);

	std::shared_ptr<Cle::OPENGL::Mesh>& glmesh = m_registry->get<std::shared_ptr<Cle::OPENGL::Mesh >> (entity);
		
	if (gmesh.verticesDirty)
	{
		glBindBuffer(GL_ARRAY_BUFFER,glmesh->m_VBO);
		glBufferSubData(GL_ARRAY_BUFFER, 0, gmesh.getVertices().size()*sizeof(Cle::Gfx::Vertex), gmesh.getVertices().data());
		gmesh.verticesDirty = false;
		gmesh.m_local_AABB.updateToWorld(gmesh.getVertices(), glm::mat4(1.0f));
	}
	if (gmesh.indicesDirty)
	{
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER,glmesh->m_EBO);
		glBufferSubData(GL_ELEMENT_ARRAY_BUFFER,0, gmesh.getIndices().size() * sizeof(unsigned int), gmesh.getIndices().data());
		gmesh.indicesDirty = false;
		gmesh.m_local_AABB.updateToWorld(gmesh.getVertices(), glm::mat4(1.0f));
	}
		
}

int Cle::OPENGL::Renderer::assignLOD(entt::entity entity, glm::vec3 viewPosition)
{
	auto& mesh = m_registry->get<std::shared_ptr<Cle::OPENGL::Mesh>>(entity);
	float distance = glm::length(viewPosition - m_registry->get<Cle::Components::Transform>(entity).getPosition());
	int lodsmesh = mesh->LODIndicesEBOMap.size();
	return mesh->m_EBO;
	if (lodsmesh < 3)
	{
		return mesh->m_EBO;
	}
	if (distance < 30) return mesh->m_EBO;
	if (distance < 70) return  std::next(mesh->LODIndicesEBOMap.begin())->first;
	else return  std::next(std::next(mesh->LODIndicesEBOMap.begin()))->first;

}

std::shared_ptr<Cle::Gfx::ITexture> Cle::OPENGL::Renderer::createTexture(std::string path)
{
	if (m_AssetHandler.textureCache.contains(path))
	{
		return m_AssetHandler.textureCache[path];
	}

	m_AssetHandler.textureCache[path] = std::make_shared<Cle::OPENGL::Texture>(path);
	return m_AssetHandler.textureCache[path];

}

void Cle::OPENGL::Renderer::drawMesh(entt::entity e, entt::registry& registry, Cle::Gfx::Camera& camera)
{
	if (!registry.all_of<Cle::Gfx::Material, Cle::Components::Transform, std::shared_ptr<Cle::OPENGL::Mesh>>(e)) {
		 return;		
	}
	cleanDirtyMesh(e);
	auto& material = registry.get<Cle::Gfx::Material>(e);
	auto& mesh = registry.get<std::shared_ptr<Cle::OPENGL::Mesh>>(e);
	auto& gmesh = registry.get<Cle::Gfx::GenericMesh>(e);

	auto& transform = registry.get<Cle::Components::Transform>(e);

	if (transform.dirty)
	{
		gmesh.m_Bounding_Sphere.dirty = true;
		gmesh.m_AABB.dirty = true;
	}


	glUseProgram(material.m_Shader.programID);
	glBindVertexArray(mesh->m_VAO);
	material.m_Shader.setVec3("color", material.getColor());

	if (material.dirty)
	{
		material.dirty = false;
	} 


	glm::mat4& model = transform.model;
	
	if (transform.dirty)
	{
		gmesh.m_AABB.dirty = true;
		gmesh.m_Bounding_Sphere.dirty = true;
		transform.computeMatrix();
	}
		
	material.m_Shader.setMat4("model", model);
	material.m_Shader.setVec3("vPosition", transform.getPosition());
	transform.dirty = false;
	lightPass();

	
	if (gmesh.texture && gmesh.texture->getID() !=-1) {
		glActiveTexture(GL_TEXTURE0+meshColorMap);

		glBindTexture(GL_TEXTURE_2D, gmesh.texture->getID());
		material.m_Shader.setInt("usesMeshColorMap", (int)(true));
		material.m_Shader.setInt("meshColorMap", meshColorMap);
	}
	else {
		material.m_Shader.setInt("usesMeshColorMap", (int)(false));
	}

	if (material.usesColorMap) {

		glActiveTexture(GL_TEXTURE0 + materialColorMap);
		glBindTexture(GL_TEXTURE_2D, material.getColorMap().getID());
		material.m_Shader.setInt("usesMaterialColorMap", (int)(true));
		material.m_Shader.setInt("materialColorMap", materialColorMap);
	}
	else {
		material.m_Shader.setInt("usesMaterialColorMap", (int)(false));
	}
	GLuint nebo = assignLOD(e, camera.Position);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, nebo);
	mesh->draw();
}

void Cle::OPENGL::Renderer::clear()
{
	programMap.clear();
}
