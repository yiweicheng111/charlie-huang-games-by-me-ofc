#include "World.h"
#include <cereal/cereal.hpp>
#include <cereal/access.hpp>
#include <cereal/types/string.hpp>
#include <cereal/archives/binary.hpp>
#include "shared.h"
#include "Audio/AudioEngine.h"
using namespace Cle::Components;
using namespace Cle::Gfx;



entt::entity Cle::World::CreateDebugObject(std::shared_ptr<Cle::GenericMesh> GMesh)
{
	entt::entity charlie = registry->create();
	registry->emplace < Cle::Components::TreeInfo > (charlie);
	registry->emplace<MaterialRef>(charlie);
	registry->emplace<Color>(charlie);

	//registry->get<Cle::Gfx::Material>(charlie).m_Shader.programID = renderer.getDefaultShader();

	registry->emplace<Cle::Components::Transform>(charlie);
	auto& t = registry->get<Cle::Components::Transform>(charlie);
	registry->emplace<Cle::Components::Name>(charlie, "charlie");
	registry->emplace<Replicated>(charlie);

	renderer.uploadMesh(charlie, GMesh, *registry);

	auto& m = registry->get<std::shared_ptr<GenericMesh>>(charlie);
	t.setPosition(m->positionOffset);
	t.setScale(m->scaleOffset);
	t.setOrientation(m->orientationOffset);

	return charlie;

}

entt::entity Cle::World::CopyObject(entt::entity existing)
{
	entt::entity newent = registry->create();
	if (registry->any_of<Transform>(existing)) {
		registry->emplace<Transform>(newent, registry->get<Transform>(existing));
	}
	if (registry->any_of<Name>(existing)) {
		registry->emplace<Name>(newent, registry->get<Name>(existing));
	}
	if (registry->any_of<Color>(existing)) {
		registry->emplace<Color>(newent, registry->get<Color>(existing));
	}
	if (registry->any_of<MaterialRef>(existing)) {
		registry->emplace<MaterialRef>(newent, registry->get<MaterialRef>(existing));
	}
	if (registry->any_of<std::shared_ptr<GenericMesh>>(existing)) {
		renderer.uploadMesh(newent, registry->get<std::shared_ptr<GenericMesh>>(existing), *registry);
	}
	if (registry->any_of<TreeInfo>(existing)) {
		registry->emplace<TreeInfo>(newent, registry->get<TreeInfo>(existing));
	}
	if (registry->any_of<Replicated>(existing)) {
		registry->emplace<Replicated>(newent);
	}
	if (registry->any_of<ServerOnly>(existing)) {
		registry->emplace<ServerOnly>(newent);
	}
	if (registry->any_of<ClientOnly>(existing)) {
		registry->emplace<ClientOnly>(newent);
	}
	if (registry->any_of< std::shared_ptr<Cle::Audio::Sound>>(existing)) {
		auto& audio = registry->get< std::shared_ptr<Cle::Audio::Sound>>(existing);
		registry->emplace<std::shared_ptr<Cle::Audio::Sound>>(newent, std::make_shared<Cle::Audio::Sound>(*audio));
	}

	return newent;
}


void Cle::World::DestroyObject(entt::registry& registry,entt::entity existing)
{
	deleteObjectCallback();

	if (registry.any_of<TreeInfo>(existing) && registry.valid(registry.get<TreeInfo>(existing).getParent()) && registry.any_of<TreeInfo>(registry.get<TreeInfo>(existing).getParent())) {
		registry.get<TreeInfo>(registry.get<TreeInfo>(existing).getParent()).removeChild(existing, &registry);
	}
}



void Cle::World::Snapshot(std::string path)
{
	/*std::ofstream f(path, std::ios::binary);
	cereal::BinaryOutputArchive arch(f);
	entt::snapshot snapshot(*registry);
	auto view = registry->view<std::unique_ptr<IMesh>, Cle::Gfx::Material>();
	view.each([&](const entt::entity entity, std::unique_ptr<IMesh>& mesh, Cle::Gfx::Material& material) {
		Cle::MeshPacket meshpacket;
		auto& gmesh = mesh->gMesh;
		meshpacket.setMeshIndex(gmesh.loadedMeshIndex);
		meshpacket.setPath(gmesh.ModelPath);
		if (gmesh.texture) meshpacket.setTexturePath(gmesh.texture->getPath());
		registry->emplace_or_replace<Cle::MeshPacket>(entity, meshpacket);

		Cle::MaterialPacket matpacket;
		matpacket.setColor(material.getColor());

		if (material.getColorMap()) matpacket.setColorMap(material.getColorMap()->getPath());

		registry->emplace_or_replace<Cle::MaterialPacket>(entity, matpacket);

		});
	snapshot
		.get<entt::entity>(arch)
		.get<Cle::Components::Transform>(arch)
		.get<Cle::MeshPacket>(arch)
		.get<Cle::Components::Name>(arch)
		.get<Cle::MaterialPacket>(arch);*/
}

void Cle::World::LoadFile(std::string path)
{
	/*registry->clear();
	std::ifstream f(path, std::ios::binary);
	cereal::BinaryInputArchive arch(f);
	entt::snapshot_loader loader{ *registry };
	loader.get<entt::entity>(arch).
		get<Cle::Components::Transform>(arch).
		get<Cle::MeshPacket>(arch).
		get<Cle::Components::Name>(arch).
		get<Cle::MaterialPacket>(arch).orphans();
	auto view = registry->view<Cle::MeshPacket, Cle::MaterialPacket>();



	view.each([&](const entt::entity entity, auto& meshpacket, auto& materialpacket)
		{

			registry->emplace<TreeInfo>(entity);
			auto& material = registry->emplace<Cle::Gfx::Material>(entity, renderer.getDefaultShader());
			material.setColor(materialpacket.getColor());
			material.setColorMap(renderer.createTexture(materialpacket.getColorMap()));

			std::vector< Cle::GenericMesh> ModelLoaded = renderer.m_AssetHandler.LoadModel(meshpacket.getPath());
			auto& mesh = registry->emplace<Cle::GenericMesh>(entity, ModelLoaded.at(meshpacket.getMeshIndex()));
			mesh.texture = renderer.createTexture(meshpacket.getTexturePath());
			renderer.uploadMesh(entity, *registry);

			if (registry->any_of<Cle::Components::Transform>(entity))
			{
				auto& t = registry->get<Cle::Components::Transform>(entity);
				t.setPosition(mesh.offset);
			}
		});*/
}