#include "Camera.h"
#include "OPENGL4/OpenGLRenderer.h"
#include "Components/Transform.h"
#include "Mesh.h"
#include "Material.h"
#include <iostream>
void Cle::OPENGL43::Renderer::beginFrame()
{
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glfwPollEvents();
}
Cle::OPENGL43::Renderer::Renderer() {
	glfwInit();
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 1);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
}
Cle::Gfx::Material Cle::OPENGL43::Renderer::getMaterial()
{
	return Cle::Gfx::Material(programMap["MeshShader"]);
}
void Cle::OPENGL43::Renderer::clearFrame(GLFWwindow * window)
{
	glfwSwapBuffers(window);
}

void Cle::OPENGL43::Renderer::clearColor(float r, float g, float b, float w)
{
	glClearColor(r, g, b, w);
}
void Cle::OPENGL43::Renderer::uploadMesh(entt::entity e, entt::registry& registry)
{
	Cle::Gfx::GenericMesh& mesh = registry.get<Cle::Gfx::GenericMesh>(e);
	registry.emplace<std::shared_ptr<Cle::Gfx::OPENGL43::Mesh>>(e,std::make_shared<Cle::Gfx::OPENGL43::Mesh>(mesh));
}

void Cle::OPENGL43::Renderer::setSettings()
{
	glEnable(GL_DEPTH_TEST);
	glEnable(GL_CULL_FACE);
	[&] {
		std::vector<std::string> shaders = { "default.frag" };
		Cle::Gfx::OPENGL43::Program program("default.vert", shaders);
		programMap["MeshShader"] = program.ID;
		}();
}


void Cle::OPENGL43::Renderer::UniformCamMatrix(Cle::Gfx::Camera& camera, Cle::Gfx::Material& material) {
	glUseProgram(material.m_Shader.programID);
	glUniformMatrix4fv(glGetUniformLocation(material.m_Shader.programID, "camMatrix"), 1, GL_FALSE, glm::value_ptr(camera.getProjection()*camera.getViewMatrix()));
}

void Cle::OPENGL43::Renderer::drawMesh(entt::entity e, entt::registry& registry)
{
	auto material = registry.get<Cle::Gfx::Material>(e);
	auto mesh = registry.get<std::shared_ptr<Gfx::OPENGL43::Mesh>>(e);
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
