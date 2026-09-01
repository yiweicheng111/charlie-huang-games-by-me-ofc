#pragma once
#include <string>
#include <glm/glm.hpp>
#include <functional>
#include "Camera.h"
#include <entt/entt.hpp>
#include "Transform.h"
#include "LightComponent.h"
#include "CharlieEngine/Vertex.h"
namespace Cle::Components
{
	
	struct Plane
	{
		glm::vec3 normal = glm::vec3(0, 1, 0);
		float distance = 0;
		float getSignedDistanceToPlane(const glm::vec3& point) const {
			return glm::dot(normal, point) - distance;
		}
		Plane() {}
		Plane(glm::vec3 point, glm::vec3 _normal) : normal(_normal), distance(glm::dot(normal, point)) {}
		
	};
	struct Frustum
	{
		Plane topFace;
		Plane leftFace;
		Plane rightFace;
		Plane bottomFace;
		Plane farFace;
		Plane nearFace;
		Frustum() = default;
		static Frustum createFrustumInCamera(const Cle::Gfx::Camera& camera);
	};
	struct Volume
	{
		virtual bool isOnFrustum(const Frustum& frustum, const glm::mat4& model) = 0;
	};
	struct Sphere : Cle::Components::Volume
	{
		bool dirty = true;

		glm::vec3 center = glm::vec3(0.0f);
		glm::vec3 localCenter = glm::vec3(0.0f);

		float radius = 0.0f;
		float localRadius = 0.0f;

		Sphere(glm::vec3 _center, float _radius) : center(_center), radius(_radius) {}
		Sphere(){}
		bool isOnOrForwardPlane(const Plane& plane) {

			return plane.getSignedDistanceToPlane(center) >(- radius);

		}
		virtual bool isOnFrustum(const Frustum& frustum, const glm::mat4& model) override;               
		Sphere(const std::vector<Cle::Gfx::Vertex>& vertices);
		void updateToWorld(const std::vector<Cle::Gfx::Vertex>& vertices, const glm::mat4& model);
	};
	struct Ray 
	{

		Ray() = default;
		glm::vec3 origin{};
		glm::vec3 dir{};
		void SetFromPointer(double mx, double my,int screenWidth, int screenHeight, Cle::Gfx::Camera& camera);
		Ray(glm::vec3 origin, glm::vec3 dir) : dir(dir), origin(origin) {}
	};
	struct AABB : Cle::Components::Volume
	{
		glm::vec3 max{};
		glm::vec3 min{};
		bool dirty = true;

		virtual bool isOnFrustum(const Frustum& frustum, const glm::mat4& model) override
		{
			return false;
		}

		AABB() = default;
		
		void updateToWorld(const std::vector<Cle::Gfx::Vertex>& vertices, const glm::mat4& model);
		AABB(const std::vector<Cle::Gfx::Vertex>& vertices);
		void Translate(glm::vec3 offset)
		{
			min += offset;
			max += offset;
		}
		bool intersects(AABB& other);
		float intersects(Ray& ray);
	};
	struct CubeMapTexture
	{
		std::vector<std::string> faces = std::vector<std::string>(6,"sky.jpg");
	};
	struct Bounds
	{
		AABB aabb;
		Sphere sphere;
		Bounds(AABB a, Sphere s) : aabb(a), sphere(s) {}
	};
}
