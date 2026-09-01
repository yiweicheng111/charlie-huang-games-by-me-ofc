#include "EBO.h"	
#include <glad/glad.h>
#include <glfw/glfw3.h>
#include "VAO.h"
#include "IRenderer.h"
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include "VBO.h"
#include <entt/entt.hpp>
#include "Material.h"
#include "OpenGLMesh.h"
#include "OpenGL4/LightBuffer.h"

using namespace Cle::OPENGL;
namespace Cle::OPENGL
{
	class Renderer : public Cle::Renderer::IRenderer
	{
	public:
		GLuint sceneFBO;
		GLuint sceneImage;
		GLuint sceneRBO;
		Renderer(entt::registry* registry);
		virtual std::shared_ptr < Cle::Gfx::IMesh> getOrMakeMesh(std::shared_ptr<Cle::GenericMesh> mesh) override;
		virtual std::shared_ptr<Cle::Gfx::ITexture> getOrMakeTexture(const std::string& path) override;
		virtual void drawRegistry(Cle::Gfx::Camera& m_camera, GLFWwindow* window) override;
		virtual unsigned int getImage() const
		{
			return sceneImage;
		}
		virtual std::shared_ptr<Cle::IShader> getDefaultShader() override;
		virtual void beginFrame() override;
		virtual void clearFrame(GLFWwindow* window) override;
		virtual void clearColor(float r, float g, float b, float w) override;
		//virtual void uploadMesh(entt::entity e, std::shared_ptr<Cle::GenericMesh> mesh, entt::registry& registry) override;
		virtual void setSettings() override;
		virtual void drawMesh(entt::entity e, entt::registry& registry, Cle::Gfx::Camera& camera) override;
		virtual void lightPass() override;
	//	virtual void UniformCamMatrix(Cle::Gfx::Camera& camera, std::shared_ptr<Cle::IShader> shader) override;
	//	virtual void lightingPass() override;
	//	virtual void cleanDirtyMesh(entt::entity entity) override;
		virtual std::shared_ptr<Cle::Gfx::IMesh> assignLOD(entt::entity entity, glm::vec3 viewPosition) override;
		virtual std::shared_ptr<Cle::Gfx::ITexture> phraseSkybox(std::vector<std::string> skybox) override;

		//virtual void SyncMeshes(entt::registry& registry) override;
	};
}