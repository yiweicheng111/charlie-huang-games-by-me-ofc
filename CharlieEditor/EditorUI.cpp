#include "EditorUI.h"
#include "Mesh.h"
using namespace Cle::Components;

namespace Cle::Editor
{
	EditorUI::EditorUI(entt::registry* registry,GLFWwindow* m_window, Cle::Gfx::Camera* m_camera) :m_camera(m_camera), m_window(m_window), m_registry(registry)
	{
		ImGui::CreateContext();
		io = &ImGui::GetIO();
		io->ConfigFlags |= ImGuiConfigFlags_DockingEnable;

		if (m_Pipeline == Cle::Gfx::Pipeline::OPENGL) {
			ImGui_ImplGlfw_InitForOpenGL(m_window, true);
			ImGui_ImplOpenGL3_Init("#version 330 core");
		}
	}
	void EditorUI::DrawExplorer() 
	{
		if (m_Pipeline == Cle::Gfx::Pipeline::OPENGL) {
			ImGui::Begin("Explorer");
			if (ImGui::TreeNodeEx("Everything", ImGuiTreeNodeFlags_DefaultOpen)) {
				for (entt::entity ent : m_registry->view<Cle::Components::Name>()) {
					std::string name = m_registry->get< Cle::Components::Name>(ent).value;
					ImGui::PushID((int)ent);
					if (ImGui::Button(name.c_str())) {}
					ImGui::PopID();
				}
				ImGui::TreePop();
			}
			
			ImGui::End();
		}
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
		Cle::Components::AABB aabb = m_registry->get<Cle::Gfx::GenericMesh>(entity).m_AABB;
		glm::vec3 center = 0.5f * (aabb.min + aabb.max);
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
			transform->AABBdirty = true;
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