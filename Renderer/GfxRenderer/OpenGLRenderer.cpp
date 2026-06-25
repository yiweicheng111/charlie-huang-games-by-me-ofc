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
#define skyboxSlot 


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
	//glBindTexture(GL_TEXTURE_2D, buffer.lightTexture);
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
void Cle::OPENGL::Renderer::uploadMesh(entt::entity e,const Cle::Gfx::GenericMesh& mesh, entt::registry& registry)
{

	std::shared_ptr<Cle::Gfx::IMesh> imesh = std::make_shared< Cle::OPENGL::Mesh>(mesh);
	imesh->gpuUploaded = true;

	registry.emplace_or_replace<std::shared_ptr<Cle::Gfx::IMesh>>(e, imesh);
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
	auto& backgroundColor = instance.backgroundColor;
	auto& ambientColor = instance.ambientColor;

	glUseProgram(pID);
	glUniform3fv(glGetUniformLocation(pID, "sunDirection"),1,glm::value_ptr(sunDir));
	glUniform1f(glGetUniformLocation(pID, "ambientAmount"), ambient);
	glUniform3f(glGetUniformLocation(pID, "ambientColor"), ambientColor.x, ambientColor.y, ambientColor.z);
	glUniform1f(glGetUniformLocation(pID, "fogStart"), instance.fogStart);
	glUniform1f(glGetUniformLocation(pID, "fogEnd"), instance.fogEnd);
	glUniform3f(glGetUniformLocation(pID, "backgroundColor"), backgroundColor.x, backgroundColor.y, backgroundColor.z);
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

void Cle::OPENGL::Renderer::onSceneLoaded()
{
	auto view = m_registry->view<std::shared_ptr<Cle::Gfx::IMesh>, Cle::Gfx::Material, Cle::Components::Transform>();
	Cle::Gfx::Camera cam;

	view.each([&](const entt::entity entity, std::shared_ptr<Cle::Gfx::IMesh>& mesh, Cle::Gfx::Material& material, Cle::Components::Transform& transform)
		{

				material.m_Shader = getDefaultShader();

				const auto& ModelLoaded =m_AssetHandler.LoadModel(mesh->gMesh.ModelPath);

				uploadMesh(entity, ModelLoaded.at(mesh->gMesh.loadedMeshIndex), *m_registry);
				auto& nMesh = m_registry->get<std::shared_ptr<Cle::Gfx::IMesh>>(entity);


				if (nMesh->gMesh.texture) nMesh->gMesh.texture = createTexture(nMesh->gMesh.texture->getPath());
				if (material.getColorMap()) material.setColorMap(createTexture(material.getColorMap()->getPath()));
				drawMesh(entity, *m_registry,cam);


		});
}

int Cle::OPENGL::Renderer::assignLOD(entt::entity entity, glm::vec3 viewPosition)
{
	auto& imesh = m_registry->get<std::shared_ptr<Cle::Gfx::IMesh>>(entity);
	std::shared_ptr<Cle::OPENGL::Mesh> openglmesh = dynamic_pointer_cast<Cle::OPENGL::Mesh>(imesh);
	float distance = glm::length(viewPosition - m_registry->get<Cle::Components::Transform>(entity).getPosition());
	int lodsmesh = imesh->LODIndicesEBOMap.size();
	return openglmesh->m_EBO;

	if (lodsmesh < 3)
	{
		return openglmesh->m_EBO;
	}
	if (distance < 30) return  openglmesh->m_EBO;
	if (distance < 70) return  std::next(openglmesh->LODIndicesEBOMap.begin())->first;
	else return  std::next(std::next(openglmesh->LODIndicesEBOMap.begin()))->first;

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

std::shared_ptr<Cle::Gfx::ITexture> Cle::OPENGL::Renderer::createTexture(std::string path)
{

	if (m_AssetHandler.textureCache.contains(path))
	{
		if (m_AssetHandler.textureCache[path]->loaded == false) return nullptr;
		m_AssetHandler.textureCache[path]->gpuUploaded = true;

		return m_AssetHandler.textureCache[path];
	}
	m_AssetHandler.textureCache[path]->gpuUploaded = true;

	m_AssetHandler.textureCache[path] = std::make_shared<Cle::OPENGL::Texture>(path);
	return m_AssetHandler.textureCache[path];

}

void Cle::OPENGL::Renderer::drawMesh(entt::entity e, entt::registry& registry, Cle::Gfx::Camera& camera)
{

	if (!registry.all_of<Cle::Gfx::Material, Cle::Components::Transform, std::shared_ptr<Cle::Gfx::IMesh>>(e)) {
		 return;		
	}
	cleanDirtyMesh(e);


	auto& material = registry.get<Cle::Gfx::Material>(e);

	auto& mesh = registry.get<std::shared_ptr<Cle::Gfx::IMesh>>(e);
	auto& gmesh = mesh->gMesh;

	auto& transform = registry.get<Cle::Components::Transform>(e);

	if (transform.dirty)
	{
		gmesh.m_Bounding_Sphere.dirty = true;
		gmesh.m_AABB.dirty = true;
	}

	glUseProgram(material.m_Shader.programID);
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

	
	if (gmesh.texture) {
		glActiveTexture(GL_TEXTURE0 + meshColorMap);

		material.m_Shader.setInt("usesMeshColorMap", (int)(true));
		material.m_Shader.setInt("meshColorMap", meshColorMap);

	}
	else {
		material.m_Shader.setInt("usesMeshColorMap", (int)(false));
	}

	if (material.usesColorMap) {

		glActiveTexture(GL_TEXTURE0 + materialColorMap);
		glBindTexture(GL_TEXTURE_2D, material.getColorMap()->getID());
		material.m_Shader.setInt("usesMaterialColorMap", (int)(true));
		material.m_Shader.setInt("materialColorMap", materialColorMap);
	}
	else {
		material.m_Shader.setInt("usesMaterialColorMap", (int)(false));
	}

	GLuint nebo = assignLOD(e, camera.Position);

	mesh->draw();
}

void Cle::OPENGL::Renderer::clear()
{
	programMap.clear();
}

void Cle::OPENGL::Renderer::passSkybox()
{
	glDepthFunc(GL_LEQUAL);
	glDepthFunc(GL_LESS);
}
