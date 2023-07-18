#ifndef CAMERA_H
#define CAMERA_H

#include "frustum.hpp"

class Camera {
public:
    glm::vec3 position = glm::vec3(0.0f);
    glm::vec3 direction = glm::vec3(0.0f, 0.0f, 1.0f);

    glm::vec3 prevPosition = position;
    glm::vec3 prevDirection = direction;

    float width, height = 0;

    Frustum frustum;

    //Vectors
    glm::vec3 up = glm::vec3(0.0f, 1.0f, 0.0f);

    //Resources
    glm::mat4 view;
    glm::mat4 projection;
    glm::mat4 viewProj;
    glm::mat4 invViewProj;

    glm::mat4 prevView = glm::mat4(1.0f);
    glm::mat4 prevProj = glm::mat4(1.0f);
    glm::mat4 prevViewProj = glm::mat4(1.0f);
    glm::mat4 prevInvViewProj = glm::mat4(1.0f);

    //Projection properties
    float nearPlane = 0.01f;
    float farPlane = 1000.0f;
    float fov = 45.0f;
    float aspectRatio = 1.0f;

    void loadViewProj();
};

#endif
