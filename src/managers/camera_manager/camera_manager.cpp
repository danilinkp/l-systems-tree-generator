#include "camera_manager.h"
#include "orbit_camera.h"
#include "free_camera.h"

CameraManager::CameraManager()
	: orbitFactory(std::make_unique<OrbitCameraFactory>()),
	  freeFactory(std::make_unique<FreeCameraFactory>()) {
	activeCamera = orbitFactory->createCamera();
}

void CameraManager::switchToOrbit() {
	if (auto* freeCam = dynamic_cast<FreeCamera*>(activeCamera.get())) {
		auto [pos, yaw, pitch] = freeCam->getState();
		savedFreeState.position = pos;
		savedFreeState.yaw = yaw;
		savedFreeState.pitch = pitch;
		savedFreeState.valid = true;
	}
	
	activeCamera = orbitFactory->createCamera();
	
	if (savedOrbitState.valid)
		if (auto* orbitCam = dynamic_cast<OrbitCamera*>(activeCamera.get()))
			orbitCam->setState(savedOrbitState.position, savedOrbitState.target);
}

void CameraManager::switchToFree() {
	if (auto* orbitCam = dynamic_cast<OrbitCamera*>(activeCamera.get())) {
		auto [pos, target] = orbitCam->getState();
		savedOrbitState.position = pos;
		savedOrbitState.target = target;
		savedOrbitState.valid = true;
	}
	
	activeCamera = freeFactory->createCamera();
	
	if (savedFreeState.valid)
		if (auto* freeCam = dynamic_cast<FreeCamera*>(activeCamera.get()))
			freeCam->setState(savedFreeState.position, savedFreeState.yaw, savedFreeState.pitch);
}

Camera& CameraManager::getActiveCamera() const {
	return *activeCamera;
}
