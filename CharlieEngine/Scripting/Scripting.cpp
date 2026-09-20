#include "Scripting.h"
#include "shared.h"
#include "World.h"
#include "Scripting/Event.h"
#include "UserInputService.h"
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
	
	lua["workspace"] = LuaEntity(world->Scene,registry);
	lua["client"] = LuaEntity(world->Client, registry);

	lua["replicated"] = LuaEntity(world->Replicated, registry);
	lua["server"] = LuaEntity(world->Server, registry);

	
}

void Cle::ScriptHandler::openLibraries()
{
	lua.open_libraries(sol::lib::base, sol::lib::coroutine, sol::lib::math);
	lua.new_usertype<Cle::Event>("Event",
		"Fire", &Cle::Event::Fire,
		"Connect", &Cle::Event::Connect
	);
	lua.new_usertype<Cle::UserInputService>("UserInputServiceType",
		"isKeyDown", &Cle::UserInputService::IsKeyDown
	);

	lua["UserInputService"] = Cle::UserInputService{ this->registry };
	lua.new_usertype<glm::vec4>("Vector4",
		sol::constructors<glm::vec4(), glm::vec4(float, float, float, float)>(),
		"x", &glm::vec4::x,
		"y", &glm::vec4::y,
		"z", &glm::vec4::z,
		"w", &glm::vec4::w
	);


	lua.new_usertype<glm::vec3>("Vector3",
		sol::constructors<glm::vec3(), glm::vec3(float, float, float)>(),
		"x", &glm::vec3::x,
		"y", &glm::vec3::y,
		"z", &glm::vec3::z
	);

	lua.new_usertype<LuaEntity>(
		"Entity",
		"Destroy",
		[this](LuaEntity& entity) {
			if (this->registry->valid(entity.entity))
				scriptPendingDestroy.push_back(entity.entity);
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

		"Parent", sol::property(
			[this](LuaEntity& entity) {
				auto v = this->registry->try_get<Cle::Components::TreeInfo>(entity.entity);

				if (!v) return sol::make_object(lua, sol::nil);


				return sol::make_object(lua, LuaEntity{ v->getParent(),this->registry });
			},
			[this](LuaEntity& entity, LuaEntity other) {

				if (!this->registry->try_get<Cle::Components::TreeInfo>(other.entity) || !this->registry->try_get<Cle::Components::TreeInfo>(entity.entity))
				{
					return;
				}

				this->registry->try_get<Cle::Components::TreeInfo>(entity.entity)->setParent(entity.entity, other.entity, this->registry);

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
			auto child = findFirstChild(key, this->registry, this->lua);
			if (child != sol::nil)
				return child;

			auto& holder = registry->get_or_emplace<Cle::EventHolder>(entity.entity, entity.entity, registry);
			return sol::make_object(lua, std::ref(holder.getOrMakeEvent(key)));
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
	lua["Instance"] = lua.create_table();
	lua["Instance"]["new"] = [this](const std::string& className, sol::optional<LuaEntity> parent) {
		entt::entity e = entt::null;
		if (className == "Part")
		{
			e = this->world->addModelToScene("primitives/cube.gltf")[0];
		}
		if (className == "Event")
		{
			auto& events = this->registry->get_or_emplace<Cle::EventHolder>(parent->entity, parent->entity, this->registry);
			return sol::make_object(lua, events.getOrMakeEvent(className));
		}
		auto& name = this->registry->emplace_or_replace<Cle::Components::Name>(e, className);
		auto& tree = this->registry->emplace_or_replace<Cle::Components::TreeInfo>(e);
		if (parent && this->registry->valid(parent->entity))
		{
			tree.setParent(e, parent->entity, this->registry);
		}
		return sol::make_object(lua, LuaEntity(e, this->registry));

		};
	lua["tick"] = []() {
		return std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::system_clock::now().time_since_epoch()).count();
		};
}
void Cle::ScriptHandler::run(double currentTime, double dt)

{

	std::vector<entt::entity> entities;
	entities.reserve(registry->storage<Script>().size());
	for (auto e : registry->view<Script>()) entities.push_back(e);


	for (auto& e : entities)
	{

		if (!registry->valid(e)) continue;         
		if (!registry->all_of<Script>(e)) continue;
		auto& script = registry->get<Script>(e);
		if (script.ran) continue;
		if (!script.enabled) continue;

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
					sol::error err = result;
					std::cout << err.what() << std::endl;
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
		auto result = registry->get<Script>(e).cor();
		if (!registry->valid(e) || !registry->all_of<Script>(e))
		{
			continue;
		}
		auto& scriptAfter = registry->get<Script>(e);
		if (!result.valid())
		{
			sol::error err = result;
			std::cout << err.what() << std::endl;
			scriptAfter.ran = true;
			continue;
		}
		if (scriptAfter.cor.status() == sol::call_status::yielded)
		{
			scriptAfter.resumeTime = currentTime + result.get<float>();
		}
		else {
			scriptAfter.ran = true;

		}
		
		for (auto e : scriptPendingDestroy)
		{
			if (registry->valid(e))
				registry->destroy(e);
		}
		scriptPendingDestroy.clear();
	}

}

Cle::ScriptHandler::ScriptHandler()
{
	
	

}
