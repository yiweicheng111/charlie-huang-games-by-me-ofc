
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
#include "CharlieEngine/CharliePlayer.h"
#include "CharlieEngine/AssetHandler.h"
using Vertex = Cle::Gfx::Vertex;
std::vector<Vertex> vertices = {
	Vertex({-0.5f,-0.5f,0.0f},{0.0f,0.0f},{0.0f,0.0f,0.0f}),
	Vertex({-0.5f,0.5f,0.0f},{0.0f,0.0f},{0.0f,0.0f,0.0f}),
	Vertex({0.5f,-0.5f,0.0f},{0.0f,0.0f},{0.0f,0.0f,0.0f}),
};

int main() {

	srand(time(NULL));
	Cle::Editor::EditorApplication app;
	//Cle::RunnableApplication app;
	app.m_network->connectServer(8080,"127.0.0.1");


	//std::filesystem::current_path("C:/Users/yiwei/Desktop/Charlie");
	//std::filesystem::current_path("../");sa




	std::cout << std::filesystem::current_path() << std::endl;;
	auto scripttest = app.World->CreateDebugObject();
	app.registry.emplace<Cle::Script>(scripttest,"Scripts/script.lua");
	app.registry.get<Cle::Components::TreeInfo>(scripttest).setParent(scripttest,app.World->Client,&app.registry);
	app.registry.get<Cle::Components::Name>(scripttest).setName("script");
	app.registry.emplace<std::unique_ptr< Cle::Audio::Sound>>(scripttest, std::make_unique<Cle::Audio::Sound>("beatit.mp3",&app.audio_engine,true));
	app.registry.get<std::unique_ptr<Cle::Audio::Sound>>(scripttest)->Play();

	std::cout << app.registry.get<Cle::Script>(scripttest).path << std::endl;

	auto s2 = app.World->CreateDebugObject();
	app.registry.emplace<Cle::Script>(s2, "Scripts/server.lua");
	app.registry.get<Cle::Components::TreeInfo>(s2).setParent(s2, app.World->Server, &app.registry);
	app.registry.get<Cle::Components::Name>(s2).setName("serverscript");
	//app.World->addModelToScene("map/f.gltf");
//	auto tex = Cle::Gfx::OPENGL43::Texture("chair.png");


//	std::cout << app.registry.storage<entt::entity>().size();

	//Cle::Physics::Physics1::reg(app.registry);
	//Cle::Physics::Physics1::physicssystem->GetBodyInterface().SetMotionType(app.registry.get<Cle::Components::PhysicsComponent>(floor).ID, JPH::EMotionType::Static, JPH::EActivation::Activate);

	glm::vec3 pos = app.m_camera->Position;
	Cle::ScriptHandler::getInstance();			

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