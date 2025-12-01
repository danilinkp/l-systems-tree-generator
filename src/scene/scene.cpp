#include "scene.h"
#include <algorithm>

void Scene::addObject(std::unique_ptr<SceneObject> obj) {
	objects.push_back(std::move(obj));
}

void Scene::removeObject(size_t id) {
	objects.erase(
		std::ranges::remove_if(objects,
		                       [id](const std::unique_ptr<SceneObject> &obj) {
			                       return obj->getId() == id;
		                       }).begin(),
		objects.end()
	);
}

void Scene::clear() {
	objects.clear();
}

void Scene::render(BaseVisitor &visitor) const {
	for (const auto &obj : objects)
		obj->accept(visitor);
}

const std::vector<std::unique_ptr<SceneObject> > &Scene::getObjects() const {
	return objects;
}

void Scene::addLight(const Light &light) {
	lights.push_back(light);
}

const std::vector<Light> &Scene::getLights() const {
	return lights;
}

Light Scene::getSunLight() const {
	for (const auto &light : lights)
		if (light.type == LightType::Directional)
			return light;

	return Light::createDirectional(glm::vec3(-1.0f, -1.5f, -1.0f));
}

void Scene::clearLights() {
	lights.clear();
}