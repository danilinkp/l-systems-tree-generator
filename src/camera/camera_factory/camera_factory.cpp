#include "camera_factory.h"
#include "orbit_camera.h"
#include "free_camera.h"

std::unique_ptr<Camera> OrbitCameraFactory::createCamera() const {
	return std::make_unique<OrbitCamera>(glm::vec3(0.0f));
}

std::unique_ptr<Camera> FreeCameraFactory::createCamera() const {
	return std::make_unique<FreeCamera>(glm::vec3(0.0f, 5.0f, 10.0f));
}