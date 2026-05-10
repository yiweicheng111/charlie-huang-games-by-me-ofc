#include "PlayApplication.h"
#include "OpenGL4/OpenGLRenderer.h"
using namespace Cle::Gfx;
using namespace Cle::Components;


namespace Cle::Core
{
	PlayApplication::PlayApplication()
	{
		renderer = Renderer::IRenderer::Create();
		window = glfwCreateWindow(800, 800, "charlie", NULL, NULL);
		assert(window != NULL);
		glfwMakeContextCurrent(window);
		gladLoadGL();
		glfwSetFramebufferSizeCallback(window, [](GLFWwindow* window, int width, int height)
			{
				glViewport(0, 0, width, height);
			});
		renderer->setSettings();
		m_ScriptHandler = Cle::Scripting::ScriptHandler::getInstance();
		m_camera = Camera(glm::radians(70.0f), 1);
	}

	entt::entity PlayApplication::CreateDebugObject(const std::vector<Cle::Gfx::Vertex>& defaultVert, const std::vector<unsigned int>& indices)
	{
		entt::entity charlie = registry.create();
		registry.emplace<Cle::Gfx::GenericMesh>(charlie, defaultVert, indices);
		registry.emplace<Cle::Components::Transform>(charlie);
		registry.emplace<Cle::Gfx::Material>(charlie, 3);
		return charlie;
	}



	void PlayApplication::Run()
	{

		while (!glfwWindowShouldClose(window))
		{
			renderer->beginFrame();
			Render();
			Update(0.1f);
			renderer->clearFrame(window);
		}
	}

	void PlayApplication::Render()
	{
		int width, height;
		glfwGetWindowSize(window, &width, &height);
		m_camera.aspect = float(width) / height;
		for (auto entity : registry.view<GenericMesh>())
		{
			Transform& transform = registry.get<Transform>(entity);
			Cle::Gfx::Material material = registry.get<Cle::Gfx::Material>(entity);
			renderer->UniformCamMatrix(m_camera, material);
			renderer->drawMesh(entity, registry);
		}
	}
	void PlayApplication::Update(float dt) {
	}
}
