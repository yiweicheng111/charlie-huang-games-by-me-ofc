#include "EditorUI.h"
#include "Mesh.h"
#include <filesystem>
#include "imgui/misc/cpp/imgui_stdlib.h"
#include "Audio/AudioEngine.h"
#include "shared.h"
#include "imgui_internal.h"
using namespace Cle::Components;

namespace Cle::Editor
{
	Cle::Editor::EditorUI::EditorUI(Cle::World* World, GLFWwindow* m_window) : m_window(m_window), m_registry(World->registry), World(World)
	{
		ImGui::CreateContext();
		io = &ImGui::GetIO();
		io->ConfigFlags |= ImGuiConfigFlags_DockingEnable;


		io->Fonts->AddFontFromFileTTF("./fonts/OpenSans.ttf", 18.0f);

		ImGuiStyle& style = ImGui::GetStyle();
		ImGui::StyleColorsDark();
	//	style.Colors[ImGuiCol_WindowBg] = ImVec4(0.8, 0.8, 0.8, 1.0);
		//style.Colors[ImGuiCol_Text] = ImVec4(0.1, 0.1, 0.1, 1.0);
		//style.Colors[ImGuiCol_TitleBg] = ImVec4(0.8, 0.8, 0.8, 1.0);
		//style.Colors[ImGuiCol_TitleBgActive] = ImVec4(0.8, 0.8, 0.8, 1.0);
		style.WindowRounding = 5.0f;
		if (m_Pipeline == Cle::Gfx::Pipeline::OPENGL) {
			ImGui_ImplGlfw_InitForOpenGL(m_window, true);
			ImGui_ImplOpenGL3_Init("#version 330 core");

		}
	
	}
	void EditorUI::DrawChildren(entt::entity parent)
	{
		if (!m_registry->any_of<TreeInfo>(parent)) return;
		auto& children = m_registry->get<TreeInfo>(parent).getChildren();
		if (children.empty()) return;
		for (entt::entity child : children) {
			if (!m_registry->any_of<Name>(child)) {
				m_registry->emplace<Name>(child, "Untitled Object");
			}
			std::string name = m_registry->get<Cle::Components::Name>(child).getName();
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
	static bool explorerHovered = false;
	static bool propertiesHovered = false;
	void EditorUI::DrawExplorer()
	{
		
		ImGui::Begin("Explorer");
		explorerHovered = ImGui::IsWindowHovered(ImGuiHoveredFlags_AllowWhenBlockedByActiveItem);
	
	
	
		if (ImGui::TreeNodeEx("World", ImGuiTreeNodeFlags_DefaultOpen))
		{
			for (auto e : m_registry->view<TreeInfo>())
			{
				if (m_registry->get<TreeInfo>(e).getParent() != entt::null) continue;
				if (!m_registry->any_of<Name>(e)) m_registry->emplace<Name>(e, "Untitled Object");
				ImGui::PushID((int)e);

				if (m_Focused_Entity == e) ImGui::PushStyleColor(ImGuiCol_Header, ImVec4(0.7, 0.7, 1.0, 1.0));
				else ImGui::PushStyleColor(ImGuiCol_Header, ImGui::GetStyle().Colors[ImGuiCol_WindowBg]);
				bool open = ImGui::TreeNodeEx(m_registry->get<Cle::Components::Name>(e).getName().c_str(), ImGuiTreeNodeFlags_Framed);

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
		
		ImGui::Begin("Properties");
		propertiesHovered = ImGui::IsWindowHovered(ImGuiHoveredFlags_AllowWhenBlockedByActiveItem);

		if (m_Focused_Entity == entt::null)
		{
			ImGui::End();
			return;
		}
		Transform* transform = m_registry->try_get<Transform>(m_Focused_Entity);
		LightComponent* lightComponent = m_registry->try_get<LightComponent>(m_Focused_Entity);
		Color* color = m_registry->try_get<Color>(m_Focused_Entity);
		auto name = m_registry->try_get<Name>(m_Focused_Entity);

		auto mesh = m_registry->try_get<std::shared_ptr<Cle::GenericMesh>>(m_Focused_Entity);
		std::shared_ptr<Cle::Audio::Sound>* soundptr = m_registry->try_get<std::shared_ptr<Cle::Audio::Sound>>(m_Focused_Entity);

		Cle::Components::CubeMapTexture* cubeMap = m_registry->try_get<Cle::Components::CubeMapTexture>(m_Focused_Entity);
		
		if (name)
		{
			if (ImGui::TreeNodeEx("Name", ImGuiTreeNodeFlags_DefaultOpen))
			{
				std::string newname = name->getName();
				ImGui::InputText("Name", &newname);
				name->setName(newname);

				ImGui::TreePop();
			}
		}
		if (soundptr)
		{
			auto sound = *soundptr;
			if (ImGui::TreeNodeEx("Sound", ImGuiTreeNodeFlags_DefaultOpen))
			{
				std::string soundPath = sound->getPath();
				float timePosition = sound->getTimePosition();
				bool playing = sound->isPlaying();
				ImGui::DragFloat("Volume", &sound->volume);
				if (ImGui::InputText("Path", &soundPath) && glfwGetKey(m_window, GLFW_KEY_ENTER))
				{
					sound->setPath(soundPath);
				}
				if (ImGui::DragFloat("Time position", &timePosition) && glfwGetKey(m_window, GLFW_KEY_ENTER))
				{
					sound->setTimePosition(timePosition);
				}
				if (ImGui::Checkbox("Playing", &playing))
				{
					if (!playing) sound->Pause();
					else sound->Play();
				}
				if (ImGui::Checkbox("Global", &sound->global))
				{
				}
				ImGui::TreePop();

			}
		}
		if (transform)
		{
			if (ImGui::TreeNodeEx("Transform", ImGuiTreeNodeFlags_DefaultOpen))
			{
				auto pos = transform->getPosition();
				auto orien = glm::eulerAngles(transform->getOrientation());
				auto scale = transform->getScale();
				if (ImGui::DragFloat3("Position", (float*)&pos))
				{
					transform->setPosition(pos);

				}
				if (ImGui::DragFloat3("Orientation", (float*)&orien))
				{
					transform->setOrientation(glm::quat(orien));

				}
				if (ImGui::DragFloat3("Scale", (float*)&scale))
				{
					transform->setScale(scale);

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
		if (color)
		{
			if (ImGui::TreeNodeEx("Color", ImGuiTreeNodeFlags_DefaultOpen))
			{

				glm::vec3 Color = color->value;
				if (ImGui::ColorEdit3("Color", (float*)&Color))
				{
					color->value = glm::vec4(Color, 1);
				}
				ImGui::TreePop();

			}
		}

		if (mesh)
		{
			if (ImGui::TreeNodeEx("Mesh", ImGuiTreeNodeFlags_DefaultOpen))
			{
				std::string texturePath;
				std::string modelPath;
				auto gmesh = m_registry->get<std::shared_ptr<GenericMesh>>(m_Focused_Entity);
				//	texturePath = gmesh->texture ? gmesh->texture->getPath()  : "null";
				std::string path = gmesh->getModelPath();
				int mindex = gmesh->getMeshIndex();
				if (ImGui::InputText("Model path", &path) && glfwGetKey(m_window, GLFW_KEY_ENTER))
				{
					World->renderer.uploadMesh(m_Focused_Entity, std::make_shared<Cle::GenericMesh>(path, gmesh->getMeshIndex()), *m_registry);
				}
				if (ImGui::DragInt("Model mesh index", &mindex) && glfwGetKey(m_window, GLFW_KEY_ENTER))
				{
					World->renderer.uploadMesh(m_Focused_Entity, std::make_shared<Cle::GenericMesh>(path, mindex), *m_registry);

				}

				ImGui::TreePop();

			}
		}
		if (ImGui::TreeNodeEx("Network Visibility", ImGuiTreeNodeFlags_DefaultOpen))
		{
			if (ImGui::RadioButton("Client", m_registry->any_of<ClientOnly>(m_Focused_Entity)))
			{
				m_registry->emplace_or_replace< ClientOnly>(m_Focused_Entity);
				m_registry->remove<Replicated>(m_Focused_Entity);
				m_registry->remove<ServerOnly>(m_Focused_Entity);
			}
			if (ImGui::RadioButton("Server", m_registry->any_of<ServerOnly>(m_Focused_Entity)))
			{
				m_registry->emplace_or_replace< ServerOnly>(m_Focused_Entity);
				m_registry->remove<Replicated>(m_Focused_Entity);
				m_registry->remove<ClientOnly>(m_Focused_Entity);
			}
			if (ImGui::RadioButton("Replicated", m_registry->any_of<Replicated>(m_Focused_Entity)))
			{
				m_registry->emplace_or_replace<Replicated>(m_Focused_Entity);
				m_registry->remove<ClientOnly>(m_Focused_Entity);
				m_registry->remove<ServerOnly>(m_Focused_Entity);
			}
			ImGui::TreePop();
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
		if (ImGui::BeginPopupContextItem(("context" + std::to_string((int)e)).c_str()))
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
	void EditorUI::DrawGamePanel()
	{
		ImGui::Begin("Game");
		auto size = ImGui::GetWindowSize();
		if (!m_registry->ctx().contains<Camera>())
		{
			ImGui::End();
			return;
		}
		static auto& m_camera = m_registry->ctx().get<Camera>();

		m_camera.width = size.x;
		m_camera.height = size.y;
		m_camera.aspect = size.x / (float)size.y;
		mousePosRelativeToGame = glm::vec2(
			size.x*(ImGui::GetMousePos().x - ImGui::GetCursorScreenPos().x )/ ImGui::GetContentRegionAvail().x
			, size.y * (ImGui::GetMousePos().y - ImGui::GetCursorScreenPos().y) / ImGui::GetContentRegionAvail().y
			);


		ImGui::Image((ImTextureID)World->renderer.getImage(), ImGui::GetContentRegionAvail(),ImVec2(0,1),ImVec2(1,0));
		ImGui::End();
	}
	void EditorUI::DrawGizmo(entt::entity entity)
	{
		if (!m_registry->ctx().contains<Camera>()) return;
		auto& m_camera = m_registry->ctx().get<Camera>();
		
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
		ImGui::Begin("Game");

		ImGuizmo::SetDrawlist(ImGui::GetWindowDrawList());


		//glfwGetWindowSize(m_window, &width, &height);
		auto size = ImGui::GetWindowSize();
		auto pos = ImGui::GetWindowPos();
		ImGuizmo::SetRect(
			pos.x,
			pos.y,
			size.x,
			size.y
		);
		glm::mat4 modelCopy = transform->getRelativeModel(m_camera);

		ImGuizmo::Manipulate(
			glm::value_ptr(m_camera.getViewMatrix()),
			glm::value_ptr(m_camera.getProjection()),
			gizmoType,
			ImGuizmo::LOCAL,
			glm::value_ptr(modelCopy)
		);
		if (ImGuizmo::IsUsing()) {
			glm::vec3 nscale, npos, nrot;
			ImGuizmo::DecomposeMatrixToComponents(glm::value_ptr(modelCopy), glm::value_ptr(npos), glm::value_ptr(nrot), glm::value_ptr(nscale));\
				npos += m_camera.Position;
			transform->setScale(nscale);  transform->setOrientation(glm::radians(nrot)); transform->setPosition(npos);
			transform->dirty = true;
		}
		ImGui::End();
	}
	static void autodock(EditorUI* ui,ImGuiID& dockspace)
	{
		static ImGuiID leftDock, rightDock, rightBottom, center = 0;
		static bool docked = false;
		if (!docked)
		{
			ImGui::DockBuilderRemoveNode(dockspace);
			ImGui::DockBuilderAddNode(dockspace, ImGuiDockNodeFlags_DockSpace);
			ImGui::DockBuilderSetNodeSize(
				dockspace,
				ImGui::GetMainViewport()->WorkSize
			);
			center = dockspace;

			ImGui::DockBuilderSplitNode(center, ImGuiDir_Left, 0.30f, &leftDock, &center);
			ImGui::DockBuilderSplitNode(center, ImGuiDir_Right, 0.30f, &rightDock, &center);
		//	ImGui::DockBuilderSplitNode(center, ImGuiDir_Down, 0.25f, &rightBottom, &rightDock);

			ImGui::DockBuilderDockWindow("Game", center);

			ImGui::DockBuilderDockWindow("Explorer", rightDock);

			ImGui::DockBuilderDockWindow("Properties", leftDock);

			ImGui::DockBuilderFinish(dockspace);
			docked = true;
		}
	}
	void EditorUI::Update()
	{
		if (explorerHovered || propertiesHovered) pointerBusy = true;
		else pointerBusy = false;
		if (!m_registry->ctx().contains<Camera>())
		{
			std::cout << "no camera\n";
			m_registry->ctx().emplace<Camera>();
		}
		static bool autodocked = false;
		
		ImGui_ImplOpenGL3_NewFrame();
		ImGui_ImplGlfw_NewFrame();
		ImGui::NewFrame();
		ImGuizmo::BeginFrame();
		ImGuiID dockspace = ImGui::DockSpaceOverViewport(ImGui::GetMainViewport()->ID);
		autodock(this, dockspace);
		
		

		DrawGamePanel();
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