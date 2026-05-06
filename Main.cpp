
#include "IRenderer.h"
#include "EditorApplication.h"
#include "Mesh.h"
#include "OPENGL4/VAO.h"
#include "OPENGL4/VBO.h"
#include "OPENGL4/OpenGLMesh.h"
#include "OPENGL4/Program.h"
#include <iostream>
#include "miniaudio/miniaudio.h"
using Vertex = Cle::Gfx::Vertex;
std::vector<Vertex> vertices = {
	Vertex({-0.5f,-0.5f,0.0f},{0.0f,0.0f},{0.0f,0.0f,0.0f}),
	Vertex({-0.5f,0.5f,0.0f},{0.0f,0.0f},{0.0f,0.0f,0.0f}),
	Vertex({0.5f,-0.5f,0.0f},{0.0f,0.0f},{0.0f,0.0f,0.0f}),
};
int main() {
	Cle::Editor::EditorApplication app;

	std::filesystem::current_path("C:/Users/yiwei/Desktop/Charlie");
	std::vector< Cle::Gfx::GenericMesh> ModelLoaded = app.renderer->m_GenericMeshHandler.LoadModel("Models/bleachers.obj");
	auto tex = Cle::Gfx::OPENGL43::Texture("chair.png");
	for (Cle::Gfx::GenericMesh& I : ModelLoaded) {
		auto e = app.CreateDebugObject(I.Vertices,I.Indices);
		app.registry.get<Cle::Gfx::Material>(e) = app.renderer->getMaterial();
		app.registry.get<Cle::Gfx::Material>(e).colorMap = tex;
		app.registry.get<Cle::Gfx::Material>(e).usesColorMap = false;
		app.registry.get<Cle::Gfx::Material>(e).Color = glm::vec4(0.7,0.7,0.7,1);

		app.renderer->uploadMesh(e, app.registry);

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
