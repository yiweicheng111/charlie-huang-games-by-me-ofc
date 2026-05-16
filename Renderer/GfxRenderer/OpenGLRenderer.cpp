#include "Camera.h"
#include "OPENGL4/OpenGLRenderer.h"
#include "OpenGL4/OpenGLMesh.h"
#include "Components/Transform.h"
#include "Mesh.h"
#include "Material.h"
#include <iostream>
void Cle::OPENGL::Renderer::beginFrame()
{
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glfwPollEvents();
}
void Cle::OPENGL::Renderer::lightPass()
{
	auto& buffer = m_registry->ctx().get<LightBuffer>();
	glUseProgram(buffer.m_Program);
	for (auto& e : m_registry->view<Cle::Components::LightComponent>())
	{
		auto& light = m_registry->get<Cle::Components::LightComponent>(e).m_Light; 
		if (m_registry->any_of<Cle::Components::TreeInfo>(e))
		{
			auto& parent = m_registry->get<Cle::Components::TreeInfo>(e).parent;
			if (parent != entt::null)
			{
				light.position = m_registry->get<Cle::Components::Transform>(parent).getPosition();
				buffer.updateLight(m_registry->get<Cle::Components::LightComponent>(e).ID, m_registry->get<Cle::Components::LightComponent>(e).m_Light);
			}
		}
	}
	buffer.updateVector();
	glActiveTexture(GL_TEXTURE1);
	glBindTexture(GL_TEXTURE_2D, buffer.lightTexture);
	glUniform1i(glGetUniformLocation(buffer.m_Program, "lightTexture"), 1);
	
}
Cle::OPENGL::Renderer::Renderer(entt::registry* registry) : m_registry(registry) {
	glfwInit();
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 1);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
}
Cle::Gfx::Material Cle::OPENGL::Renderer::getMaterial()
{
	return Cle::Gfx::Material(programMap["MeshShader"]);
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
	lightc.ID = lightbuffer.addLight(lightc.m_Light);
}
void Cle::OPENGL::Renderer::setSettings()
{
	glEnable(GL_DEPTH_TEST);
	glEnable(GL_CULL_FACE);
	[&] {
		std::vector<std::string> shaders = { "Shaders/default.frag" };
		Cle::OPENGL::Program program("Shaders/default.vert", shaders);
		programMap["MeshShader"] = program.ID;
		}();
	m_registry->ctx().emplace<LightBuffer>(programMap["MeshShader"]);
	m_registry->on_construct<Cle::Components::LightComponent>().connect<&onlightadded>();
}


void Cle::OPENGL::Renderer::UniformCamMatrix(Cle::Gfx::Camera& camera, Cle::Gfx::Material& material) {
	glUseProgram(material.m_Shader.programID);
	glUniformMatrix4fv(glGetUniformLocation(material.m_Shader.programID, "camMatrix"), 1, GL_FALSE, glm::value_ptr(camera.getProjection() * camera.getViewMatrix()));
}

void Cle::OPENGL::Renderer::drawMesh(entt::entity e, entt::registry& registry)
{
	auto material = registry.get<Cle::Gfx::Material>(e);
	auto mesh = registry.get<std::shared_ptr<Cle::OPENGL::Mesh>>(e);
	auto& transform = registry.get<Cle::Components::Transform>(e);
	transform.computeMatrix();

	glUseProgram(material.m_Shader.programID);
	glBindVertexArray(mesh->m_VAO);

	material.m_Shader.setVec3("color", material.Color);
	glm::mat4 model = transform.model;
	material.m_Shader.setMat4("model", model);
	if (material.usesColorMap) {
		glBindTexture(GL_TEXTURE_2D, material.colorMap.ID);
		glActiveTexture(GL_TEXTURE0);
		material.m_Shader.setInt("usesColorMap", (int)(true));
		material.m_Shader.setInt("colorMap", 0);
	}
	else {
		material.m_Shader.setInt("usesColorMap", (int)(false));

	}


	mesh->draw();
}