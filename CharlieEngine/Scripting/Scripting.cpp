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
	return sol::make_object(lua, sol::nil);
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
	lua.open_libraries(sol::lib::base, sol::lib::coroutine, sol::lib::math, sol::lib::table);
	lua.new_usertype<Cle::Event>("Event",
		"Fire", &Cle::Event::Fire,
		"Connect", &Cle::Event::Connect
	);
	lua.new_usertype<Cle::EventHolder>(
		"EventComponent",
		"Remove",
		[this](EventHolder& component, const std::string& key) {
			if (!component.registry ||
				!component.registry->valid(component.entity))
			{
				return;

			}
			component.removeEvent(key);
		},
		sol::meta_function::index,

		[this](EventHolder& component,
			const std::string& key) -> sol::object
		{
			if (!component.registry ||
				!component.registry->valid(component.entity))
			{
				return sol::make_object(lua, sol::nil);
			}

			auto& holder =
				component.registry->get_or_emplace<Cle::EventHolder>(
					component.entity,
					component.entity,
					component.registry
				);

			return sol::make_object(
				lua,
				std::ref(holder.getOrMakeEvent(key))
			);
		}
	);
	lua.new_usertype<Cle::UserInputService>("UserInputServiceType",
		"isKeyDown", &Cle::UserInputService::IsKeyDown,
		"getCursorPos", &Cle::UserInputService::getCursorPos,
		"isMouseButtonDown", &Cle::UserInputService::IsMouseButtonDown
	);

	lua["UserInputService"] = Cle::UserInputService{ this->registry };
	lua.new_usertype<glm::vec4>("Vector4",
		sol::constructors<glm::vec4(), glm::vec4(float, float, float, float)>(),
		"x", &glm::vec4::x,
		"y", &glm::vec4::y,
		"z", &glm::vec4::z,
		"w", &glm::vec4::w,
		"Magnitude", sol::property(&glm::vec4::length),
		sol::meta_function::addition, [](const glm::vec4& a, const glm::vec4& b) {
			return a + b;
		},
		sol::meta_function::subtraction, [](const glm::vec4& a, const glm::vec4& b) {
			return a - b;
		}
	);
	lua.new_usertype<glm::vec2>("Vector2",
		sol::constructors<glm::vec2(), glm::vec2(float, float)>(),
		"x", &glm::vec2::x,
		"y", &glm::vec2::y,

		"Magnitude", sol::property(&glm::vec2::length),
		sol::meta_function::addition, [](const glm::vec2& a, const glm::vec2& b) {
			return a + b;
		},
		sol::meta_function::subtraction, [](const glm::vec2& a, const glm::vec2& b) {
			return a - b;
		}
	);

	lua.new_usertype<glm::vec3>("Vector3",
		sol::constructors<glm::vec3(), glm::vec3(float, float, float)>(),
		"x", &glm::vec3::x,
		"y", &glm::vec3::y,
		"z", &glm::vec3::z,
		"Magnitude", sol::property(&glm::vec3::length),
		sol::meta_function::addition, [](const glm::vec3& a, const glm::vec3& b) {
			return a + b;
		},
		sol::meta_function::subtraction, [](const glm::vec3& a, const glm::vec3& b) {
			return a - b;
		}
	);
	lua.new_usertype<Cle::Gfx::Camera>("CameraType",
		"Position", &Cle::Gfx::Camera::Position,
		"Forward", sol::property(&Cle::Gfx::Camera::getForward),
		"LookAt", &Cle::Gfx::Camera::lookAt
	);

	lua.new_usertype<LuaEntity>(
		"Entity",
		"Destroy",
		[this](LuaEntity& entity) {
			if (this->registry->valid(entity.entity))
				scriptPendingDestroy.push_back(entity.entity);
		},
		"AddComponent",
		[this](LuaEntity& entity, const std::string& cname) -> sol::object {
			if (entity.entity == entt::null || !this->registry->valid(entity.entity))
				 	return sol::make_object(
					lua,
					sol::nil
				);

			for (const auto& component : registeredComponents)
			{
				if (component.name != cname) continue;

				if (component.emplace)
					component.emplace(*this->registry, entity.entity);   

				return sol::make_object(lua, LuaComponent{ entity.entity, component.id, registry });
			}
			return sol::make_object(lua, sol::nil);

		},
		"GetComponent",
		[this](LuaEntity& entity, const std::string& cname) -> sol::object {

			if (entity.entity == entt::null || !this->registry->valid(entity.entity))
				return sol::make_object(lua,sol::nil);
			if (cname == "Event")
			{
				auto* events = registry->try_get<EventHolder>(entity.entity);

				if (!events)
				{
					events = &registry->emplace<EventHolder>(
						entity.entity,
						entity.entity,
						registry
					);
				}

				return sol::make_object(lua, std::ref(*events));
			}
			for (const auto& component : registeredComponents)
			{
				if (component.name != cname)
					continue;

				auto* storage = this->registry->storage(component.id);

				if (!storage)

					return sol::make_object(
						lua,
						sol::nil
					);

				return sol::make_object(
					lua,
					LuaComponent{
						entity.entity,
						component.id,
						registry
					}
				);

			}
			return sol::make_object(
				lua,
				sol::nil
			);
		},
		"RemoveComponent",
		[this](LuaEntity & entity, const std::string& cname){
			if (entity.entity == entt::null || !this->registry->valid(entity.entity))
				return false;
		
			for (const auto& component : registeredComponents)
			{
				if (component.name != cname)
					continue;

				auto* storage = this->registry->storage(component.id);

				if (!storage)
					return false;

				return storage->remove(entity.entity);
			}

			return false;
		},
		"Clone",
		[this](LuaEntity& entity) {
			return LuaEntity(world->CopyObject(entity.entity), entity.registry);
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
			auto child = findFirstChild(key, this->registry, this->lua);
			if (child != sol::nil)
				return child;
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
	
			return sol::make_object(lua, sol::nil);


/*

			auto& holder = registry->get_or_emplace<Cle::EventHolder>(entity.entity, entity.entity, registry);
			return sol::make_object(lua, std::ref(holder.getOrMakeEvent(key)));*/
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
	lua["LocalPlayer"] = lua.create_table();
	if (registry->ctx().contains<Cle::Gfx::Camera>()) {
		lua["LocalPlayer"]["Camera"] = &registry->ctx().get<Cle::Gfx::Camera>();
	}
	else {
		lua["Camera"] = sol::nil;
	}
	lua["Instance"] = lua.create_table();
	
	lua["Instance"]["new"] = [this](const std::string& className, sol::optional<LuaEntity> parent, sol::optional<std::string> name) {
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
		auto& tree = this->registry->emplace_or_replace<Cle::Components::TreeInfo>(e);
		if (parent && this->registry->valid(parent->entity))
		{
			tree.setParent(e, parent->entity, this->registry);
		}
		else
		{
			tree.setParent(e, world->Scene, this->registry);

		}
		if (name)
		{
			registry->emplace_or_replace<Name>(e, Name(className));
		}
		else
		{
			this->registry->emplace_or_replace<Cle::Components::Name>(e, Name(className));
		}
		return sol::make_object(lua, LuaEntity(e, this->registry));

		};
	lua["tick"] = []() {
		return std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::system_clock::now().time_since_epoch()).count();
		};
}
void Cle::ScriptHandler::run(double currentTime, double dt)

{
	if (Cle::gameIO::getInstance().loading) return;
//	std::cout << "used memory " << lua.memory_used() << std::endl;

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
