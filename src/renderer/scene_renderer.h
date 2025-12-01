#ifndef SCENE_RENDERER_H
#define SCENE_RENDERER_H

#include "scene.h"
#include "rasterizer.h"
#include "shadow_map_renderer.h"
#include "camera.h"
#include <QImage>
#include <memory>

class SceneRenderer {
public:
	SceneRenderer(int width, int height);

	QImage render(const Scene& scene, Camera& camera);

	void setShadowsEnabled(bool enabled);
	void setShadowMapSize(int size);
	void setSunLight(const Light& light);
	void resize(int width, int height);

	bool areShadowsEnabled() const { return shadowsEnabled; }
	Light getSunLight() const { return sunLight; }
	int getWidth() const;
	int getHeight() const;

	static glm::vec3 computeSceneCenter(const Scene& scene);

private:
	std::unique_ptr<Rasterizer> rasterizer;
	std::unique_ptr<ShadowMapRenderer> shadowMapRenderer;

	bool shadowsEnabled;
	int shadowMapSize;
	Light sunLight;

	void renderWithShadows(const Scene& scene, Camera& camera, const std::vector<Light>& sceneLights) const;
	void renderWithoutShadows(const Scene& scene, Camera& camera, const std::vector<Light>& sceneLights) const;
	glm::mat4 computeLightMatrix(const Scene& scene) const;
};

#endif // SCENE_RENDERER_H