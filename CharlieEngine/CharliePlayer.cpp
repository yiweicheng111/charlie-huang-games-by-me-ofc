#include "CharliePlayer.h"
#include "OpenGL4/OpenGLRenderer.h"
#include <glm/glm.hpp>
#include <iostream>
using namespace Cle::Gfx;
using namespace Cle::Components;
/*
void Cle::RunnableApplication::updateAABBS() {
	static auto frustum = Frustum::createFrustumInCamera(m_camera);
	for (auto ent : registry.view<std::shared_ptr<GenericMesh>>()) {
		if (registry.all_of<Transform, Bounds, std::shared_ptr<GenericMesh>>(ent)) {
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
}*/

void Cle::RunnableApplication::updateBoundingSpheres() {
	static auto frustum = Frustum::createFrustumInCamera(*m_camera);
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


Cle::RunnableApplication::~RunnableApplication()
{
	glfwTerminate();
}
Cle::RunnableApplication::RunnableApplication()
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

	if (!registry.ctx().contains<Camera>())
	{
		registry.ctx().emplace<Camera>();
	}
	
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

	/*	World->deleteObjectCallback = [&]()
		{
			m_UIHandler.m_Focused_Entity = entt::null;
		};
	*/

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
		Render();

		Update(0.1f);

		renderer->clearFrame(window);

		m_network->poll();

	}
}
void Cle::RunnableApplication::AudioPass() {
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
void Cle::RunnableApplication::Render()
{
	m_camera->aspect = renderer->width/(float)renderer->height;
	renderer->drawRegistry(*m_camera, window);
}
void Cle::RunnableApplication::Update(float dt) {
	//Physics1::update(registry);



	updateBoundingSpheres();

}