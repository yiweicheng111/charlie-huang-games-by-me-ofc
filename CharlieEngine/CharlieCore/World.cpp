#include "World.h"
#include <cereal/cereal.hpp>
#include <cereal/access.hpp>
#include <cereal/types/string.hpp>
#include <cereal/archives/binary.hpp>
using namespace Cle::Components;
using namespace Cle::Gfx;
entt::entity Cle::World::CreateDebugObject(const std::vector<Cle::Gfx::Vertex>& defaultVert, const std::vector<unsigned int>& indices)
{
	entt::entity charlie = registry->create();
	registry->emplace<Cle::Gfx::Material>(charlie);
	registry->get<Cle::Gfx::Material>(charlie).m_Shader.programID = renderer->getDefaultShader();

	registry->emplace<Cle::Gfx::GenericMesh>(charlie, defaultVert,indices);
	const Cle::Gfx::GenericMesh& m = registry->get<Cle::Gfx::GenericMesh>(charlie);
	registry->emplace<Cle::Components::Transform>(charlie);
	auto& t = registry->get<Cle::Components::Transform>(charlie);
	t.setPosition(m.offset);

	registry->emplace<Cle::Components::Name>(charlie, "charlie");
	registry->emplace<TreeInfo>(charlie);
	renderer->uploadMesh(charlie, *registry);
	return charlie;
}

entt::entity Cle::World::CreateDebugObject(const std::vector<Cle::Gfx::Vertex>& defaultVert, const std::vector<unsigned int>& indices, entt::entity Parent)
{
	entt::entity charlie = registry->create();
	registry->emplace<Cle::Gfx::Material>(charlie);
	registry->get<Cle::Gfx::Material>(charlie).m_Shader.programID = renderer->getDefaultShader();

	registry->emplace<Cle::Gfx::GenericMesh>(charlie, defaultVert,indices);
	const Cle::Gfx::GenericMesh& m = registry->get<Cle::Gfx::GenericMesh>(charlie);
	registry->emplace<Cle::Components::Transform>(charlie);
	auto& t = registry->get<Cle::Components::Transform>(charlie);
	t.setPosition(m.offset);
	registry->emplace<Cle::Components::Name>(charlie, "charlie");
	registry->emplace<TreeInfo>(charlie);
	renderer->uploadMesh(charlie, *registry);
	return charlie;

}
entt::entity  Cle::World::CreateDebugObject(const GenericMesh& GMesh)
{
	entt::entity charlie = registry->create();
	registry->emplace<Cle::Gfx::Material>(charlie);
	registry->get<Cle::Gfx::Material>(charlie).m_Shader.programID = renderer->getDefaultShader();

	registry->emplace<Cle::Gfx::GenericMesh>(charlie, GMesh);
	const Cle::Gfx::GenericMesh& m = registry->get<Cle::Gfx::GenericMesh>(charlie);
	registry->emplace<Cle::Components::Transform>(charlie);
	auto& t = registry->get<Cle::Components::Transform>(charlie);
	t.setPosition(m.offset);

	registry->emplace<Cle::Components::Name>(charlie, "charlie");
	registry->emplace<TreeInfo>(charlie);
	renderer->uploadMesh(charlie, *registry);
	return charlie;

}

entt::entity Cle::World::CopyObject(entt::entity existing)
{
	entt::entity newent = registry->create();
	if (registry->all_of<Transform>(existing)) {
		registry->emplace<Transform>(newent, registry->get<Transform>(existing));
	}
	if (registry->all_of<Name>(existing)) {
		registry->emplace<Name>(newent, registry->get<Name>(existing));
	}
	if (registry->all_of<Material>(existing)) {
		registry->emplace<Material>(newent, registry->get<Material>(existing));
	}
	if (registry->all_of<GenericMesh>(existing)) {
		registry->emplace<GenericMesh>(newent, registry->get<GenericMesh>(existing));
		renderer->uploadMesh(newent, *registry);
	}
	if (registry->all_of<TreeInfo>(existing)) {
		registry->emplace<TreeInfo>(newent, registry->get<TreeInfo>(existing));
	}
	return newent;
}


void Cle::World::DestroyObject(entt::entity existing)
{
	deleteObjectCallback();

	if (!registry->valid(existing)) return;

	if (registry->any_of<TreeInfo>(registry->get<TreeInfo>(existing).parent)) {
		std::vector<entt::entity>& pChildren = registry->get<TreeInfo>(registry->get<TreeInfo>(existing).parent).Children;
		pChildren.erase(std::remove(pChildren.begin(), pChildren.end(), existing), pChildren.end());
	}
	registry->destroy(existing);
}



namespace Cle::Components
{
	template <class Archive>
	void save(Archive& ar, const Transform& t)
	{
		auto pos = t.getPosition();
		auto oren = glm::eulerAngles(t.getOrientation());
		auto scale = t.getScale();
		ar(pos.x, pos.y, pos.z);
		ar(oren.x, oren.y, oren.z);
		ar(scale.x, scale.y, scale.z);
	}
	template <class Archive>
	void load(Archive& ar, Transform& t)
	{
		glm::vec3 pos, oren, scale;
		ar(pos.x, pos.y, pos.z);
		ar(oren.x, oren.y, oren.z);
		ar(scale.x, scale.y, scale.z);
		t.setPosition(pos);
		t.setOrientation(glm::quat(oren));
		t.setScale(scale);
		t.dirty = true;

	}
	template <class Archive>
	void save(Archive& ar, const Name& n)
	{
		ar(n.value);
	}
	template <class Archive>
	void load(Archive& ar, Name& n)
	{
		ar(n.value);
	}
	
}
namespace Cle::Gfx
{
	template <class Archive>
	void save(Archive& ar, const GenericMesh& m)
	{
		ar(m.loadedMeshIndex);
		ar(m.ModelPath);

	}
	template <class Archive>
	void load(Archive& ar, GenericMesh& m)
	{
		ar(m.loadedMeshIndex);
		std::string modelPath;
		ar(modelPath);
		m.ModelPath = modelPath;
	}
	template <class Archive>
	void save(Archive& ar, const Material& m)
	{

		glm::vec3 color = m.getColor();
		ar(color.x, color.y, color.z);

	}
	template <class Archive>
	void load(Archive& ar, Material& m)
	{
		glm::vec3 color{};

		ar(color.x, color.y, color.z);
		m.setColor(color);
	}
}
void Cle::World::Snapshot(std::string path)
{
	std::ofstream f(path, std::ios::binary);
	cereal::BinaryOutputArchive arch(f);
	entt::snapshot snapshot(*registry);
	snapshot
		.get<entt::entity>(arch)
		.get<Cle::Components::Transform>(arch)
		.get<Cle::Gfx::GenericMesh>(arch)
		.get<Cle::Components::Name>(arch)
		.get<Cle::Gfx::Material>(arch);
}

void Cle::World::LoadFile(std::string path)
{
	worldLoading = true;
	registry->clear();
	std::ifstream f(path, std::ios::binary);
	cereal::BinaryInputArchive arch(f);
	entt::snapshot_loader loader{ *registry };
	loader.get<entt::entity>(arch).
		get<Cle::Components::Transform>(arch).
		get<Cle::Gfx::GenericMesh>(arch).
		get<Cle::Components::Name>(arch).
		get<Cle::Gfx::Material>(arch).orphans();
	auto view = registry->view<GenericMesh, Cle::Gfx::Material>();



	view.each([&](auto entity,GenericMesh& mesh, auto&&...)
		{
			
			registry->emplace<TreeInfo>(entity);
			registry->get<Material>(entity).m_Shader.programID = renderer->getDefaultShader();
			registry->get<Cle::Gfx::Material>(entity).usesColorMap = false;
			if (mesh.ModelPath.empty()) return;
			std::vector< Cle::Gfx::GenericMesh> ModelLoaded = renderer->m_AssetHandler.LoadModel(mesh.ModelPath);
			mesh = ModelLoaded.at(mesh.loadedMeshIndex);
			renderer->uploadMesh(entity, *registry);
		});
	worldLoading = false;
}
