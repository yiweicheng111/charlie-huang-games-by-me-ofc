

#include "gameIO.h"
#include <fstream>
#include "Mesh.h"
#include "Material.h"
#include "Components/Components.h"
#include "shared.h"
using namespace Cle::Components;
void Cle::gameIO::Snapshot(std::string path)
{

	std::ofstream f(path, std::ios::binary);
	cereal::BinaryOutputArchive arch(f);
	entt::snapshot snapshot(*registry);

	snapshot
		.get<entt::entity>(arch)
		.get<Cle::Components::Transform>(arch)
		.get<std::shared_ptr<Cle::Gfx::IMesh>>(arch)
		.get<Cle::Components::Name>(arch)
		.get<Cle::Gfx::Material>(arch);
}

void Cle::gameIO::LoadFile(std::string path)
{
	if (!registry)
	{
		std::cout << "no registry or renderer to load\n";
		return;
	}
	registry->clear();
	std::ifstream f(path, std::ios::binary);
	cereal::BinaryInputArchive arch(f);
	entt::snapshot_loader loader{ *registry };
	loader.get<entt::entity>(arch).
		get<Cle::Components::Transform>(arch).
		get<std::shared_ptr<Cle::Gfx::IMesh>>(arch).
		get<Cle::Components::Name>(arch).
		get<Cle::Gfx::Material>(arch).orphans();
	auto view = registry->view<std::shared_ptr<Cle::Gfx::IMesh>, Cle::Gfx::Material, Cle::Components::Transform>();
	Cle::Gfx::Camera cam;

	view.each([&](const entt::entity entity, std::shared_ptr<Cle::Gfx::IMesh>& mesh, Cle::Gfx::Material& material, Cle::Components::Transform& transform)
		{
			
			registry->emplace<TreeInfo>(entity);
		//	material.m_Shader = renderer->getDefaultShader();

		 //  const auto& ModelLoaded = renderer->m_AssetHandler.LoadModel(mesh->gMesh.ModelPath);
	
			//renderer->uploadMesh(entity, ModelLoaded.at(mesh->gMesh.loadedMeshIndex), *registry);
			//auto& nMesh = registry->get<std::shared_ptr<Cle::Gfx::IMesh>>(entity);


			//if (nMesh->gMesh.texture) nMesh->gMesh.texture = renderer->createTexture(nMesh->gMesh.texture->getPath());
			//if (material.getColorMap()) material.setColorMap(renderer->createTexture(material.getColorMap()->getPath()));
			//renderer->drawMesh(entity, *registry,cam);


		});
}