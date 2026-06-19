#include "Light.h"
namespace Cle::Components
{
	class LightComponent 
	{
	private:
		Cle::Gfx::Light m_Light;
	public:
		const Cle::Gfx::Light& getLight() const
		{
			return m_Light;
		}
		int ID = -1;
		bool dirty = false;
		glm::vec3 getPosition() const { return m_Light.position; }
		glm::vec3 getColor() const { return m_Light.color; }
		float getRadius() const  { return m_Light.radius; }

		void setPosition(glm::vec3 position) 
		{ 
			m_Light.position = position; 
			dirty = true;
		};
		void setColor(glm::vec3 color) 
		{
			m_Light.color = color;
			dirty = true;
		};
		void setRadius(float radius)
		{
			m_Light.radius = radius;
			dirty = true;
		}

	};
}
