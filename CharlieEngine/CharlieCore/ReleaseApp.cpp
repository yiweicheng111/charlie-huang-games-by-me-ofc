/*
#include "ReleaseApp.h"
#include "OpenGL4/OpenGLRenderer.h"
#include <iostream>
using namespace Cle::Gfx;
using namespace Cle::Components;
using namespace Cle::Core;

void ReleaseApp::updateBoundingSpheres() {
	Frustum frustum = Frustum::createFrustumInCamera(m_camera);
	for (auto ent : registry.view<GenericMesh>()) {
		if (registry.any_of<Transform>(ent)) {
			Transform& t = registry.get<Transform>(ent);
			GenericMesh& m = registry.get<GenericMesh>(ent);
			glm::mat4 model = t.computeMatrix();
			if (m.m_Bounding_Sphere.isOnFrustum(frustum, model)) {
				if (t.dirty) {
					registry.get<GenericMesh>(ent).m_Bounding_Sphere.updateToWorld(m.getVertices(), model);
					t.dirty = false;
				}
			}
		}
	}
}
ReleaseApp::~ReleaseApp()
{
	ma_engine_uninit(&audio_engine);
}
ReleaseApp::ReleaseApp()
{

	ma_engine_init(nullptr, &audio_engine);
	renderer = Renderer::IRenderer::Create(&registry);
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

entt::entity ReleaseApp::CreateDebugObject(const std::vector<Cle::Gfx::Vertex>& defaultVert, const std::vector<unsigned int>& indices)
{
	entt::entity charlie = registry.create();
	registry.emplace<Cle::Gfx::GenericMesh>(charlie, defaultVert, indices);
	registry.emplace<Cle::Components::Transform>(charlie);
	registry.emplace<Cle::Gfx::Material>(charlie, renderer->getMaterial());
	registry.emplace<Cle::Components::Name>(charlie, "charlie");
	registry.emplace<TreeInfo>(charlie);

	return charlie;
}

entt::entity ReleaseApp::CreateDebugObject(const std::vector<Cle::Gfx::Vertex>& defaultVert, const std::vector<unsigned int>& indices, entt::entity Parent)
{
	entt::entity charlie = registry.create();
	registry.emplace<Cle::Gfx::GenericMesh>(charlie, defaultVert, indices);
	registry.emplace<Cle::Components::Transform>(charlie);
	registry.emplace<Cle::Gfx::Material>(charlie, renderer->getMaterial());
	registry.emplace<Cle::Components::Name>(charlie, "charlie");
	registry.emplace<TreeInfo>(charlie);
	registry.get<TreeInfo>(charlie).parent = Parent;
	registry.get<TreeInfo>(Parent).Children.push_back(charlie);

	return charlie;

}
void ReleaseApp::AudioPass() {
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
entt::entity ReleaseApp::CopyObject(entt::entity existing)
{
	entt::entity newent = registry.create();
	if (registry.all_of<Transform>(existing)) {
		registry.emplace<Transform>(newent, registry.get<Transform>(existing));
	}
	if (registry.all_of<Name>(existing)) {
		registry.emplace<Name>(newent, registry.get<Name>(existing));
	}
	if (registry.all_of<Material>(existing)) {
		registry.emplace<Material>(newent, registry.get<Material>(existing));
	}
	if (registry.all_of<GenericMesh>(existing)) {
		registry.emplace<GenericMesh>(newent, registry.get<GenericMesh>(existing));
		renderer->uploadMesh(newent, registry);
	}
	if (registry.all_of<TreeInfo>(existing)) {
		registry.emplace<TreeInfo>(newent, registry.get<TreeInfo>(existing));
	}
	return newent;
}

void ReleaseApp::DestroyObject(entt::entity existing)
{

	if (!registry.valid(existing)) return;

	if (registry.any_of<TreeInfo>(registry.get<TreeInfo>(existing).parent)) {
		std::vector<entt::entity>& pChildren = registry.get<TreeInfo>(registry.get<TreeInfo>(existing).parent).Children;
		pChildren.erase(std::remove(pChildren.begin(), pChildren.end(), existing), pChildren.end());
	}
	registry.destroy(existing);
}



void ReleaseApp::Run()
{

	while (!glfwWindowShouldClose(window))
	{
		AudioPass();
		renderer->beginFrame();
		Render();
		Update(0.1f);
		renderer->clearFrame(window);
	}
}

void ReleaseApp::Render()
{
	//renderer->SyncMeshes(registry);

	int width, height;
	glfwGetWindowSize(window, &width, &height);
	if (width == 0 || height == 0) return;
	m_camera.aspect = float(width) / height;
	int totalMeshes = 0;
	int totalDrawn = 0;
	Frustum frustum = Frustum::createFrustumInCamera(m_camera);
	for (auto entity : registry.view<GenericMesh>())
	{
		auto& mesh = registry.get<GenericMesh>(entity);
		Transform& transform = registry.get<Transform>(entity);
		Cle::Gfx::Material& material = registry.get<Cle::Gfx::Material>(entity);
		renderer->UniformCamMatrix(m_camera, material);
		totalMeshes++;
		if (mesh.m_Bounding_Sphere.isOnFrustum(frustum, transform.computeMatrix())) {
			renderer->drawMesh(entity, registry);
			totalDrawn++;
		}
	}
	//std::cout << "Meshes: " << totalMeshes << " Drawn: " << totalDrawn << std::endl;
}
void ReleaseApp::Update(float dt) {

}*/