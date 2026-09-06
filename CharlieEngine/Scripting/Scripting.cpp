#include "Scripting.h"
#include "Scripting.h"
#include "shared.h"
#include "World.h"
using namespace Cle;
void Cle::ScriptHandler::setVariables(World* world,entt::registry* registry)
{
	this->world = world;
	this->registry = registry;
	lua.open_libraries(sol::lib::base,sol::lib::coroutine);
	lua.new_usertype< Cle::Components::Transform>("Transform",
		"position", sol::property(&Cle::Components::Transform::getPosition, &Cle::Components::Transform::setPosition)
	);

	lua.new_usertype<glm::vec3>("Vector3",
		sol::constructors<glm::vec3(),glm::vec3(float,float,float)>(),
		"x", &glm::vec3::x,
		"y", &glm::vec3::y,
		"z", &glm::vec3::z
	);

	lua.new_usertype<LuaEntity>(
		"Entity",

		sol::meta_function::index,
		[this](LuaEntity& entity, const std::string& key) -> sol::object
		{
			if (entity.entity == entt::null ||
				!this->registry->valid(entity.entity))
			{
				return sol::make_object(lua, sol::nil);
			}

			if (key == "transform")
			{
				if (!this->registry->all_of<Cle::Components::Transform>(entity.entity))
				{
					return sol::make_object(lua, sol::nil);
				}

				Cle::Components::Transform& transform =
					this->registry->get<Cle::Components::Transform>(entity.entity);

				return sol::make_object(
					lua,
					std::ref(transform)
				);
			}

			auto children = getchildren(key);

			if (!children.empty())
			{
				return sol::make_object(lua, children[0]);
			}

			return sol::make_object(lua, sol::nil);
		}
	);

	lua["workspace"] = LuaEntity(world->Scene,registry);
	lua["tick"] = []() {
		return std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::system_clock::now().time_since_epoch()).count();
		};

}
std::vector<LuaEntity> Cle::ScriptHandler::getchildren(const std::string& name)
{
	std::vector<LuaEntity> found;
	for (const auto& e : registry->view<Cle::Components::Name>())
	{
		if (registry->get<Cle::Components::Name>(e).getName() == name) found.push_back({e,registry});
	}
	return found;
}

void Cle::ScriptHandler::run()
{

	for (auto& e : registry->view<Script>())
	{

		auto& script = registry->get<Script>(e);
		if (script.ran) continue;

		try
		{
			lua.script_file(script.path);
		}
		catch (std::exception e)
		{
			std::cout << e.what() << std::endl;
			continue;
		}
		script.ran = true;
		

	}

}

Cle::ScriptHandler::ScriptHandler()
{
	
	

}
