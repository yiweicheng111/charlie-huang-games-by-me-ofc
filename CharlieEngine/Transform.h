#pragma once
#include <glm/glm.hpp>
#include <glm/gtc/quaternion.hpp>
#include <functional>
#include "CharlieEngine/Camera.h"
namespace Cle::Components
{
	class Transform
	{
	 
	private:
		glm::quat orientation = glm::quat(glm::vec3(0, 0, 0));
		glm::vec3 position = glm::vec3(0.0f);
		glm::vec3 scale = glm::vec3(1.0f);
		mutable glm::mat4 model = glm::mat4(1.0f);
		mutable bool dirty = true;

	public:
		glm::mat4 computeMatrix();

		glm::vec3 relativePosition;
		glm::mat4 getRelativeModel(Cle::Gfx::Camera& cam);
		glm::quat getOrientation() const { return orientation; }
		glm::vec3 getPosition() const { return position; }
		glm::vec3 getScale() const { return scale; }
		bool physicsDirty = true;
		bool boundsDirty = true;
		void setScale(glm::vec3 _scale) {
			scale = _scale;  dirty = true; 
			physicsDirty = true;
			boundsDirty = true;
		}
		void setPosition(glm::vec3 _position) {
			position = _position; 
			dirty = true;
			boundsDirty = true;

		}
		void setOrientation(glm::quat _rot)
		{ orientation = _rot; 
		dirty = true;
		boundsDirty = true;

		}

		template <class Archive>
		void save(Archive& ar) const
		{
			auto pos = getPosition();
			auto oren = glm::eulerAngles(getOrientation());
			auto scale = getScale();
			ar(pos.x, pos.y, pos.z);
			ar(oren.x, oren.y, oren.z);
			ar(scale.x, scale.y, scale.z);
		}
		template <class Archive>
		void load(Archive& ar) 
		{
			glm::vec3 pos, oren, scale;
			ar(pos.x, pos.y, pos.z);
			ar(oren.x, oren.y, oren.z);
			ar(scale.x, scale.y, scale.z);
			setPosition(pos);
			setOrientation(glm::quat(oren));
			setScale(scale);
			dirty = true;

		}
	};
}