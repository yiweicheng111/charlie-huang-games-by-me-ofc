#include "EditorUI.h"
#include "Mesh.h"
#include <filesystem>
#include "imgui/misc/cpp/imgui_stdlib.h"
using namespace Cle::Components;

namespace Cle::Editor
{
	EditorUI::EditorUI(Cle::World* World ,GLFWwindow* m_window, Cle::Gfx::Camera* m_camera) :m_camera(m_camera), m_window(m_window), m_registry(World->registry), World(World)
	{
		ImGui::CreateContext();
		io = &ImGui::GetIO();
		io->ConfigFlags |= ImGuiConfigFlags_DockingEnable;


		io->Fonts->AddFontFromFileTTF("./fonts/OpenSans.ttf", 18.0f);

		ImGuiStyle& style = ImGui::GetStyle();
		ImGui::StyleColorsLight();
		style.Colors[ImGuiCol_WindowBg] = ImVec4(0.8, 0.8, 0.8, 1.0);
		style.Colors[ImGuiCol_Text] = ImVec4(0.1,0.1,0.1, 1.0);
		style.Colors[ImGuiCol_TitleBg] = ImVec4(0.8, 0.8, 0.8, 1.0);
		style.Colors[ImGuiCol_TitleBgActive] = ImVec4(0.8, 0.8, 0.8, 1.0);
		style.WindowRounding = 5.0f;
		if (m_Pipeline == Cle::Gfx::Pipeline::OPENGL) {
			ImGui_ImplGlfw_InitForOpenGL(m_window, true);
			ImGui_ImplOpenGL3_Init("#version 330 core");
		}
		
	}
	void EditorUI::DrawChildren(entt::entity parent)
	{
		if (!m_registry->any_of<TreeInfo>(parent)) return;
		auto& children = m_registry->get<TreeInfo>(parent).Children;
		if (children.empty()) return;
		for (entt::entity child : children) {
			if (!m_registry->any_of<Name>(child)) {
				m_registry->emplace<Name>(child, "Untitled Object");
			}
			std::string name = m_registry->get<Cle::Components::Name>(child).value;
			ImGui::PushID((int)child);
			bool open = ImGui::TreeNode(name.c_str());
			DrawContextMenu(child);
			if (open)
			{
				DrawChildren(child);

				ImGui::TreePop();
			}

			ImGui::PopID();
		}
	}
	void EditorUI::DrawExplorer()
	{
			
			ImGui::Begin("Explorer");
			if (ImGui::TreeNodeEx("World", ImGuiTreeNodeFlags_DefaultOpen))
			{
				for (auto e : m_registry->view<TreeInfo>())
				{  
					if (m_registry->get<TreeInfo>(e).parent != entt::null) continue;
					if (!m_registry->any_of<Name>(e)) m_registry->emplace<Name>(e, "Untitled Object");
					ImGui::PushID((int)e);

					if (m_Focused_Entity == e) ImGui::PushStyleColor(ImGuiCol_Header, ImVec4(0.7, 0.7, 1.0, 1.0));
					else ImGui::PushStyleColor(ImGuiCol_Header, ImVec4(0.8, 0.8, 0.8, 1.0));
					bool open = ImGui::TreeNodeEx(m_registry->get<Cle::Components::Name>(e).value.c_str(),ImGuiTreeNodeFlags_Framed);

					DrawContextMenu(e);
					if (open && ImGui::IsItemToggledOpen())
					{
						m_Focused_Entity = e;						
					}
					if (open)
					{
						DrawChildren(e);
						ImGui::TreePop();
					}

					ImGui::PopStyleColor();

					ImGui::PopID();
				}
				ImGui::TreePop();
			}
			ImGui::End();

	}
	void EditorUI::DrawProperties()
	{
		if (m_Focused_Entity == entt::null)
		{
			return;
		}
		ImGui::Begin("Properties");

		Transform* transform =	m_registry->try_get<Transform>(m_Focused_Entity);
		LightComponent* lightComponent = m_registry->try_get<LightComponent>(m_Focused_Entity);
		Cle::Gfx::Material* material = m_registry->try_get<Cle::Gfx::Material>(m_Focused_Entity);
		Cle::Gfx::GenericMesh* mesh = m_registry->try_get<Cle::Gfx::GenericMesh>(m_Focused_Entity);

		Cle::Components::CubeMapTexture* cubeMap = m_registry->try_get<Cle::Components::CubeMapTexture>(m_Focused_Entity);

		if (transform)
		{
			if (ImGui::TreeNodeEx("Transform", ImGuiTreeNodeFlags_DefaultOpen))
			{
				auto pos = transform->getPosition();
				auto orien = transform->getOrientation();
				auto scale = transform->getScale();
				if (ImGui::DragFloat3("Position", (float*)&pos))
				{
					transform->setPosition(pos);

				}
				if (ImGui::DragFloat3("Orientation", (float*)&orien))
				{
					transform->setScale(scale);

				}
				if (ImGui::DragFloat3("Scale", (float*)&scale))
				{
					transform->setOrientation(orien);

				}				

				ImGui::TreePop();

			}
		}
		if (lightComponent)
		{
			glm::vec3 Color = lightComponent->getColor();
			float radius = lightComponent->getRadius();
			if (ImGui::TreeNodeEx("Light", ImGuiTreeNodeFlags_DefaultOpen))
			{
				if (ImGui::ColorEdit3("Color", (float*)&Color))
				{
					lightComponent->setColor(Color);
				}

				if (ImGui::InputFloat("Radius", &radius))
				{
					lightComponent->setRadius(radius);
				}


				ImGui::TreePop();
			}
		}
		if (material)
		{
			if (ImGui::TreeNodeEx("Material", ImGuiTreeNodeFlags_DefaultOpen))
			{

				glm::vec3 Color = material->getColor();
				if (ImGui::ColorEdit3("Color", (float*)&Color))
				{
					material->setColor(Color);
				}
				ImGui::TreePop();

			}
		}

		if (mesh)
		{
			if (ImGui::TreeNodeEx("Mesh", ImGuiTreeNodeFlags_DefaultOpen))
			{
				std::string path;
				if (!mesh->texture)
				{
					path = "";
					if (ImGui::InputText("Path", &path) && glfwGetKey(m_window, GLFW_KEY_ENTER))
					{
						mesh->texture->setPath(path);
					}
				}
			
				
				ImGui::TreePop();

			}
		}
		ImGui::End();
	}
	void EditorUI::DrawTopBar()
	{
		if (m_Pipeline == Cle::Gfx::Pipeline::OPENGL)
		{
			ImGui::Begin("Top Bar");
			ImGui::End();
		}
	}
	void EditorUI::DrawContextMenu(entt::entity e)
	{
		if (e == entt::null) return;
		if (ImGui::BeginPopupContextItem(("context"+std::to_string((int)e)).c_str()))
		{
			if (ImGui::MenuItem("Copy"))
			{

			}
			if (ImGui::BeginMenu("Add"))
			{


				if (ImGui::MenuItem("Light")) {
					if (!m_registry->any_of<Cle::Components::LightComponent>(e)) {
						m_registry->emplace<Cle::Components::LightComponent>(e);
					}
				}
				if (ImGui::MenuItem("Texture")) {
					if (!m_registry->any_of<Cle::Components::CubeMapTexture>(e)) {
						m_registry->emplace<Cle::Components::CubeMapTexture>(e);
					}
				}
				ImGui::EndMenu();
			}
			ImGui::EndPopup();
		}
	}
	void EditorUI::DrawGizmo(entt::entity entity)
	{
		static auto gizmoType = ImGuizmo::OPERATION::TRANSLATE;
		if (glfwGetKey(m_window, GLFW_KEY_E) == GLFW_PRESS) {
			gizmoType = ImGuizmo::OPERATION::TRANSLATE;
		}
		if (glfwGetKey(m_window, GLFW_KEY_R) == GLFW_PRESS) {
			gizmoType = ImGuizmo::OPERATION::SCALE;
		}
		if (glfwGetKey(m_window, GLFW_KEY_T) == GLFW_PRESS) {
			gizmoType = ImGuizmo::OPERATION::ROTATE;
		}
		Cle::Components::Transform* transform = m_registry->try_get<Cle::Components::Transform>(entity);
		if (!transform) return;
		if (!m_registry->any_of<Cle::Gfx::GenericMesh>(entity)) return;
		Cle::Gfx::GenericMesh mesh = m_registry->get<Cle::Gfx::GenericMesh>(entity);
		ImGuizmo::SetDrawlist(ImGui::GetBackgroundDrawList());
		int width, height;
		glfwGetWindowSize(m_window, &width, &height);
		ImGuizmo::SetRect(
			0,0,
			width,
			height
		);
		
		glm::mat4 modelCopy = transform->model;
		ImGuizmo::Manipulate(
			glm::value_ptr(m_camera->getViewMatrix()),
			glm::value_ptr(m_camera->getProjection()),
			gizmoType,
			ImGuizmo::LOCAL,
			glm::value_ptr(modelCopy)
		);
		if (ImGuizmo::IsUsing()) {
			glm::vec3 nscale, npos, nrot;
			ImGuizmo::DecomposeMatrixToComponents(glm::value_ptr(modelCopy), glm::value_ptr(npos), glm::value_ptr(nrot), glm::value_ptr(nscale));
			transform->setScale(nscale);  transform->setOrientation(glm::radians(nrot)); transform->setPosition(npos);
			transform->dirty = true;
		}
	
	}
	void EditorUI::Update()
	{

		ImGui_ImplOpenGL3_NewFrame();
		ImGui_ImplGlfw_NewFrame();
		ImGui::NewFrame();
		ImGuizmo::BeginFrame();
		DrawExplorer();
		DrawProperties();
		if (m_Focused_Entity != entt::null && m_registry->valid(m_Focused_Entity)) {
			DrawGizmo(m_Focused_Entity);
		}
		ImGui::GetIO().WantCaptureMouse = ImGuizmo::IsOver() ? false : ImGui::GetIO().WantCaptureMouse;
		ImGui::Render();
		ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
	}
	
}