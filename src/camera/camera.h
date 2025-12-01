#ifndef CAMERA_H
#define CAMERA_H

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

class Camera {
public:
	virtual ~Camera() = default;

	[[nodiscard]] virtual glm::mat4 getViewMatrix() const = 0;
	virtual glm::mat4 getProjectionMatrix(int width, int height) = 0;
	[[nodiscard]] virtual glm::vec3 getPosition() const = 0;
};

#endif // CAMERA_H