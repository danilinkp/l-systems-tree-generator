#include "scene_renderer.h"
#include "draw_visitor.h"
#include "shadow_visitor.h"
#include "mesh_object.h"
#include "instanced_mesh_object.h"
#include "plane_object.h"
#include <glm/gtx/quaternion.hpp>
#include <glm/gtc/matrix_transform.hpp>

SceneRenderer::SceneRenderer(int width, int height)
	: shadowsEnabled(true),
	  shadowMapSize(2048),
	  sunLight(Light::createDirectional(glm::vec3(1.0f, -2.0f, 1.0f))) {
	rasterizer = std::make_unique<Rasterizer>(width, height);
	shadowMapRenderer = std::make_unique<ShadowMapRenderer>(shadowMapSize, shadowMapSize);
}

QImage SceneRenderer::render(const Scene &scene, Camera &camera) {
	if (!rasterizer)
		return {};

	const auto &sceneLights = scene.getLights();

	bool foundDirectionalForShadows = false;
	for (const auto &light : sceneLights) {
		if (light.type == LightType::Directional) {
			sunLight = light;
			foundDirectionalForShadows = true;
			break;
		}
	}

	if (shadowsEnabled && foundDirectionalForShadows) {
		renderWithShadows(scene, camera, sceneLights);
	} else {
		renderWithoutShadows(scene, camera, sceneLights);
	}

	return rasterizer->endFrame();
}

void SceneRenderer::setShadowsEnabled(const bool enabled) {
	shadowsEnabled = enabled;
}

void SceneRenderer::setShadowMapSize(const int size) {
	shadowMapSize = size;
	rasterizer->clearShadowMap();
	shadowMapRenderer = std::make_unique<ShadowMapRenderer>(shadowMapSize, shadowMapSize);
}

void SceneRenderer::setSunLight(const Light &light) {
	sunLight = light;
}

void SceneRenderer::resize(int width, int height) {
	rasterizer = std::make_unique<Rasterizer>(width, height);
}

int SceneRenderer::getWidth() const {
	return rasterizer->getWidth();
}

int SceneRenderer::getHeight() const {
	return rasterizer->getHeight();
}

glm::vec3 SceneRenderer::computeSceneCenter(const Scene &scene) {
	if (scene.getObjects().empty())
		return {0.0f, 0.0f, 0.0f};

	glm::vec3 minBound(FLT_MAX);
	glm::vec3 maxBound(-FLT_MAX);
	bool hasGeometry = false;

	for (const auto &obj : scene.getObjects()) {
		if (auto *meshObj = dynamic_cast<MeshObject *>(obj.get())) {
			for (const auto &v : meshObj->getMesh().vertices) {
				glm::vec3 worldPos = v.position + meshObj->getPosition();
				minBound = glm::min(minBound, worldPos);
				maxBound = glm::max(maxBound, worldPos);
				hasGeometry = true;
			}
		} else if (auto *instObj = dynamic_cast<InstancedMeshObject *>(obj.get())) {
			const auto &proto = instObj->getPrototype();
			for (const auto &inst : instObj->getInstances()) {
				for (const auto &v : proto.vertices) {
					glm::vec3 worldPos = inst.position + v.position;
					minBound = glm::min(minBound, worldPos);
					maxBound = glm::max(maxBound, worldPos);
					hasGeometry = true;
				}
			}
		} else if (auto *planeObj = dynamic_cast<PlaneObject *>(obj.get())) {
			for (const auto &v : planeObj->getMesh().vertices) {
				glm::vec3 worldPos = v.position + planeObj->getPosition();
				minBound = glm::min(minBound, worldPos);
				maxBound = glm::max(maxBound, worldPos);
				hasGeometry = true;
			}
		}
	}

	if (!hasGeometry)
		return {0.0f, 0.0f, 0.0f};

	return (minBound + maxBound) * 0.5f;
}

void SceneRenderer::renderWithShadows(const Scene &scene, Camera &camera, const std::vector<Light> &sceneLights) const {
	glm::vec3 lightDir = glm::normalize(sunLight.direction);
	glm::vec3 lightDirectionFrom = -lightDir;
	glm::vec3 sceneCenter = computeSceneCenter(scene);

	float shadowDistance = 50.0f;
	glm::vec3 lightPos = sceneCenter + lightDirectionFrom * shadowDistance;

	glm::vec3 up = glm::vec3(0, 1, 0);
	if (std::abs(glm::dot(lightDirectionFrom, up)) > 0.95f)
		up = glm::vec3(1, 0, 0);

	glm::mat4 lightView = glm::lookAt(lightPos, sceneCenter, up);

	float shadowOrthoSize = 40.0f;
	glm::mat4 lightProj = glm::ortho(
		-shadowOrthoSize,
		shadowOrthoSize,
		-shadowOrthoSize,
		shadowOrthoSize,
		0.1f,
		150.0f
	);
	glm::mat4 lightMVP = lightProj * lightView;

	shadowMapRenderer->beginFrame();
	ShadowVisitor shadowVis(*shadowMapRenderer, lightMVP);
	scene.render(shadowVis);

	rasterizer->beginFrame();
	rasterizer->setShadowMap(
		shadowMapRenderer->getShadowMapData(),
		shadowMapRenderer->getWidth(),
		shadowMapRenderer->getHeight(),
		lightMVP
	);
	rasterizer->enableShadows(true);

	rasterizer->clearLights();
	for (const auto &light : sceneLights) {
		rasterizer->addLight(light);
	}

	DrawVisitor mainVisitor(*rasterizer, camera);
	scene.render(mainVisitor);
}

void SceneRenderer::renderWithoutShadows(const Scene &scene,
                                         Camera &camera,
                                         const std::vector<Light> &sceneLights) const {
	rasterizer->beginFrame();
	rasterizer->enableShadows(false);

	rasterizer->clearLights();
	for (const auto &light : sceneLights) {
		rasterizer->addLight(light);
	}

	DrawVisitor visitor(*rasterizer, camera);
	scene.render(visitor);
}

glm::mat4 SceneRenderer::computeLightMatrix(const Scene &scene) const {
	glm::vec3 lightDir = glm::normalize(sunLight.direction);
	glm::vec3 sceneCenter = computeSceneCenter(scene);

	float shadowDistance = 40.0f;
	glm::vec3 lightPos = sceneCenter - lightDir * shadowDistance;

	glm::mat4 lightView = glm::lookAt(lightPos, sceneCenter, glm::vec3(0, 1, 0));
	float shadowOrthoSize = 25.0f;
	glm::mat4 lightProj = glm::ortho(
		-shadowOrthoSize,
		shadowOrthoSize,
		-shadowOrthoSize,
		shadowOrthoSize,
		0.1f,
		100.0f
	);

	return lightProj * lightView;
}
