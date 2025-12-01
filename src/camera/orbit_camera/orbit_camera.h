#ifndef ORBIT_CAMERA_H
#define ORBIT_CAMERA_H

#include "camera.h"

class OrbitCamera : public Camera {
public:
	explicit OrbitCamera(const glm::vec3& t = glm::vec3(0));

	glm::mat4 getViewMatrix() const override;
	glm::mat4 getProjectionMatrix(int width, int height) override;
	glm::vec3 getPosition() const override;

	void setPosition(const glm::vec3& pos);

	void orbit(float deltaYaw, float deltaPitch);
	void zoom(float delta);
	void moveTarget(const glm::vec3& offset);
	void setViewTop();
	void setViewFront();
	void setViewSide();

	void setTarget(const glm::vec3& t);
	glm::vec3 getTarget() const;
	
	// Сохранение и восстановление состояния
	void setState(const glm::vec3& pos, const glm::vec3& tgt);
	std::pair<glm::vec3, glm::vec3> getState() const;

private:
	glm::vec3 position;
	glm::vec3 target;
	glm::vec3 up = glm::vec3(0, 1, 0);
	float fov = 60.0f;
	float aspect = 4.0f / 3.0f;
	float nearPlane = 0.5f;
	float farPlane = 100.0f;

	void updatePosition();
};

#endif // ORBIT_CAMERA_H