#include "EBO.h"	
#include "VAO.h"
#include "IRenderer.h"
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include "VBO.h"
#include <entt/entt.hpp>
#include "Material.h"
#include "OpenGLMesh.h"

using namespace Cle::Gfx::OPENGL43;
namespace Cle::OPENGL43
{
	class Renderer : public Cle::Renderer::IRenderer
	{
	public:
		std::unordered_map<std::string, GLuint> programMap;
		Renderer();
		virtual Cle::Gfx::Material getMaterial() override;
		virtual void beginFrame() override;
		virtual void clearFrame(GLFWwindow* window) override;
		virtual void clearColor(float r, float g, float b, float w) override;
		virtual void uploadMesh(entt::entity e, entt::registry& registry) override;
		virtual void setSettings() override;
		virtual void drawMesh(entt::entity e, entt::registry& registry) override;
		virtual void UniformCamMatrix(Cle::Gfx::Camera& camera, Cle::Gfx::Material& material) override;

	};
}