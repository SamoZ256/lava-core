#ifndef FIRST_PERSON_CAMERA_H
#define FIRST_PERSON_CAMERA_H

#include "lvnd/lvnd.h"

#include "camera.hpp"

class FirstPersonCamera : public Camera {
private:
	//Positions
	glm::vec3 up = glm::vec3(0.0f, 1.0f, 0.0f);
	glm::vec3 movement = glm::vec3(0.0f);

	float prevMouseX, prevMouseY;

	float sensitivity = 100.0f;
    float speed = 0.04f;

public:
  	void inputs(LvndWindow* window, float dt);

	void applyMovement() {
		prevPosition = position;
		position += movement;
		movement = glm::vec3(0.0f);
	}

	//Getters
	inline const glm::vec3& getMovement() {
		return movement;
	}

	//Setters
	inline void setMovement(const glm::vec3& aMovement) {
		movement = aMovement;
	}
};

#endif
