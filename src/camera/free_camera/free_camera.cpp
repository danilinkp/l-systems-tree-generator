#include "free_camera.h"
#include <cmath>

FreeCamera::FreeCamera(const glm::vec3& pos)
    : position(pos), yaw(-glm::pi<float>() / 2.0f), pitch(0.0f) {
    updateVectors();
}

void FreeCamera::updateVectors() {
    glm::vec3 front;
    front.x = cos(yaw) * cos(pitch);
    front.y = sin(pitch);
    front.z = sin(yaw) * cos(pitch);
    front = glm::normalize(front);
    target = position + front;
}

glm::mat4 FreeCamera::getViewMatrix() const {
    return glm::lookAt(position, target, up);
}

glm::mat4 FreeCamera::getProjectionMatrix(int width, int height) {
    aspect = float(width) / float(height);
    return glm::perspective(glm::radians(fov), aspect, nearPlane, farPlane);
}

glm::vec3 FreeCamera::getPosition() const {
    return position;
}

void FreeCamera::moveForward(float amount) {
    glm::vec3 front = glm::normalize(target - position);
    position += front * amount * speed;
    target += front * amount * speed;
}

void FreeCamera::moveRight(float amount) {
    glm::vec3 front = glm::normalize(target - position);
    glm::vec3 right = glm::normalize(glm::cross(front, up));
    position += right * amount * speed;
    target += right * amount * speed;
}

void FreeCamera::moveUp(float amount) {
    position += up * amount * speed;
    target += up * amount * speed;
}

void FreeCamera::rotate(float dYaw, float dPitch) {
    yaw += dYaw;
    pitch += dPitch;
    pitch = glm::clamp(pitch, -glm::pi<float>() * 0.49f, glm::pi<float>() * 0.49f);
    updateVectors();
}

void FreeCamera::setState(const glm::vec3& pos, float y, float p) {
	position = pos;
	yaw = y;
	pitch = p;
	updateVectors();
}

std::tuple<glm::vec3, float, float> FreeCamera::getState() const {
	return {position, yaw, pitch};
}