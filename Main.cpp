
#include "IRenderer.h"
#include "EditorApplication.h"
#include "Mesh.h"
#include "OPENGL4/VAO.h"
#include "OPENGL4/VBO.h"
#include "OPENGL4/OpenGLMesh.h"
#include "OPENGL4/Program.h"
#include <iostream>
#include "OPENGL4/LightBuffer.h"
#include "packet.h"
using Vertex = Cle::Gfx::Vertex;
std::vector<Vertex> vertices = {
	Vertex({-0.5f,-0.5f,0.0f},{0.0f,0.0f},{0.0f,0.0f,0.0f}),
	Vertex({-0.5f,0.5f,0.0f},{0.0f,0.0f},{0.0f,0.0f,0.0f}),
	Vertex({0.5f,-0.5f,0.0f},{0.0f,0.0f},{0.0f,0.0f,0.0f}),
};

int main() {
	srand(time(NULL));
	Cle::ColorPacket packet{};
	Cle::Editor::EditorApplication app;
	app.connectServer(8080,"127.0.0.1");
	auto radio = app.registry.create();

	app.registry.emplace<Cle::Audio::Sound>(radio, "goeshard.mp3", app.audio_engine);

	//app.registry.get<Cle::Audio::Sound>(radio).Play();
	app.registry.get<Cle::Audio::Sound>(radio).global = true;

	//std::filesystem::current_path("C:/Users/yiwei/Desktop/Charlie");
	//std::filesystem::current_path("../");

	std::cout << std::filesystem::current_path() << std::endl;;
	std::vector< Cle::Gfx::GenericMesh> ModelLoaded = app.renderer->m_AssetHandler.LoadModel("Models/alphabet.obj");
//	auto tex = Cle::Gfx::OPENGL43::Texture("chair.png");
	std::vector< Cle::Gfx::GenericMesh> alphabet = ModelLoaded;

	int i = 0;
	entt::entity floor;

	for (Cle::Gfx::GenericMesh& I : ModelLoaded) {
		auto e = app.World.CreateDebugObject(I);
		app.registry.get<Cle::Gfx::Material>(e).usesColorMap = false;

		app.registry.get<Cle::Gfx::Material>(e).setColor(I.assimpRequestedColor);

		std::cout << "new cube\n";
		break;
		//	std::cout << I.assimpRequestedColor.x << std::endl;
		//	std::cout << I.assimpRequestedTexturePath << std::endl;
	}
	for (Cle::Gfx::GenericMesh& I : alphabet) {
		//std::cout << j << std::endl;
		
		auto e = app.World.CreateDebugObject(I);
		auto& m = app.registry.get< std::shared_ptr < Cle::OPENGL::Mesh >>(e);
		auto& gm = app.registry.get<Cle::Gfx::GenericMesh>(e);

	
		app.registry.get<Cle::Gfx::Material>(e).usesColorMap = false;
		app.registry.get<Cle::Gfx::Material>(e).setColor(gm.assimpRequestedColor);

		//app.registry.get<Cle::Gfx::Material>(e).setColorMap(Cle::OPENGL::Texture(bat.ID));
	//	if (!gm.assimpRequestedDiffuse.empty()) gm.texture = app.renderer->createTexture(gm.assimpRequestedDiffuse);
		app.registry.get<Cle::Components::Name>(e).value = "hi";
		
	
	}
	//Cle::Physics::Physics1::reg(app.registry);
	//Cle::Physics::Physics1::physicssystem->GetBodyInterface().SetMotionType(app.registry.get<Cle::Components::PhysicsComponent>(floor).ID, JPH::EMotionType::Static, JPH::EActivation::Activate);

	app.renderer->clearColor(0.5f, 0.5f, 0.9f, 1);
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