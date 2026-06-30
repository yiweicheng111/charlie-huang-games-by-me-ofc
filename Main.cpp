
#include "IRenderer.h"
#include "EditorApplication.h"
#include "Mesh.h"
#include "OPENGL4/VAO.h"
#include "OPENGL4/VBO.h"
#include "OPENGL4/OpenGLMesh.h"
#include "OPENGL4/Program.h"
#include <iostream>
#include "OPENGL4/LightBuffer.h"
#include "shared.h"
#include "CharlieEngine/CharlieCore/CharliePlayer.h"
#include "CharlieCore/AssetHandler.h"
using Vertex = Cle::Gfx::Vertex;
std::vector<Vertex> vertices = {
	Vertex({-0.5f,-0.5f,0.0f},{0.0f,0.0f},{0.0f,0.0f,0.0f}),
	Vertex({-0.5f,0.5f,0.0f},{0.0f,0.0f},{0.0f,0.0f,0.0f}),
	Vertex({0.5f,-0.5f,0.0f},{0.0f,0.0f},{0.0f,0.0f,0.0f}),
};

int main() {
	srand(time(NULL));
	//Cle::Editor::EditorApplication app;
	Cle::Editor::EditorApplication app;

	app.m_network.connectServer(8080,"127.0.0.1");



	//std::filesystem::current_path("C:/Users/yiwei/Desktop/Charlie");
	//std::filesystem::current_path("../");

	std::cout << std::filesystem::current_path() << std::endl;;
	const std::vector<std::shared_ptr<Cle::GenericMesh>>& ModelLoaded = Cle::AssetHandler::getInstance().LoadModel("map/w.gltf");
	//const std::vector< Cle::GenericMesh>& ModelLoaded = app.renderer->m_AssetHandler.LoadModel("map/d.obj");

//	auto tex = Cle::Gfx::OPENGL43::Texture("chair.png");

	int i = 0;
	entt::entity floor;

	for (auto& I : ModelLoaded) {
		//std::cout << j << std::endl;
	
		auto e = app.World->CreateDebugObject(I);
		if (i == 0)
		{
			floor = e;
			app.registry.emplace<std::shared_ptr<Cle::Audio::Sound>>(e, std::make_shared<Cle::Audio::Sound>("beatit.mp3", &app.audio_engine));
			app.registry.get<std::shared_ptr<Cle::Audio::Sound>>(e)->Play();
			app.m_UIHandler.m_Focused_Entity = floor;
		}
	
		//app.registry.get<Cle::Components::MaterialRef>(e).usesColorMap = false;
		app.registry.get<Cle::Components::Color>(e).value = glm::vec4(I->assimpRequestedColor ,1);

		//app.registry.get<Cle::Gfx::Material>(e).setColorMap(Cle::OPENGL::Texture(bat.ID));
		//if (!gm.assimpRequestedDiffuse.empty()) gm.texture = app.renderer->createTexture(gm.assimpRequestedDiffuse);

		i++;

	}

	//Cle::Physics::Physics1::reg(app.registry);
	//Cle::Physics::Physics1::physicssystem->GetBodyInterface().SetMotionType(app.registry.get<Cle::Components::PhysicsComponent>(floor).ID, JPH::EMotionType::Static, JPH::EActivation::Activate);

	app.m_camera.Position.z = 4;
	app.m_camera.lookAt(glm::vec3{ 0, 0, 0 });
	glm::vec3 pos = app.m_camera.Position;
	app.Run();
	glfwTerminate();
	return 0;
}

/*
#include "Scripting/Scripting.h"

int main()
{
	Cle::Scripting::ScriptHandler g_ScriptHandler = Cle::Scripting::ScriptHandler::getInstance();
	g_ScriptHandler.runFile(std::string("../CharlieEngine/Scripts/Main.lua"));
	return 0;
}*/