#include "first_person_camera.hpp"

void FirstPersonCamera::inputs(LvndWindow* window, float dt) {
	prevDirection = direction;

    int32_t mouseX, mouseY;
    lvndGetCursorPosition(window, &mouseX, &mouseY);
    uint16_t width, height;
    lvndGetWindowSize(window, &width, &height);

	float crntSpeed = speed;
	if (lvndGetKeyState(window, LVND_KEY_LEFT_SHIFT) == LVND_STATE_PRESSED)
		crntSpeed *= 6.0f;

	if (lvndGetKeyState(window, LVND_KEY_W) == LVND_STATE_PRESSED)
		movement += crntSpeed * direction;
	if (lvndGetKeyState(window, LVND_KEY_A) == LVND_STATE_PRESSED)
		movement += crntSpeed * -glm::normalize(glm::cross(direction, up));
	if (lvndGetKeyState(window, LVND_KEY_S) == LVND_STATE_PRESSED)
		movement += crntSpeed * -direction;
	if (lvndGetKeyState(window, LVND_KEY_D) == LVND_STATE_PRESSED)
		movement += crntSpeed * glm::normalize(glm::cross(direction, up));

	if (lvndGetMouseButtonState(window, LVND_MOUSE_BUTTON_LEFT) == LVND_STATE_PRESSED) {
		float rotX = sensitivity * float(mouseY - prevMouseY) / float(height);
		float rotY = sensitivity * float(mouseX - prevMouseX) / float(width);

		direction = glm::rotate(direction, glm::radians(rotX), glm::normalize(glm::cross(direction, up)));

		/*
		if (abs(glm::angle(newDirection, up) - glm::radians(90.0f)) <= glm::radians(85.0f)) {
		direction = newDirection;
		}
		*/

		direction = glm::rotate(direction, glm::radians(-rotY), up);
	}

    prevMouseX = mouseX;
    prevMouseY =  mouseY;
}
