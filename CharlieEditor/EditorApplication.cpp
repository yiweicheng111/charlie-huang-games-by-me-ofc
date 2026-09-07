#define NOMINMAX
#include "EditorApplication.h"
#include "OpenGL4/OpenGLRenderer.h"
#include <glm/glm.hpp>
#include <iostream>
#include "FakeCharController.h"
using namespace Cle::Gfx;
using namespace Cle::Components;
void Cle::Editor::EditorApplication::updateAABBS() {
	auto frustum = Frustum::createFrustumInCamera(*m_camera);
	for (auto ent : registry.view<std::shared_ptr<GenericMesh>>()) {
		if (registry.all_of<Transform,Bounds,std::shared_ptr<GenericMesh>>(ent)) {
			Transform& t = registry.get<Transform>(ent);
			auto& m = registry.get<std::shared_ptr<GenericMesh>>(ent);
			glm::mat4 model = t.model;
			auto& aabb = registry.get<Bounds>(ent).aabb;
		
	
			if (aabb.dirty) {
				aabb = m->geometry->m_local_AABB;
				aabb.max *= t.getScale();
				aabb.min *= t.getScale();
				aabb.Translate(t.getPosition());
				aabb.dirty = false;
			}
		}
	}
}

void Cle::Editor::EditorApplication::updateBoundingSpheres() {
	auto frustum = Frustum::createFrustumInCamera(*m_camera);
	for (auto ent : registry.view<std::shared_ptr<GenericMesh>>()) {
		if (registry.all_of<Transform, Bounds, std::shared_ptr<GenericMesh>>(ent)) {
			Transform& t = registry.get<Transform>(ent);
			auto& m = registry.get<std::shared_ptr<GenericMesh>>(ent);
			auto& sphere = registry.get<Bounds>(ent).sphere;
			glm::mat4 model = t.model;
			
			/*if (sphere.dirty)
			{
				sphere.updateToWorld(m->getVertices(), model);
				sphere.dirty = false;
			}*/
			
			//std::cout << t.getPosition().x << " " << t.getPosition().y << " " << t.getPosition().z;
		
			//std::cout << "\n";
			sphere.updateToWorld(m->getVertices(), model);
			//std::cout << "sphere at " << sphere.center.x << " " << sphere.center.y << " "<<sphere.center.z << std::endl;
			sphere.dirty = false;
		}
	}
}



Cle::Editor::EditorApplication::~EditorApplication()
{
	glfwTerminate();
}
Cle::Editor::EditorApplication::EditorApplication()
{
	ma_engine_init(nullptr, &audio_engine);
	registry.ctx().emplace<ma_engine*>(&audio_engine);
	renderer = Renderer::IRenderer::Create(&registry);

	window = glfwCreateWindow(800, 800, "CHARLIE ZI", NULL, NULL);
	assert(window != NULL);
	glfwMakeContextCurrent(window);
	gladLoadGL();
	glfwSetFramebufferSizeCallback(window, [](GLFWwindow* window, int width, int height)
		{
			glViewport(0, 0, width, height);
		});
	renderer->setSettings();
	World = std::make_unique<Cle::World>(&registry, *renderer);

	//m_ScriptHandler = Cle::Scripting::ScriptHandler::getInstance();
	registry.ctx().emplace<Camera>(Camera());
	m_camera = &registry.ctx().get<Camera>();
	Network::setRegistry(&registry);
	m_network = &Network::getInstance();
	m_network->onSceneLoaded = [this]()
		{
			renderer->onSceneLoaded();
		};

/*
	player = World.CreateDebugObject({}, {});
	registry.get<Cle::Components::Transform>(player).setScale({ 1,1,1 });*/
//	m_Controller = Cle::ObjectCameraController(&m_camera, window, &registry,player);
	//m_Controller = Cle::CharController(m_camera, window);
	m_Controller = std::make_unique<Cle::Editor::FreeCameraControl>(m_camera, window);

	m_UIHandler = Cle::Editor::EditorUI(World.get(), window);
	/*	World->deleteObjectCallback = [&]()
		{
			m_UIHandler.m_Focused_Entity = entt::null;
		};
	*/

}



void Cle::Editor::EditorApplication::runHotKey()
{
	if (m_UIHandler.pointerBusy) return;
	//if (m_UIHandler.io->WantCaptureKeyboard) return;
	bool saveKey = glfwGetKey(window, GLFW_KEY_LEFT_CONTROL) == GLFW_PRESS && glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS;
	bool loadKey = glfwGetKey(window, GLFW_KEY_LEFT_CONTROL) == GLFW_PRESS && glfwGetKey(window, GLFW_KEY_L) == GLFW_PRESS;

	if (saveKey)
	{
		std::cout << "saved\n";
		Cle::gameIO::getInstance().setRegistry(&registry);
	//	Cle::gameIO::getInstance().setRenderer(renderer.get());

		Cle::gameIO::getInstance().Snapshot("world.bin");
	}
	mapLoading = true;
	if (loadKey)
	{

		Cle::gameIO::getInstance().setRegistry(&registry);
		//Cle::gameIO::getInstance().setRenderer(renderer.get());
		Cle::gameIO::getInstance().LoadFile("world.bin");
		renderer->onSceneLoaded();
	}
	mapLoading = false;

	if (m_UIHandler.m_Focused_Entity == entt::null) return;
	static bool copykeypresslastframe = false;
	bool copykeypress = glfwGetKey(window, GLFW_KEY_LEFT_CONTROL) == GLFW_PRESS && glfwGetKey(window, GLFW_KEY_C) == GLFW_PRESS;
	if (copykeypress && !copykeypresslastframe){
		World->CopyObject(m_UIHandler.m_Focused_Entity); copykeypresslastframe = true;
	}
	copykeypresslastframe = copykeypress;
	
	if (glfwGetKey(window, GLFW_KEY_LEFT_CONTROL) == GLFW_PRESS && glfwGetKey(window, GLFW_KEY_X) == GLFW_PRESS && registry.valid(m_UIHandler.m_Focused_Entity)) {
		registry.destroy(m_UIHandler.m_Focused_Entity);
	}
	if (glfwGetKey(window, GLFW_KEY_LEFT_CONTROL) == GLFW_PRESS && glfwGetKey(window, GLFW_KEY_F) == GLFW_PRESS && registry.valid(m_UIHandler.m_Focused_Entity) && registry.any_of<Transform>(m_UIHandler.m_Focused_Entity)) {
		m_camera->Position = registry.get<Transform>(m_UIHandler.m_Focused_Entity).getPosition();
	}


}

void Cle::Editor::EditorApplication::runPointer()
{
//	if (ImGui::GetIO().WantCaptureMouse) return;
	if (ImGuizmo::IsUsing()) return;
	if (m_UIHandler.pointerBusy) return;
	if (glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_1) != GLFW_PRESS) return;
	m_UIHandler.m_Focused_Entity = entt::null;
//	int winWidth, winHeight;
	double pX, pY;
	glfwGetCursorPos(window, &pX, &pY);
	
	//glfwGetWindowSize(window, &winWidth, &winHeight);
	Cle::Components::Ray ray;
	ray.SetFromPointer(m_UIHandler.mousePosRelativeToGame.x, m_UIHandler.mousePosRelativeToGame.y, m_camera->width, m_camera->height, *m_camera);
	float nearestD = (std::numeric_limits<float>::max)();
	entt::entity nearestEnt = entt::null;
	for (auto& ent : registry.view<std::shared_ptr<GenericMesh>>()) {
		if (!registry.any_of<Components::Bounds>(ent)) continue;
		auto& aabb = registry.get<Components::Bounds>(ent).aabb;
		float distance = aabb.intersects(ray);
		if (distance == -1) continue;
		if (distance >= nearestD) continue;
		nearestD = distance;
		nearestEnt = ent;
		m_UIHandler.m_Focused_Entity = nearestEnt;
	}
}



void Cle::Editor::EditorApplication::Run()
{
	while (!glfwWindowShouldClose(window))
	{
		if (glfwGetKey(window, GLFW_KEY_LEFT_CONTROL) == GLFW_PRESS && glfwGetKey(window, GLFW_KEY_T) == GLFW_PRESS) {
		
		}
		bool key0 = false;
		key0 = glfwGetKey(window, GLFW_KEY_0)==GLFW_PRESS;
		/*if (key0&&Physics1::running)
		{
			Physics1::pause();
		}
		else if (key0 && !Physics1::running)
		{
			Physics1::resume(registry);
		}*/
		AudioPass();
		Render();

		Update(0.1f);
		renderer->clearFrame(window);
		m_network->poll();
		ScriptHandler::getInstance().run();

	}
}
void Cle::Editor::EditorApplication::AudioPass() {
	for (auto e : registry.view<std::shared_ptr<Cle::Audio::Sound>>()) {
		auto& sound = registry.get<std::shared_ptr<Cle::Audio::Sound>>(e);
		if (registry.any_of<Cle::Components::Transform>(e) && !sound->global) {
			glm::vec3 position = registry.get<Cle::Components::Transform>(e).getPosition();

			sound->position = position;
			sound->UpdateVolume(m_camera->Position);
			continue;
		}
	}
}
void Cle::Editor::EditorApplication::Render()
{
	renderer->drawRegistry(*m_camera, window);
}
void Cle::Editor::EditorApplication::Update(float dt) {
	//Physics1::update(registry);
	if (!m_UIHandler.pointerBusy) m_Controller->Poll();

	m_UIHandler.Update();

	updateAABBS();

	updateBoundingSpheres();

	runHotKey();
	runPointer();
}