#include "Transform.h"
#include <entt/entt.hpp>
#include <entt/meta/factory.hpp>
#include <entt/meta/resolve.hpp>
#include <entt/meta/template.hpp>
glm::mat4 Cle::Components::Transform::computeMatrix()
{
	if (dirty)
	{
		model = glm::mat4(1.0f);
		model = glm::translate(model, position);
		model *= glm::mat4_cast(orientation);
		model = glm::scale(model, scale);
		dirty = false;
	}
	return model;

}
