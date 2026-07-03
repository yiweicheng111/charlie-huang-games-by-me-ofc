#include "CharliePlayer.h"
#include "IRenderer.h"
using namespace Cle::Gfx;
using namespace Cle::Components;


void Cle::RunnableApplication::updateBoundingSpheres() {
	static auto frustum = Frustum::createFrustumInCamera(m_camera);
	for (auto ent : registry.view<std::shared_ptr<GenericMesh>>()) {
		if (registry.any_of<Transform>(ent)) {
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

void Cle::RunnableApplication::findClientCamera()
{
	if (!registry.ctx().contains<Cle::Gfx::Camera>()) return;
	m_camera = registry.ctx().get<Cle::Gfx::Camera>();
}



Cle::RunnableApplication::~RunnableApplication()
{
	if (m_network.client) enet_host_destroy(m_network.client);
	glfwTerminate();
}
Cle::RunnableApplication::RunnableApplication()
{
	enet_initialize();
	ma_engine_init(nullptr, &audio_engine);
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
	m_network = Network(&registry);
	/*
		player = World.CreateDebugObject({}, {});
		registry.get<Cle::Components::Transform>(player).setScale({ 1,1,1 });*/
		//	m_Controller = Cle::ObjectCameraController(&m_camera, window, &registry,player);

	
}








void Cle::RunnableApplication::Run()
{
	while (!glfwWindowShouldClose(window))
	{
		bool key0 = false;
		key0 = glfwGetKey(window, GLFW_KEY_0) == GLFW_PRESS;
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
void Cle::RunnableApplication::AudioPass() {
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
void Cle::RunnableApplication::Render()
{
	if (World->worldLoading) return;
	renderer->lightingPass();
	//renderer->SyncMeshes(registry);
	int width, height;
	glfwGetWindowSize(window, &width, &height);
	if (width == 0 || height == 0) return;
	m_camera.aspect = float(width) / height;
	int totalMeshes = 0;
	int totalDrawn = 0;
	Frustum frustum = Frustum::createFrustumInCamera(m_camera);
	auto view = registry.view<std::shared_ptr<GenericMesh>, Transform, Components::Bounds>();
	view.each([&](auto entity, std::shared_ptr<GenericMesh>& mesh, Transform& transform, Components::Bounds& bounds)
		{
			if (World->worldLoading) return;
			auto& shader = registry.emplace<std::shared_ptr<IShader>>(entity, renderer->getDefaultShader());
			renderer->UniformCamMatrix(m_camera, shader);
			//totalMeshes++;
			if (bounds.sphere.isOnFrustum(frustum, transform.model) && !mapLoading) {
				renderer->drawMesh(entity, registry, m_camera);
				//totalDrawn++;
			}
		});
	//std::cout << "Meshes: " << totalMeshes << " Drawn: " << totalDrawn << std::endl;
}
void Cle::RunnableApplication::Update(float dt) {
	//Physics1::update(registry);
	updateBoundingSpheres();
}