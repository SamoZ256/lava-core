#include "frustum.hpp"

float Plan::getSignedDistanceToPlan(const glm::vec3& point) {
    return glm::dot(normal, point) - distance;
}

void Frustum::create(glm::vec3 position, glm::vec3 direction, float nearPlane, float farPlane, float fov, float aspectRatio) {
    const float halfVSide = farPlane * tanf(fov * .5f);
    const float halfHSide = halfVSide * aspectRatio;
    const glm::vec3 frontMultFar = farPlane * direction;

    glm::vec3 right = glm::normalize(glm::cross(direction, glm::vec3(0.0f, 1.0f, 0.0f)));
    glm::vec3 up    = glm::normalize(glm::cross(right, direction));

    nearFace = Plan(position + nearPlane * direction, direction);
    farFace = Plan(position + frontMultFar, -direction);
    rightFace = Plan(position, glm::cross(up, frontMultFar + right * halfHSide));
    leftFace = Plan(position, glm::cross(frontMultFar - right * halfHSide, up));
    topFace = Plan(position, glm::cross(right, frontMultFar - up * halfVSide));
    bottomFace = Plan(position, glm::cross(frontMultFar + up * halfVSide, right));
}

bool Frustum::intersectsWithAABB(const AABB& aabb) {
    return (isInOrInFrontOfPlanAABB(aabb, leftFace) &&
            isInOrInFrontOfPlanAABB(aabb, rightFace) &&
            isInOrInFrontOfPlanAABB(aabb, topFace) &&
            isInOrInFrontOfPlanAABB(aabb, bottomFace) &&
            isInOrInFrontOfPlanAABB(aabb, nearFace) &&
            isInOrInFrontOfPlanAABB(aabb, farFace));
}
