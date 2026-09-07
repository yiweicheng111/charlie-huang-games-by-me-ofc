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
	lua.open_libraries(sol::lib::base,sol::lib::coroutine,sol::lib::math);

	lua.new_usertype<glm::vec4>("Vector4",
		sol::constructors<glm::vec4(), glm::vec4(float, float, float,float)>(),
		"x", &glm::vec4::x,
		"y", &glm::vec4::y,
		"z", &glm::vec4::z,
		"w", &glm::vec4::w
	);


	lua.new_usertype<glm::vec3>("Vector3",
		sol::constructors<glm::vec3(),glm::vec3(float,float,float)>(),
		"x", &glm::vec3::x,
		"y", &glm::vec3::y,
		"z", &glm::vec3::z
	);

	lua.new_usertype<LuaEntity>(
		"Entity",
		"Destroy",
		[this](LuaEntity& entity) {
			if (this->registry->valid(entity.entity))
			{
				this->registry->destroy(entity.entity);
			}
		},
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
				auto meta = entt::resolve(component.id);
				if (!meta) return sol::make_object(lua, sol::nil);
				int propcount = 0;
				entt::id_type onlypropid;
				for (auto&& [propId, propData] : meta.data())
				{
					propcount++;
					onlypropid = propId;
				}
				if (propcount == 1)
				{
					entt::meta_any instance = meta.from_void(storage->value(entity.entity));
					entt::meta_any value = meta.data(onlypropid).get(instance);
					return Cle::MetaToLua(value, lua);
				}
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
				auto meta = entt::resolve(component.id);
				if (!meta) return;
				int propcount = 0;
				entt::id_type onlypropid;
				for (auto&& [propId, propData] : meta.data())
				{
					propcount++;
					onlypropid = propId;
				}
				if (propcount == 1)
				{
				
					return 	setLuaProperty(*this->registry, entity.entity, component.id, propertyNames[onlypropid], value);
				}
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


void Cle::ScriptHandler::run(double currentTime, double dt)

{

	for (auto& e : registry->view<Script>())
	{

		auto& script = registry->get<Script>(e);
		if (script.ran) continue;

		try
		{
			if (!script.started)
			{
				script.thread  = sol::thread::create(lua.lua_state());
				sol::state_view state = script.thread.state();
				state["script"] = LuaEntity(e, registry);

				sol::load_result result = state.load_file(script.path);
				if (!result.valid())
				{
					script.ran = true;
					continue;
				}

				script.cor = result;
				script.started = true;
				script.resumeTime = 0;
			}
		}
		catch (std::exception e)
		{
			std::cout << e.what() << std::endl;
			continue;
		}
		if (currentTime < script.resumeTime) continue;
		auto result = script.cor();
		if (script.cor.status() == sol::call_status::yielded)
		{
			script.resumeTime = currentTime + result.get<float>();
		}
		else {
			script.ran = true;

		}
		

	}

}

Cle::ScriptHandler::ScriptHandler()
{
	
	

}
