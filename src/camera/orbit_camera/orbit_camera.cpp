#include "orbit_camera.h"
#include <cmath>

OrbitCamera::OrbitCamera(const glm::vec3& t)
    : target(t) {
    position = glm::vec3(0, 0, -8);
}

glm::mat4 OrbitCamera::getViewMatrix() const {
    return glm::lookAt(position, target, up);
}

glm::mat4 OrbitCamera::getProjectionMatrix(int width, int height) {
    aspect = float(width) / float(height);
    return glm::perspective(glm::radians(fov), aspect, nearPlane, farPlane);
}

glm::vec3 OrbitCamera::getPosition() const {
    return position;
}

void OrbitCamera::setPosition(const glm::vec3 &pos) {
    position = pos;
}

void OrbitCamera::orbit(float deltaYaw, float deltaPitch) {
    glm::vec3 offset = position - target;
    float radius = glm::length(offset);
    float theta = std::atan2(offset.z, offset.x);
    float phi = std::acos(offset.y / radius);

    theta += deltaYaw;
    phi += deltaPitch;
    const float epsilon = 0.01f;
    phi = glm::clamp(phi, epsilon, glm::pi<float>() - epsilon);

    position.x = target.x + radius * std::sin(phi) * std::cos(theta);
    position.y = target.y + radius * std::cos(phi);
    position.z = target.z + radius * std::sin(phi) * std::sin(theta);
}

void OrbitCamera::zoom(float delta) {
    glm::vec3 direction = glm::normalize(target - position);
    position += direction * delta;
    float distance = glm::length(target - position);
    if (distance < 0.5f) {
        position = target - direction * 0.5f;
    }
}

void OrbitCamera::moveTarget(const glm::vec3& offset) {
    target += offset;
    position += offset;
}

void OrbitCamera::setViewTop() {
    float dist = glm::length(position - target);
    position = target + glm::vec3(0, dist, 0);
}

void OrbitCamera::setViewFront() {
    float dist = glm::length(position - target);
    position = target + glm::vec3(0, 0, dist);
}

void OrbitCamera::setViewSide() {
    float dist = glm::length(position - target);
    position = target + glm::vec3(dist, 0, 0);
}

void OrbitCamera::setTarget(const glm::vec3& t) {
    position += (t - target);
    target = t;
}

glm::vec3 OrbitCamera::getTarget() const {
    return target;
}

void OrbitCamera::setState(const glm::vec3& pos, const glm::vec3& tgt) {
	position = pos;
	target = tgt;
}

std::pair<glm::vec3, glm::vec3> OrbitCamera::getState() const {
	return {position, target};
}