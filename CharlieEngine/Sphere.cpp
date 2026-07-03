#include "Components.h"
bool Cle::Components::Sphere::isOnFrustum(const Frustum& frustum, const glm::mat4& model)
{
	Sphere globalSphere(center, radius);
	return (globalSphere.isOnOrForwardPlane(frustum.leftFace) &&
		globalSphere.isOnOrForwardPlane(frustum.rightFace) &&
		globalSphere.isOnOrForwardPlane(frustum.farFace) &&
		globalSphere.isOnOrForwardPlane(frustum.nearFace) &&
		globalSphere.isOnOrForwardPlane(frustum.topFace) &&
		globalSphere.isOnOrForwardPlane(frustum.bottomFace));
}

Cle::Components::Sphere::Sphere(const std::vector<Cle::Gfx::Vertex>& vertices)
{
	for (auto& vertex : vertices) {
		center += vertex.Position;
	}
	center /= (float)vertices.size();
	for (auto& vertex : vertices) {
		float d = glm::distance(center, vertex.Position);
		if (d > radius) radius = d;
		localRadius = radius;
		localCenter = center;
	}
}
void Cle::Components::Sphere::updateToWorld(const std::vector<Cle::Gfx::Vertex>& vertices, const glm::mat4& model)
{
	center = glm::vec3(model * glm::vec4(localCenter, 1.0f));
	glm::vec3 globalScale = glm::vec3(glm::length(glm::vec3(model[0])),glm::length(glm::vec3(model[1])) ,glm::length(glm::vec3(model[2])));
	radius = localRadius * std::max({ globalScale.x,globalScale.y,globalScale.z });
}

