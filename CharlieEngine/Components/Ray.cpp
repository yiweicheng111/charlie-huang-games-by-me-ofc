#include "Components.h"
void Cle::Components::Ray::SetFromPointer(double mx, double my, int screenWidth, int screenHeight, Cle::Gfx::Camera& camera)
{


    float x = (2 * mx) / screenWidth - 1;
    float y = 1 - (2 * my) / screenHeight;
    glm::mat4 inverse = glm::inverse(camera.getProjection() * camera.getViewMatrix());
    glm::vec4 clip = glm::vec4(x, y, -1.0f, 1.0f);
    glm::vec4 world = inverse * clip;
    if (world.w != 0) {
        world.x /= world.w;
        world.y /= world.w;
        world.z /= world.w;
    }
    origin = camera.Position;
    dir = glm::normalize(glm::vec3(world.x, world.y, world.z) - origin);
}
