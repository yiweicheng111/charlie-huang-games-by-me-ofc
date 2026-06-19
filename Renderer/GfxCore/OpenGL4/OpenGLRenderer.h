#include "EBO.h"	
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
		entt::registry* m_registry;
		std::unordered_map<std::string, GLuint> programMap;
		Renderer(entt::registry* registry);
		virtual unsigned int getDefaultShader() override;
		virtual void beginFrame() override;
		virtual void lightPass() override;
		virtual void clearFrame(GLFWwindow* window) override;
		virtual void clearColor(float r, float g, float b, float w) override;
		virtual void uploadMesh(entt::entity e, entt::registry& registry) override;
		virtual void setSettings() override;
		virtual void drawMesh(entt::entity e, entt::registry& registry, Cle::Gfx::Camera& camera) override;
		virtual void clear() override;
		virtual void UniformCamMatrix(Cle::Gfx::Camera& camera, Cle::Gfx::Material& material) override;
		virtual void lightingPass() override;
		virtual void cleanDirtyMesh(entt::entity entity) override;
		virtual int assignLOD(entt::entity entity, glm::vec3 viewPosition) override;
		virtual std::shared_ptr<Cle::Gfx::ITexture> createTexture(std::string path) override;

		//virtual void SyncMeshes(entt::registry& registry) override;
	};
}