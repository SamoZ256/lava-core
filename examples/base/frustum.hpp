#ifndef LV_FRUSTUM_H
#define LV_FRUSTUM_H

#include "glm.hpp"

struct AABB {
    glm::vec3 center;
    glm::vec3 size;
};

struct Plan {
    glm::vec3 normal = {0.0f, 1.0f, 0.0f};
    float distance = 0.0f;

    Plan() = default;

    Plan(glm::vec3 point, glm::vec3 aNormal) : normal(glm::normalize(aNormal)), distance(glm::dot(normal, point)) {}

    float getSignedDistanceToPlan(const glm::vec3& point);
};

class Frustum {
public:
    Plan topFace;
    Plan bottomFace;
    Plan farFace;
    Plan nearFace;
    Plan rightFace;
    Plan leftFace;

    Frustum() = default;

    void create(glm::vec3 position, glm::vec3 direction, float nearPlane, float farPlane, float fov, float aspectRatio);

    //Intersections
    bool intersectsWithAABB(const AABB& aabb);

    bool isInOrInFrontOfPlanAABB(const AABB& aabb, Plan& face) {
        // Compute the projection interval radius of b onto L(t) = b.c + t * p.n
        const float r = aabb.size.x * 0.5f * std::abs(face.normal.x) + aabb.size.y * 0.5f * std::abs(face.normal.y) + aabb.size.z * 0.5f * std::abs(face.normal.z);
        //std::cout << -r << " : " << face.getSignedDistanceToPlan(position) << std::endl << std::endl;

        return -r <= face.getSignedDistanceToPlan(aabb.center);
    }
};

#endif
