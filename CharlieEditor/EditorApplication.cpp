#include "EditorApplication.h"
#include "OpenGL4/OpenGLRenderer.h"
using namespace Cle::Gfx;
using namespace Cle::Components;
Cle::Editor::EditorApplication::EditorApplication()
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
	m_Controller = Cle::Editor::FreeCameraControls(&m_camera,window);
	m_UIHandler = Cle::Editor::EditorUI(&registry, window,&m_camera);
}

entt::entity Cle::Editor::EditorApplication::CreateDebugObject(std::vector<Cle::Gfx::Vertex>& defaultVert, std::vector<unsigned int>& indices)
{
	entt::entity charlie = registry.create();
	registry.emplace<Cle::Gfx::GenericMesh>(charlie, defaultVert, indices);
	registry.emplace<Cle::Components::Transform>(charlie);
	registry.emplace<Cle::Gfx::Material>(charlie,renderer->getMaterial());
	registry.emplace<Cle::Components::Name>(charlie, "charlie");

	return charlie;
}

void Cle::Editor::EditorApplication::runPointer()
{
	if (glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_1) == GLFW_PRESS) {
		int winWidth, winHeight;
		double pX, pY;
		glfwGetCursorPos(window, &pX, &pY);
		glfwGetWindowSize(window, &winWidth, &winHeight);
		Cle::Components::Ray ray;
		ray.SetFromPointer(pX, pY, winWidth, winHeight, m_camera);
		float nearestD = std::numeric_limits<float>::max();
		entt::entity nearestEnt = entt::null;
		for (auto& ent : registry.view<GenericMesh>()) {
			AABB aabb = registry.get<GenericMesh>(ent).m_AABB;
			float distance = aabb.intersects(ray);
			if (distance != -1) {
				if (distance < nearestD) {
					nearestD = distance;
					nearestEnt = ent;
					m_UIHandler.m_Focused_Entity = nearestEnt;
				}
			}
		}
	}
}



void Cle::Editor::EditorApplication::Run()
{

	while (!glfwWindowShouldClose(window))
	{
		renderer->beginFrame();
		Render();
		Update(0.1f);
		renderer->clearFrame(window);
	}
}

void Cle::Editor::EditorApplication::Render()
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
void Cle::Editor::EditorApplication::Update(float dt) {
	m_Controller.Poll();
	m_UIHandler.Update();
	runPointer();
}