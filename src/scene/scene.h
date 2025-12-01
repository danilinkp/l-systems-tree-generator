#ifndef SCENE_H
#define SCENE_H

#include "scene_object.h"
#include <vector>
#include <memory>

#include "light.h"
#include "mesh_object.h"

class Scene {
public:
	void addObject(std::unique_ptr<SceneObject> obj);
	void removeObject(size_t id);
	void clear();

	void render(BaseVisitor& visitor) const;

	[[nodiscard]] const std::vector<std::unique_ptr<SceneObject>>& getObjects() const;

	void addLight(const Light& light);
	[[nodiscard]] const std::vector<Light>& getLights() const;
	Light getSunLight() const;
	void clearLights();

private:
	std::vector<std::unique_ptr<SceneObject>> objects;

	std::vector<Light> lights;
};

#endif // SCENE_H