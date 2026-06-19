#include "EditorApplication.h"
#include "OpenGL4/OpenGLRenderer.h"
#include <glm/glm.hpp>
#include <iostream>
using namespace Cle::Gfx;
using namespace Cle::Components;
void Cle::Editor::EditorApplication::updateAABBS() {
	Frustum frustum = Frustum::createFrustumInCamera(m_camera);
	for (auto ent : registry.view<GenericMesh>()) {
		if (registry.any_of<Transform>(ent)) {
			Transform& t = registry.get<Transform>(ent);
			GenericMesh& m = registry.get<GenericMesh>(ent);
			glm::mat4 model = t.model;
			auto& aabb = registry.get<GenericMesh>(ent).m_AABB;
			if (aabb.dirty) {
				aabb = registry.get<GenericMesh>(ent).m_local_AABB;
				aabb.max *= t.getScale();
				aabb.min *= t.getScale();
				aabb.Translate(t.getPosition());
				aabb.dirty = false;
			}
		}
	}
}

void Cle::Editor::EditorApplication::updateBoundingSpheres() {
	Frustum frustum = Frustum::createFrustumInCamera(m_camera);
	for (auto ent : registry.view<GenericMesh>()) {
		if (registry.any_of<Transform>(ent)) {
			Transform& t = registry.get<Transform>(ent);
			GenericMesh& m = registry.get<GenericMesh>(ent);
			glm::mat4 model = t.model;
			registry.get<GenericMesh>(ent).m_Bounding_Sphere.updateToWorld(m.getVertices(), model);
		}
	}
}
void Cle::Editor::EditorApplication::connectServer(int port,std::string ip)
{
	ENetAddress address;
	ENetPeer* peer;
	
	client = enet_host_create(NULL, 1, 2, 0, 0);
	if (!client)
	{
		std::cout << "cant create client\n";
		return;
	}
	enet_address_set_host(&address, ip.c_str());
	address.port = port;
	peer = enet_host_connect(client, &address, 2, 1);
	if (!peer)
	{
		std::cout << "server full\n";
		enet_host_destroy(client);
		return;
	}
	
	ENetEvent event;
	if (enet_host_service(client, &event, 1000) > 0 && event.type == ENET_EVENT_TYPE_CONNECT)
	{
		std::cout << "connected sucess\n";
	}
	else
	{
		std::cout << "connection timed out\n";
	}
}

void Cle::Editor::EditorApplication::listenServer()
{
	if (!client) return;
	ENetEvent* event;

}

Cle::Editor::EditorApplication::~EditorApplication()
{
	if (client) enet_host_destroy(client);
	glfwTerminate();
}
Cle::Editor::EditorApplication::EditorApplication()
{
	enet_initialize();
	ma_engine_init(nullptr, &audio_engine);
	renderer = Renderer::IRenderer::Create(&registry);
	window = glfwCreateWindow(800, 800, "t", NULL, NULL);
	assert(window != NULL);
	glfwMakeContextCurrent(window);
	gladLoadGL();
	glfwSetFramebufferSizeCallback(window, [](GLFWwindow* window, int width, int height)
		{
			glViewport(0, 0, width, height);
		});
	renderer->setSettings();
	World = Cle::World(&registry, renderer);
	World.deleteObjectCallback = [&]()
		{
			m_UIHandler.m_Focused_Entity = entt::null;
		};
	m_ScriptHandler = Cle::Scripting::ScriptHandler::getInstance();
	m_camera = Camera(glm::radians(70.0f), 1);
/*
	player = World.CreateDebugObject({}, {});
	registry.get<Cle::Components::Transform>(player).setScale({ 1,1,1 });*/
//	m_Controller = Cle::ObjectCameraController(&m_camera, window, &registry,player);
	m_Controller = Cle::Editor::FreeCameraControl(&m_camera, window);

	m_UIHandler = Cle::Editor::EditorUI(&World, window, &m_camera);

}



void Cle::Editor::EditorApplication::runHotKey()
{
	bool saveKey = glfwGetKey(window, GLFW_KEY_LEFT_CONTROL) == GLFW_PRESS && glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS;
	bool loadKey = glfwGetKey(window, GLFW_KEY_LEFT_CONTROL) == GLFW_PRESS && glfwGetKey(window, GLFW_KEY_L) == GLFW_PRESS;
	if (saveKey)
	{
		World.Snapshot("world.bin");
	}
	if (loadKey)
	{
		World.LoadFile("world.bin");
	}
	if (m_UIHandler.m_Focused_Entity == entt::null) return;
	if (glfwGetKey(window, GLFW_KEY_LEFT_CONTROL) == GLFW_PRESS && glfwGetKey(window, GLFW_KEY_C) == GLFW_PRESS){
		World.CopyObject(m_UIHandler.m_Focused_Entity);
	}
	if (glfwGetKey(window, GLFW_KEY_LEFT_CONTROL) == GLFW_PRESS && glfwGetKey(window, GLFW_KEY_X) == GLFW_PRESS) {
		World.DestroyObject(m_UIHandler.m_Focused_Entity);
	}
	if (glfwGetKey(window, GLFW_KEY_LEFT_CONTROL) == GLFW_PRESS && glfwGetKey(window, GLFW_KEY_F) == GLFW_PRESS && m_UIHandler.m_Focused_Entity != entt::null) {
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
	for (auto& ent : registry.view<GenericMesh>()) {
		AABB aabb = registry.get<GenericMesh>(ent).m_AABB;
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
	}
}
void Cle::Editor::EditorApplication::AudioPass() {
	for (auto e : registry.view<Cle::Audio::Sound>()) {
		auto& sound = registry.get<Cle::Audio::Sound>(e);
		if (registry.all_of<Cle::Components::Transform>(e) && !sound.global) {
			glm::vec3 position = registry.get<Cle::Components::Transform>(e).getPosition();

			sound.position = position;
			sound.UpdateVolume(m_camera.Position);
			continue;
		}
	}
}
void Cle::Editor::EditorApplication::Render()
{
	if (World.worldLoading) return;
	renderer->lightingPass();
	//renderer->SyncMeshes(registry);
	int width, height;
	glfwGetWindowSize(window, &width, &height);
	if (width == 0 || height == 0) return;
	m_camera.aspect = float(width) / height;
	int totalMeshes = 0;
	int totalDrawn = 0;
	Frustum frustum = Frustum::createFrustumInCamera(m_camera);
	auto view = registry.view<GenericMesh,Transform, Cle::Gfx::Material>();

	view.each([&](auto entity, GenericMesh& mesh, Transform& transform, Material& material)
		{
			if (World.worldLoading) return;

			renderer->UniformCamMatrix(m_camera, material);
			//totalMeshes++;
		 if (mesh.m_Bounding_Sphere.isOnFrustum(frustum, transform.model)) {

				if (!World.worldLoading) renderer->drawMesh(entity, registry,m_camera);
				//totalDrawn++;
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