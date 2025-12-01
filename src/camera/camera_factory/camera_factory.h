#ifndef CAMERA_FACTORY_H
#define CAMERA_FACTORY_H

#include "camera.h"
#include <memory>

class CameraFactory {
public:
	virtual ~CameraFactory() = default;
	[[nodiscard]] virtual std::unique_ptr<Camera> createCamera() const = 0;
};

class OrbitCameraFactory : public CameraFactory {
public:
	[[nodiscard]] std::unique_ptr<Camera> createCamera() const override;
};

class FreeCameraFactory : public CameraFactory {
public:
	[[nodiscard]] std::unique_ptr<Camera> createCamera() const override;
};

#endif // CAMERA_FACTORY_H