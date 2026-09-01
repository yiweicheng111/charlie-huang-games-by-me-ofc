#define GLM_ENABLE_EXPERIMENTAL

#include "Camera.h"
#include "OPENGL4/OpenGLRenderer.h"
#include "OpenGL4/OpenGLMesh.h"
#include "CharlieEngine/Transform.h"
#include "Mesh.h"
#include "Material.h"
#include "CharlieEngine/AssetHandler.h"
#include <iostream>
#include <glm/gtx/string_cast.hpp> 

#include <glm/gtx/norm.hpp>
#define materialColorMap 0
#define meshColorMap 1
#define lightMap 2
#define skyboxSlot 3
#define shadowMapSlot 4
using namespace Cle;
static Cle::Gfx::Camera g_camera;
void Cle::OPENGL::Renderer::beginFrame()
{
	glfwPollEvents();
	auto& lighting = Lighting::getInstance();

	glClearColor(lighting.backgroundColor.x, lighting.backgroundColor.y, lighting.backgroundColor.z, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

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


	buffer.shader->UniformLights(buffer.lightTexture, lightMap);


}
static GLuint shadowFBO, shadowMap;

static float shadowWidth = 4048;
static float shadowHeight = 4048;
static float orthoHeight = 100.0f;
static float orthoWidth = 100.0f;
void Cle::OPENGL::Renderer::drawRegistry(Cle::Gfx::Camera& m_camera, GLFWwindow* window)
{
	g_camera = m_camera;
	using namespace Cle::Components;
	
	auto& lightingInstance = Cle::Lighting::getInstance();
	
	auto drawShadow = [&]()
		{

			static bool binded = false;
			if (!binded)
			{
				glGenFramebuffers(1, &shadowFBO);
				glGenTextures(1, &shadowMap);
				glBindTexture(GL_TEXTURE_2D, shadowMap);

				glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT32F, shadowWidth, shadowHeight, 0, GL_DEPTH_COMPONENT, GL_FLOAT, nullptr);
				glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
				glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
				glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
				glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);
				
				float borderColor[] = { 1.0f, 1.0f, 1.0f, 1.0f };
				glTexParameterfv(GL_TEXTURE_2D, GL_TEXTURE_BORDER_COLOR, borderColor);

				glBindFramebuffer(GL_FRAMEBUFFER, shadowFBO);
				glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, shadowMap, 0);

				glDrawBuffer(GL_NONE);
				glReadBuffer(GL_NONE);
				glBindFramebuffer(GL_FRAMEBUFFER, 0);

				binded = true;
			}
		};
	drawShadow();
	
	auto drawSceneDepth = [&](const glm::mat4& lightSpaceMatrix)
		{
			glCullFace(GL_FRONT);
			glViewport(0, 0, shadowWidth, shadowHeight);
			glBindFramebuffer(GL_FRAMEBUFFER, shadowFBO);
			glClear(GL_DEPTH_BUFFER_BIT);
			auto& depthShader = shaderCache["DepthShader"];
			depthShader->Bind();
			depthShader->setMat4("lightSpaceMatrix", lightSpaceMatrix);


			auto view = m_registry->view<std::shared_ptr<GenericMesh>, Transform>();
			view.each([&](auto entity, std::shared_ptr<GenericMesh>& mesh, Transform& transform)
				{
					if (isWithinFarPlane(entity, m_camera))
					{
						depthShader->setMat4("model", transform.getRelativeModel(m_camera));
						auto gpuMesh = std::static_pointer_cast<Cle::OPENGL::Mesh>(getOrMakeMesh(mesh));
						gpuMesh->draw();
					}
				
				});
			glCullFace(GL_BACK);
		};
	auto drawSkybox = [&]()
		{

			static auto cubemapTexture = phraseSkybox(lightingInstance.skybox.faces)->getID();

			static Cle::OPENGL::VBO skyboxVBO(cube);
			static Cle::OPENGL::VAO skyboxVAO(skyboxVBO.ID);
			static auto& shader = shaderCache["SkyboxShader"];
			static bool binded = false;
			if (!binded)
			{
				skyboxVAO.LinkAttrib(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), 0);
				binded = true;
			}
			glDepthMask(GL_FALSE);
			shader->Bind();
			glBindVertexArray(skyboxVAO.ID);
			if (lightingInstance.usesSkybox)
			{
				shader->setInt("usesSkybox", 1);
				shader->setInt("skybox", skyboxSlot);
				glActiveTexture(GL_TEXTURE0 + skyboxSlot);
				glBindTexture(GL_TEXTURE_CUBE_MAP, cubemapTexture);
			}
			else
			{
				shader->setInt("usesSkybox", 0);
			}

			shader->setMat4("projection", m_camera.getProjection());
			glm::mat4 skyboxView = glm::mat4(glm::mat3(m_camera.getViewMatrix()));
			shader->setMat4("view", skyboxView);
			shader->setVec3("clearColor", lightingInstance.backgroundColor);
			shader->setVec3("sunDir", lightingInstance.sunDirection);
			shader->setVec3("skyBottom", lightingInstance.skyBottom);
			shader->setVec3("skyTop", lightingInstance.skyTop);

			glDrawArrays(GL_TRIANGLES, 0, 36);

			glDepthMask(GL_TRUE);
		};
	auto drawScene = [&]()
		{
			
			lightingPass();

			lightPass();

			UniformCamMatrix(m_camera, getDefaultShader());

			//renderer->SyncMeshes(registry);

		//	if (width == 0 || height == 0) return;
			int totalMeshes = 0;
			int totalDrawn = 0;
			Frustum frustum = Frustum::createFrustumInCamera(m_camera);
			auto view = m_registry->view<std::shared_ptr<GenericMesh>, Transform, Bounds>();

			view.each([&](auto entity, std::shared_ptr<GenericMesh>& mesh, Transform& transform, Bounds& bounds) {
				//auto globalSphere = bounds.sphere;
				if (transform.dirty)
				{
					bounds.aabb.dirty = true;
					bounds.sphere.dirty = true;
					transform.computeMatrix();


				}
				totalMeshes++;
				if (bounds.sphere.isOnFrustum(frustum, transform.model) && isWithinFarPlane(entity, m_camera)) {
					drawMesh(entity, *m_registry, m_camera);

					totalDrawn++;

				}

				});

		};
	//static int width, height;
	//glfwGetWindowSize(window, &width, &height);




	glm::mat4 lightProjection = glm::ortho(-orthoWidth, orthoWidth, -orthoHeight, orthoHeight, 0.1f, 1000.0f);
	glm::vec3 sceneCenter = glm::vec3(0.0f); //glm::vec3(-274, 28, -146);
	glm::vec3 lightEye = sceneCenter - lightingInstance.sunDirection * 100.0f;

	
	glm::vec3 upVector = (glm::abs(glm::dot(lightingInstance.sunDirection, glm::vec3(0, 1, 0))) > 0.99f)
		? glm::vec3(0, 0, 1)
		: glm::vec3(0, 1, 0);

	glm::mat4 lightView = glm::lookAt(lightEye, sceneCenter, upVector);

	glm::mat4 lightSpaceMatrix = lightProjection * lightView;

	


	drawSceneDepth(lightSpaceMatrix);






	glBindFramebuffer(GL_FRAMEBUFFER, sceneFBO);
	glViewport(0, 0, resolutionWidth, resolutionHeight);

	beginFrame();

	shaderCache["MeshShader"]->Bind();

	shaderCache["MeshShader"]->setMat4("lightSpaceMatrix", lightSpaceMatrix);
	shaderCache["MeshShader"]->setVec3("sunDirection", lightingInstance.sunDirection);
	shaderCache["MeshShader"]->setFloat("ambientAmount", 0.3f);
	shaderCache["MeshShader"]->setVec3("camPos", m_camera.Position);

	UniformCamMatrix(m_camera, shaderCache["MeshShader"]);

	drawSkybox();
	drawScene();
	glfwGetWindowSize(window, &width, &height);

	glBindFramebuffer(GL_READ_FRAMEBUFFER, sceneFBO);
	glBindFramebuffer(GL_DRAW_FRAMEBUFFER, 0);
	glBlitFramebuffer(0, 0, resolutionWidth, resolutionHeight, 0, 0, width, height, GL_COLOR_BUFFER_BIT, GL_LINEAR);
	glBindFramebuffer(GL_FRAMEBUFFER, 0);

	//glViewport(0, 0, width, height);
	//beginFrame();

//	drawScene();
	//std::cout << "Meshes: " << totalMeshes << " Drawn: " << totalDrawn << std::endl;
}
Cle::OPENGL::Renderer::Renderer(entt::registry* registry) {
	m_registry = registry;
	glfwInit();
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 1);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);


}
std::shared_ptr<Cle::Gfx::IMesh> Cle::OPENGL::Renderer::getOrMakeMesh(std::shared_ptr<Cle::GenericMesh> mesh)
{

	if (!gpuMeshCache.contains(mesh))
	{

		gpuMeshCache[mesh] = std::make_shared<OPENGL::Mesh>(mesh);
		gpuMeshCache[mesh]->gpuUploaded = true;

	}

	return gpuMeshCache[mesh];

}
std::shared_ptr<Cle::Gfx::ITexture> Cle::OPENGL::Renderer::getOrMakeTexture(const std::string& path)
{
	if (!textureCache.contains(path))
	{

		textureCache[path] = std::make_shared<OPENGL::Texture>(path);
	}

	return textureCache[path];


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

static void onlightadded(entt::registry& registry, entt::entity e)
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
	glEnable(GL_MULTISAMPLE);
	glfwWindowHint(GLFW_SAMPLES,2);
	glfwSwapInterval(1);
	[&] {
		std::vector<std::string> shaders = { "Shaders/light.frag","Shaders/fog.frag","Shaders/default.frag" };
		Cle::OPENGL::Program program("Shaders/default.vert", shaders);
		shaderCache["MeshShader"] = std::make_shared<OPENGL::Shader>(program.ID);

		std::vector<std::string> skyshaders = { "Shaders/skybox.frag" };
		Cle::OPENGL::Program skyprogram("Shaders/skybox.vert", skyshaders);
		shaderCache["SkyboxShader"] = std::make_shared<OPENGL::Shader>(skyprogram.ID);

		std::vector<std::string> depthshaders = { "Shaders/depth.frag" };
		Cle::OPENGL::Program depthprogram("Shaders/depth.vert", depthshaders);
		shaderCache["DepthShader"] = std::make_shared<OPENGL::Shader>(depthprogram.ID);
		}();
	m_registry->ctx().emplace<LightBuffer>(std::static_pointer_cast<OPENGL::Shader>(shaderCache["MeshShader"]));
	m_registry->on_construct<Cle::Components::LightComponent>().connect<&onlightadded>();
	m_registry->on_destroy<Cle::Components::LightComponent>().connect<&onlightremoved>();
	m_registry->ctx().emplace<Cle::Lighting*>(&Lighting::getInstance());

	glGenTextures(1, &sceneImage);
	glBindTexture(GL_TEXTURE_2D, sceneImage);
	glGenFramebuffers(1, &sceneFBO);
	glBindFramebuffer(GL_FRAMEBUFFER, sceneFBO);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);


	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, resolutionWidth, resolutionHeight, 0, GL_RGB, GL_UNSIGNED_BYTE, nullptr);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, sceneImage, 0);
	glGenRenderbuffers(1, &sceneRBO);
	glBindRenderbuffer(GL_RENDERBUFFER, sceneRBO);
	glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH24_STENCIL8, resolutionWidth, resolutionHeight);
	glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_RENDERBUFFER, sceneRBO);
	glBindRenderbuffer(GL_RENDERBUFFER, 0);
	glBindFramebuffer(GL_FRAMEBUFFER, 0);

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
	else if (distance < 300 * 300) return openglmesh;
	else if (distance < 700 * 700) return openglmesh->getLodMesh(1);
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

		if (!bytes)
		{
			std::cout << skybox[i] << "not loaded\n";
		}
		else
		{
			std::cout << "loaded" << skybox[i] << std::endl;
		}
		auto format = channels == 3 ? GL_RGB : GL_RGBA;
		glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, format, width, height, 0, format, GL_UNSIGNED_BYTE, bytes);
		stbi_image_free(bytes);
	}

	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);

	Lighting::getInstance().skyboxTex = std::make_shared<Cle::OPENGL::Texture>(cubetex);
	return Lighting::getInstance().skyboxTex;
}


void Cle::OPENGL::Renderer::drawMesh(entt::entity e, entt::registry& registry, Cle::Gfx::Camera& camera)
{

	if (!registry.any_of < std::shared_ptr<IShader>>(e))
	{
		registry.emplace_or_replace<std::shared_ptr<Cle::OPENGL::Shader>>(e, std::static_pointer_cast<Cle::OPENGL::Shader>(getDefaultShader()));
	}

	if (!registry.all_of<Cle::Components::Transform, std::shared_ptr<GenericMesh>, std::shared_ptr<Cle::OPENGL::Shader>, Components::Bounds>(e)) {
		return;
	}
	cleanDirtyMesh(e);

	//Cle::Components::MaterialRef, 
//	auto& material = registry.get<Cle::Components::MaterialRef>(e);
	auto& shader = registry.get < std::shared_ptr < Cle::OPENGL::Shader >>(e);
	auto& gmesh = registry.get<std::shared_ptr<GenericMesh>>(e);
	auto& bounds = registry.get<Components::Bounds>(e);
	shader->Bind();

	auto mesh = std::static_pointer_cast<Cle::OPENGL::Mesh>(getOrMakeMesh(gmesh));

	auto& transform = registry.get<Cle::Components::Transform>(e);


	if (m_registry->any_of<Cle::Components::Color>(e))
	{
		shader->setVec3("color", glm::vec3(m_registry->get<Cle::Components::Color>(e).value));

	}
	else
	{
		shader->setVec3("color", glm::vec3(1, 1, 1));

	}


	glm::mat4 model = transform.getRelativeModel(camera);

	shader->setMat4("model", model);


	

	
	shader->setVec3("vPosition", transform.getPosition());
	/*if (gmesh->m_AABB.dirty)
		{
			gmesh->m_local_AABB.updateToWorld(gmesh->getVertices(), transform.model);
			gmesh->m_AABB.dirty = false;
		}*/

	if (registry.any_of < std::shared_ptr<Cle::Gfx::ITexture>>(e)) {
		auto& tex = registry.get < std::shared_ptr<Cle::Gfx::ITexture>>(e);

		glActiveTexture(GL_TEXTURE0 + meshColorMap);
		glBindTexture(GL_TEXTURE_2D, tex->getID());

		shader->setInt("usesMeshColorMap", (int)(true));
		shader->setInt("meshColorMap", meshColorMap);

	}
	else {
		shader->setInt("usesMeshColorMap", (int)(false));
	}

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

	glActiveTexture(GL_TEXTURE0 + shadowMapSlot);
	glBindTexture(GL_TEXTURE_2D, shadowMap);
	shaderCache["MeshShader"]->setInt("shadowMap", shadowMapSlot);

	assignLOD(e, camera.Position)->draw();


}

