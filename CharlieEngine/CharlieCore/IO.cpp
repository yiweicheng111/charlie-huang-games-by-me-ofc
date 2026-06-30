

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
		.get<std::shared_ptr<GenericMesh>>(arch)
		.get<Cle::Components::MaterialRef>(arch)
		//.get<std::shared_ptr<Cle::Gfx::ITexture>>(arch)
		.get<Cle::Components::Color>(arch)
		.get<Cle::Components::Name>(arch);

}

void Cle::gameIO::LoadFile(std::string path)
{
	if (!registry)
	{
		std::cout << "no registry\n";
		return;
	}
	registry->clear();
	std::ifstream f(path, std::ios::binary);
	cereal::BinaryInputArchive arch(f);
	entt::continuous_loader loader{ *registry };
	loader
		.get<entt::entity>(arch)
		.get<Cle::Components::Transform>(arch)
		.get<std::shared_ptr<GenericMesh>>(arch)
		.get<Cle::Components::MaterialRef>(arch)
	//	.get<std::shared_ptr<Cle::Gfx::ITexture>>(arch)
		.get<Cle::Components::Color>(arch)
		.get<Cle::Components::Name>(arch).orphans();
	for (auto ent : registry->view<Cle::Components::Name>())
	{
		registry->emplace_or_replace<Cle::Components::TreeInfo>(ent);
	}

}