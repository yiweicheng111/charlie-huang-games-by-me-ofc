#include "EditorApplication.h"
#include "OpenGL4/OpenGLRenderer.h"
#include <glm/glm.hpp>
#include <iostream>
using namespace Cle::Gfx;
using namespace Cle::Components;
void Cle::Editor::EditorApplication::updateAABBS() {
	static auto frustum = Frustum::createFrustumInCamera(m_camera);
	for (auto ent : registry.view<std::shared_ptr<GenericMesh>>()) {
		if (registry.all_of<Transform,Bounds,std::shared_ptr<GenericMesh>>(ent)) {
			Transform& t = registry.get<Transform>(ent);
			auto& m = registry.get<std::shared_ptr<GenericMesh>>(ent);
			glm::mat4 model = t.model;
			auto& aabb = registry.get<Bounds>(ent).aabb;
		
	
			if (aabb.dirty) {
				aabb = m->m_local_AABB;
				aabb.max *= t.getScale();
				aabb.min *= t.getScale();
				aabb.Translate(t.getPosition());
				aabb.dirty = false;
			}
		}
	}
}

void Cle::Editor::EditorApplication::updateBoundingSpheres() {
	static auto frustum = Frustum::createFrustumInCamera(m_camera);
	for (auto ent : registry.view<std::shared_ptr<GenericMesh>>()) {
		if (registry.all_of<Transform, Bounds, std::shared_ptr<GenericMesh>>(ent)) {
			Transform& t = registry.get<Transform>(ent);
			auto& m = registry.get<std::shared_ptr<GenericMesh>>(ent);
			auto& sphere = registry.get<Bounds>(ent).sphere;
			glm::mat4 model = t.model;
			if (sphere.dirty)
			{
				sphere.updateToWorld(m->getVertices(), model);
				sphere.dirty = false;
			}
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

	m_ScriptHandler = Cle::Scripting::ScriptHandler::getInstance();
	m_camera = Camera(glm::radians(70.0f), 1);
	registry.ctx().emplace<Camera*>(&m_camera);
	m_network = Network(&registry);
	m_network.onSceneLoaded = [this]()
		{
			renderer->onSceneLoaded();
		};

/*
	player = World.CreateDebugObject({}, {});
	registry.get<Cle::Components::Transform>(player).setScale({ 1,1,1 });*/
//	m_Controller = Cle::ObjectCameraController(&m_camera, window, &registry,player);
	m_Controller = Cle::Editor::FreeCameraControl(&m_camera, window);

	m_UIHandler = Cle::Editor::EditorUI(World.get(), window, &m_camera);
	World->deleteObjectCallback = [&]()
		{
			m_UIHandler.m_Focused_Entity = entt::null;
		};
}



void Cle::Editor::EditorApplication::runHotKey()
{
	if (m_UIHandler.io->WantCaptureKeyboard) return;
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
	if (glfwGetKey(window, GLFW_KEY_LEFT_CONTROL) == GLFW_PRESS && glfwGetKey(window, GLFW_KEY_F) == GLFW_PRESS && registry.valid(m_UIHandler.m_Focused_Entity)) {
		m_camera.Position = registry.get<Transform>(m_UIHandler.m_Focused_Entity).getPosition();
	}

}

void Cle::Editor::EditorApplication::runPointer()
{
	if (ImGui::GetIO().WantCaptureMouse) return;
	if (ImGuizmo::IsUsing()) return;
	if (glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_1) != GLFW_PRESS) return;
	m_UIHandler.m_Focused_Entity = entt::null;
	int winWidth, winHeight;
	double pX, pY;
	glfwGetCursorPos(window, &pX, &pY);
	glfwGetWindowSize(window, &winWidth, &winHeight);
	Cle::Components::Ray ray;
	ray.SetFromPointer(pX, pY, winWidth, winHeight, m_camera);
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
		renderer->beginFrame();
		Render();

		Update(0.1f);

		renderer->clearFrame(window);

		m_network.poll();

	}
}
void Cle::Editor::EditorApplication::AudioPass() {
	for (auto e : registry.view<std::shared_ptr<Cle::Audio::Sound>>()) {
		auto& sound = registry.get<std::shared_ptr<Cle::Audio::Sound>>(e);
		if (registry.any_of<Cle::Components::Transform>(e) && !sound->global) {
			glm::vec3 position = registry.get<Cle::Components::Transform>(e).getPosition();

			sound->position = position;
			sound->UpdateVolume(m_camera.Position);
			continue;
		}
	}
}
void Cle::Editor::EditorApplication::Render()
{
	if (World->worldLoading) return;
	renderer->lightingPass();

	renderer->lightPass();

	renderer->UniformCamMatrix(m_camera, renderer->getDefaultShader());

	//renderer->SyncMeshes(registry);
	int width, height;
	glfwGetWindowSize(window, &width, &height);
	if (width == 0 || height == 0) return;
	m_camera.aspect = float(width) / height;
	int totalMeshes = 0;
	int totalDrawn = 0;
	Frustum frustum = Frustum::createFrustumInCamera(m_camera);

	auto view = registry.view<std::shared_ptr<GenericMesh>,Transform,Bounds>();
	view.each([&](auto entity, std::shared_ptr<GenericMesh>& mesh, Transform& transform, Bounds& bounds)
		{

			if (World->worldLoading) return;
			//totalMeshes++;
		if (bounds.sphere.isOnFrustum(frustum, transform.model) && !mapLoading && renderer->isWithinFarPlane(entity,m_camera)) {
				renderer->drawMesh(entity, registry,m_camera);

			//	totalDrawn++;

			}
		
		});
	//std::cout << "Meshes: " << totalMeshes << " Drawn: " << totalDrawn << std::endl;
}
void Cle::Editor::EditorApplication::Update(float dt) {
	//Physics1::update(registry);
	m_Controller.Poll();

	m_UIHandler.Update();

	updateAABBS();

	updateBoundingSpheres();

	runHotKey();
	runPointer();
}