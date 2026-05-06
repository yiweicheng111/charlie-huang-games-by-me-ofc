#include "Components.h"

bool Cle::Components::AABB::intersects(AABB& other)
{
    return (max.x > other.min.x && min.x < other.max.x) && (max.y > other.min.y && min.y < other.max.y) && (max.z > other.min.z && min.z < other.max.z);
}

float Cle::Components::AABB::intersects(Ray& ray)
{
    //max.x = origin+t*direction.x;
    float x1 = (min.x - ray.origin.x) / ray.dir.x;
    float x2 = (max.x - ray.origin.x) / ray.dir.x;
    float tmin = std::min(x1, x2);
    float tmax = std::max(x1, x2);

    float y1 = (min.y - ray.origin.y) / ray.dir.y;
    float y2 = (max.y - ray.origin.y) / ray.dir.y;
    tmin = std::min(y1, y2);
    tmax = std::max(y1, y2);

    float z1 = (min.z - ray.origin.z) / ray.dir.z;
    float z2 = (max.z - ray.origin.z) / ray.dir.z;
    tmin = std::min(z1, z2);
    tmax = std::max(z1, z2);
    return tmax >= std::min(0.0f, tmin) ? tmax : -1;
}

void Cle::Components::Ray::SetFromPointer(double mx, double my, int screenWidth, int screenHeight, Cle::Gfx::Camera& camera)
{
    float x = (2 * mx) / screenWidth - 1;
    float y = 1 - (2 * my) / screenHeight;
    glm::mat4 inverse = glm::inverse(camera.getProjection() * camera.getViewMatrix());
    glm::vec4 clip = glm::vec4(mx, my, -1.0f, 1.0f);
    glm::vec4 world = inverse * clip;
    if (world.w != 0) {
        world.x /= world.w;
        world.y /= world.w;
        world.z /= world.w;
    }
    origin = camera.Position;
    dir = glm::normalize(glm::vec3(world.x, world.y, world.z) - origin);
}
