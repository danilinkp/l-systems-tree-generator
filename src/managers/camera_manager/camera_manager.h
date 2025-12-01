#ifndef CAMERA_MANAGER_H
#define CAMERA_MANAGER_H

#include "camera_factory.h"
#include <memory>

class CameraManager {
public:
	CameraManager();

	void switchToOrbit();
	void switchToFree();

	Camera& getActiveCamera() const;
private:
	std::unique_ptr<Camera> activeCamera;
	std::unique_ptr<CameraFactory> orbitFactory;
	std::unique_ptr<CameraFactory> freeFactory;

	struct SavedOrbitState {
		glm::vec3 position;
		glm::vec3 target;
		bool valid = false;
	};

	struct SavedFreeState {
		glm::vec3 position;
		float yaw;
		float pitch;
		bool valid = false;
	};

	SavedOrbitState savedOrbitState;
	SavedFreeState savedFreeState;
};

#endif // CAMERA_MANAGER_H