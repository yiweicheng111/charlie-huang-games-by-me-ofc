#include "Components.h"
Cle::Components::Frustum Cle::Components::Frustum::createFrustumInCamera(const Cle::Gfx::Camera& camera){
	Cle::Components::Frustum frustum;
	 const float vside = camera.far * glm::tan((camera.fov * 0.5f));
	 const float hside = vside * camera.aspect;
	 glm::vec3 forward = camera.getForward();
	 glm::vec3 right = camera.getRight();
	 glm::vec3 up = camera.getUp();

	 const glm::vec3 frontMultFar = camera.far * glm::normalize(forward);
	 frustum.nearFace = { camera.Position + camera.near * forward, forward };
	 frustum.farFace = { camera.Position + frontMultFar, glm::normalize( - 1.0f * forward)};
	 frustum.rightFace = { camera.Position,
							 glm::normalize(glm::cross(frontMultFar - right * hside, up)) };
	 frustum.leftFace = { camera.Position,
							glm::normalize(glm::cross(up ,frontMultFar + right * hside))};
	 frustum.topFace = { camera.Position,
							 glm::normalize(glm::cross(right, frontMultFar - up * vside)) };
	 frustum.bottomFace = { camera.Position,
							glm::normalize(glm::cross(frontMultFar + up * vside, right)) };

	 return frustum;
}