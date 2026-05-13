
#include "IRenderer.h"
#include "EditorApplication.h"
#include "Mesh.h"
#include "OPENGL4/VAO.h"
#include "OPENGL4/VBO.h"
#include "OPENGL4/OpenGLMesh.h"
#include "OPENGL4/Program.h"
#include <iostream>
using Vertex = Cle::Gfx::Vertex;
std::vector<Vertex> vertices = {
	Vertex({-0.5f,-0.5f,0.0f},{0.0f,0.0f},{0.0f,0.0f,0.0f}),
	Vertex({-0.5f,0.5f,0.0f},{0.0f,0.0f},{0.0f,0.0f,0.0f}),
	Vertex({0.5f,-0.5f,0.0f},{0.0f,0.0f},{0.0f,0.0f,0.0f}),
};

int main() {
	srand(time(NULL));

	Cle::Editor::EditorApplication app;
	auto radio = app.registry.create();
	app.registry.emplace<Cle::Audio::Sound>(radio, "goeshard.mp3", app.audio_engine);
	app.registry.get<Cle::Audio::Sound>(radio).Play();
	app.registry.get<Cle::Audio::Sound>(radio).global = true;

	std::filesystem::current_path("C:/Users/yiwei/Desktop/Charlie");
	std::vector< Cle::Gfx::GenericMesh> ModelLoaded = app.renderer->m_GenericMeshHandler.LoadModel("Models/Bambo_House.obj");
	auto tex = Cle::Gfx::OPENGL43::Texture("chair.png");
	for (Cle::Gfx::GenericMesh& I : ModelLoaded) {
		auto e = app.CreateDebugObject(I);
		app.registry.get<Cle::Gfx::Material>(e).colorMap = tex;
		app.registry.get<Cle::Gfx::Material>(e).usesColorMap = false;
		app.registry.get<Cle::Gfx::Material>(e).Color = I.assimpRequestedColor;
		//	std::cout << I.assimpRequestedColor.x << std::endl;
	
		app.renderer->uploadMesh(e, app.registry);
		//	std::cout << I.assimpRequestedTexturePath << std::endl;
	

	}

	app.renderer->clearColor(0, 0, 0, 1);
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