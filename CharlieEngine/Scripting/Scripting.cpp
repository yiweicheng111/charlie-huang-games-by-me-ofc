#include "Scripting.h"
#include "shared.h"
#include "World.h"
using namespace Cle;
static sol::object findFirstChild(const std::string& name,entt::registry* registry,sol::state_view lua)
{
	for (auto i : registry->view<Cle::Components::Name>())
	{
		auto n = registry->get<Cle::Components::Name>(i);
		if (n.getName() == name)
		{
			return sol::make_object(lua,LuaEntity{ i,registry});
		}
	}
	return sol::nil;
}
void Cle::ScriptHandler::setVariables(World* world,entt::registry* registry)
{
	this->world = world;
	this->registry = registry;
	lua.open_libraries(sol::lib::base,sol::lib::coroutine);



	lua.new_usertype<glm::vec3>("Vector3",
		sol::constructors<glm::vec3(),glm::vec3(float,float,float)>(),
		"x", &glm::vec3::x,
		"y", &glm::vec3::y,
		"z", &glm::vec3::z
	);

	lua.new_usertype<LuaEntity>(
		"Entity",
		"GetChildren", [this](LuaEntity& entity) {
			auto v = this->registry->try_get<Cle::Components::TreeInfo>(entity.entity);
			std::vector< LuaEntity> lv;

			if (!v) return sol::as_table(lv);

			for (auto& ent : v->getChildren())
			{
				lv.push_back({ ent,this->registry });
			}
			return sol::as_table(lv);
		},
		"Parent", sol::property([this](LuaEntity& entity) {
			auto v = this->registry->try_get<Cle::Components::TreeInfo>(entity.entity);

			if (!v) return sol::make_object(lua,sol::nil);

		
			return sol::make_object(lua, LuaEntity{ v->getParent(),this->registry});
		}),
	
		sol::meta_function::index,
		[this](LuaEntity& entity, const std::string& key) -> sol::object
		{

			auto registry = this->registry;
			if (entity.entity == entt::null ||
				!registry->valid(entity.entity))
			{
				return sol::make_object(lua, sol::nil);
			}
			for (const auto& component : registeredComponents)
			{
		

				if (component.name != key)
					continue;


				auto storage = registry->storage(component.id);

				if (!storage || !storage->contains(entity.entity))
					return sol::make_object(lua, sol::nil);

				return sol::make_object(
					lua,
					LuaComponent{
						entity.entity,
						component.id,
						registry
					}
				);
			}

			return findFirstChild(key,this->registry,this->lua);
			
		},
		sol::meta_function::new_index,
	
		[this](LuaEntity& entity, const std::string& key, sol::object value) 
		{
			
			auto registry = this->registry;
			if (entity.entity == entt::null ||
				!registry->valid(entity.entity))
			{
				return;
			}

			if (key == "Parent"){

				if (!value.is<LuaEntity>()) return;
				LuaEntity newParent = value.as<LuaEntity>();

				auto v = this->registry->try_get<Cle::Components::TreeInfo>(entity.entity);
				auto othert = this->registry->try_get<Cle::Components::TreeInfo>(newParent.entity);

				if (!v || !othert) return;
				if (this->registry->valid(othert->getParent()))
				{
					v->setParent(entity.entity, newParent.entity, this->registry);
				}
		
			}
		
			for (const auto& component : registeredComponents)
			{


				if (component.name != key)
					continue;


				auto storage = registry->storage(component.id);

				if (!storage || !storage->contains(entity.entity))
					return;
				setLuaProperty(*this->registry, entity.entity, component.id, key, value);
				return;
			}

		}
	);
	lua.new_usertype<LuaComponent>(
		"Component",

		sol::meta_function::index,
		[this](LuaComponent& component,
			const std::string& key) -> sol::object
		{
			return getLuaProperties(
				*component.registry,
				component.entity,
				component.type,
				key,
				lua
			);
		},
		sol::meta_function::new_index,
		[this](LuaComponent& component,
			const std::string& key,
			sol::object value)
		{
			setLuaProperty(
				*component.registry,
				component.entity,
				component.type,
				key,
				value
			);
		}
	);
	lua["workspace"] = LuaEntity(world->Scene,registry);


	lua["tick"] = []() {
		return std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::system_clock::now().time_since_epoch()).count();
		};

}


void Cle::ScriptHandler::run()
{

	for (auto& e : registry->view<Script>())
	{

		auto& script = registry->get<Script>(e);
		if (script.ran) continue;

		try
		{
			lua["script"] = LuaEntity(e, registry);
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
