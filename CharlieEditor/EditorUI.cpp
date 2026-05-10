#include "EditorUI.h"
#include "Mesh.h"
#include <filesystem>
using namespace Cle::Components;

namespace Cle::Editor
{
	EditorUI::EditorUI(entt::registry* registry,GLFWwindow* m_window, Cle::Gfx::Camera* m_camera) :m_camera(m_camera), m_window(m_window), m_registry(registry)
	{
		ImGui::CreateContext();
		io = &ImGui::GetIO();
		io->ConfigFlags |= ImGuiConfigFlags_DockingEnable;

		std::filesystem::current_path("C:/Users/yiwei/Desktop/Charlie");

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
			if (ImGui::TreeNode(name.c_str())) {

				DrawChildren(child);
				ImGui::TreePop();
			}
			ImGui::PopID();
		}
	}
	void EditorUI::DrawExplorer()
	{
			ImGui::Begin("Explorer");
			if (ImGui::TreeNodeEx("World", ImGuiTreeNodeFlags_DefaultOpen)) {
				for (auto e : m_registry->view<TreeInfo>()) {
					if (m_registry->get<TreeInfo>(e).parent != entt::null) continue;
					if (!m_registry->any_of<Name>(e)) m_registry->emplace<Name>(e, "Untitled Object");
					ImGui::PushID((int)e);
				
					if (ImGui::TreeNode(m_registry->get<Cle::Components::Name>(e).value.c_str())) {
						DrawChildren(e);
						ImGui::TreePop();
					}
				
					ImGui::PopID();
				}
				ImGui::TreePop();
			}
			ImGui::End();

	}
	void EditorUI::DrawTopBar()
	{
		if (m_Pipeline == Cle::Gfx::Pipeline::OPENGL) {
			ImGui::Begin("Top Bar");
			ImGui::End();
		}
	}
	void EditorUI::DrawGizmo(entt::entity entity)
	{
	
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
			ImGuizmo::TRANSLATE,
			ImGuizmo::LOCAL,
			glm::value_ptr(modelCopy)
		);
		if (ImGuizmo::IsUsing()) {
			glm::vec3 nscale, npos, nrot;
			ImGuizmo::DecomposeMatrixToComponents(glm::value_ptr(modelCopy), glm::value_ptr(npos), glm::value_ptr(nrot), glm::value_ptr(nscale));
			transform->setScale(nscale);  transform->setOrientation(nrot); transform->setPosition(npos);
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
		if (m_Focused_Entity != entt::null && m_registry->valid(m_Focused_Entity)) {
			DrawGizmo(m_Focused_Entity);
		}
		ImGui::GetIO().WantCaptureMouse = ImGuizmo::IsOver() ? false : ImGui::GetIO().WantCaptureMouse;
		ImGui::Render();
		ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
	}
	
}