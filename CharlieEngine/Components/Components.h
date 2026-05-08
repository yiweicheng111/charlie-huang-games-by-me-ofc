#pragma once
#include <string>
#include <glm/glm.hpp>
#include <functional>
#include "GfxBase.h"
#include "Camera.h"
#include <entt/entt.hpp>
#include "Transform.h"
namespace Cle::Components
{
	struct Name
	{
		std::string value{};
		Name(std::string value) : value(value) {}
	};
	struct TreeInfo {
		entt::entity parent = entt::null;
		std::vector<entt::entity> Children;
	};
	struct Ray 
	{
		Ray() = default;
		glm::vec3 origin{};
		glm::vec3 dir{};
		void SetFromPointer(double mx, double my,int screenWidth, int screenHeight, Cle::Gfx::Camera& camera);
		Ray(glm::vec3 origin, glm::vec3 dir) : dir(dir), origin(origin) {}
	};
	struct AABB
	{
		glm::vec3 max{};
		glm::vec3 min{};
		AABB() = default;
		
		AABB (std::vector<Cle::Gfx::Vertex>& vertices, glm::mat4 model) {
			min = glm::vec3(std::numeric_limits<float>::max());
			max = glm::vec3(std::numeric_limits<float>::min());
			for (auto& verts : vertices) {
				glm::vec3 world = glm::vec3(model * glm::vec4(verts.Position, 1.0f));
				min = glm::min(min, world);
				max = glm::max(min, world);
			}
		}
		AABB(std::vector<Cle::Gfx::Vertex>& vertices) {
			min = glm::vec3(std::numeric_limits<float>::max(), std::numeric_limits<float>::max(), std::numeric_limits<float>::max());
			max = glm::vec3(std::numeric_limits<float>::min(), std::numeric_limits<float>::min(), std::numeric_limits<float>::min());
			for (Cle::Gfx::Vertex& v : vertices) {
				min.x = std::min(min.x, v.Position.x);
				min.y = std::min(min.y, v.Position.y);
				min.z = std::min(min.z, v.Position.z);

				max.x = std::max(max.x, v.Position.x);
				max.y = std::max(max.y, v.Position.y);
				max.z = std::max(max.z, v.Position.z);
			}

		}
		void Translate(glm::vec3 offset) {
			min += offset;
			max += offset;
		}
		bool intersects(AABB& other);
		float intersects(Ray& ray);
	};
	
}
namespace Cle::Networking::Components
{
	struct Texture
	{
		unsigned int Path;
		unsigned char* data;
	};
	struct Material 
	{
		Texture colorMap;
		float color[4];
	};
}
