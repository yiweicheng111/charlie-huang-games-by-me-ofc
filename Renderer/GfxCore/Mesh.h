#pragma once
#include <vector>
#include "CharlieEngine/Vertex.h"
#include "assimp/Importer.hpp"
#include "assimp/scene.h"
#include "assimp/postprocess.h"
#include "CharlieEngine/Components.h"
#include <string>
#include "ITexture.h"
#include <entt/entt.hpp>
#include <cereal/cereal.hpp>
#include <cereal/access.hpp>
#include <cereal/types/polymorphic.hpp>
#include <cereal/types/string.hpp>
#include <cereal/archives/binary.hpp>
#include <GfxBase.h>
#include "Material.h"
#include "CharlieEngine/GenericMesh.h"
namespace Cle::Gfx
{
	
	class IMesh {
	protected:
		std::vector<std::shared_ptr<IMesh>> LODmeshes;
	public:
		virtual ~IMesh() = default;
		bool gpuUploaded = false;
		int indexCount{};
		int vertexCount{};
		IMesh() = default;
		virtual void draw() = 0;
	};
}