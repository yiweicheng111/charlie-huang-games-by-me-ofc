#pragma once
#include <entt/entt.hpp>
#include <cereal/cereal.hpp>
#include <cereal/access.hpp>
#include <cereal/types/string.hpp>
#include <cereal/archives/binary.hpp>
#include "IRenderer.h"
#include "Audio/AudioEngine.h"
namespace Cle
{
	class gameIO
	{
	private:
		gameIO() = default;
		entt::registry* registry;
	public:
		gameIO(const gameIO&) = delete;
		gameIO& operator=(const gameIO&) = delete;

		gameIO(gameIO&&) = delete;
		gameIO& operator=(gameIO&&) = delete;
		static gameIO& getInstance()
		{
			static gameIO instance;
			return instance;
		}
		void setRegistry(entt::registry* registry) {
			this->registry = registry;
		}

		void Snapshot(std::string file);
		void LoadFile(std::string file);
	};
}