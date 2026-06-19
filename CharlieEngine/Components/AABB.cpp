#include "Components.h"
#include <GLFW/glfw3.h>
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
    tmin = std::max(tmin, std::min(y1,y2));
    tmax = std::min(tmax, std::max(y1, y2));

    float z1 = (min.z - ray.origin.z) / ray.dir.z;
    float z2 = (max.z - ray.origin.z) / ray.dir.z;
    tmin = std::max(tmin, std::min(z1,z2));
    tmax = std::min(tmax, std::max(z1, z2));

    return tmax >= std::max(0.0f, tmin) ? tmax : -1;
}
void Cle::Components::AABB::updateToWorld(const std::vector<Cle::Gfx::Vertex>& vertices, const glm::mat4& model)
{
    min = glm::vec3(std::numeric_limits<float>::max());
    max = glm::vec3(std::numeric_limits<float>::lowest());
    for (auto& verts : vertices) {
        glm::vec3 world = glm::vec3(model * glm::vec4(verts.Position, 1.0f));
        min = glm::min(min, world);
        max = glm::max(max, world);
    }
}

Cle::Components::AABB::AABB(const std::vector<Cle::Gfx::Vertex>& vertices)
{
    min = glm::vec3(std::numeric_limits<float>::max(), std::numeric_limits<float>::max(), std::numeric_limits<float>::max());
    max = glm::vec3(std::numeric_limits<float>::lowest(), std::numeric_limits<float>::lowest(), std::numeric_limits<float>::lowest());
    for (const Cle::Gfx::Vertex& v : vertices) {
        min.x = std::min(min.x, v.Position.x);
        min.y = std::min(min.y, v.Position.y);
        min.z = std::min(min.z, v.Position.z);

        max.x = std::max(max.x, v.Position.x);
        max.y = std::max(max.y, v.Position.y);
        max.z = std::max(max.z, v.Position.z);
    }

}