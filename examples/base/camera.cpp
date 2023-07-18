#include "camera.hpp"

void Camera::loadViewProj() {
    prevView = view;
    prevProj = projection;
    prevViewProj = viewProj;
    prevInvViewProj = invViewProj;
    view = glm::lookAt(position, position + direction, glm::vec3(0.0f, 1.0f, 0.0f));
    projection = glm::perspective(glm::radians(fov), aspectRatio, nearPlane, farPlane);
    /*
    if (jitter) {
        float jitterX = (rand() % 1000 - 500) / 500.0f / width;
        float jitterY = (rand() % 1000 - 500) / 500.0f / height;
        projection[3][0] += jitterX;
        projection[3][1] += jitterY;
    }
    */
    viewProj = projection * view;
    invViewProj = glm::inverse(viewProj);
    
    frustum.create(position, direction, nearPlane, farPlane, fov, aspectRatio);
}
