#ifndef FREE_CAMERA_H
#define FREE_CAMERA_H

#include "camera.h"

class FreeCamera : public Camera {
private:
	glm::vec3 position;
	glm::vec3 target;
	glm::vec3 up = glm::vec3(0, 1, 0);
	float yaw;
	float pitch;
	float fov = 60.0f;
	float aspect = 4.0f / 3.0f;
	float nearPlane = 0.1f;
	float farPlane = 100.0f;
	float speed = 2.0f;

	void updateVectors();

public:
	FreeCamera(const glm::vec3& pos = glm::vec3(0, 5, 10));

	glm::mat4 getViewMatrix() const override;
	glm::mat4 getProjectionMatrix(int width, int height) override;
	glm::vec3 getPosition() const override;

	void moveForward(float amount);
	void moveRight(float amount);
	void moveUp(float amount);
	void rotate(float dYaw, float dPitch);
	
	// Сохранение и восстановление состояния
	void setState(const glm::vec3& pos, float y, float p);
	std::tuple<glm::vec3, float, float> getState() const;
};

#endif // FREE_CAMERA_H