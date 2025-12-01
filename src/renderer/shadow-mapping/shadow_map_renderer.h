#ifndef SHADOW_MAP_RENDERER_H
#define SHADOW_MAP_RENDERER_H

#include "z_buffer.h"
#include "mesh.h"
#include <glm/gtc/matrix_transform.hpp>
#include <vector>

class ShadowMapRenderer {
public:
	ShadowMapRenderer(int w, int h);

	void beginFrame();
	void renderMesh(const Mesh &mesh, const glm::mat4 &mvp);

	[[nodiscard]] const float *getShadowMapData() const;
	[[nodiscard]] int getWidth() const;
	[[nodiscard]] int getHeight() const;

private:
	std::shared_ptr<ZBuffer> zBuffer;
	int width, height;

	void fillTriangle(glm::vec2 v0, glm::vec2 v1, glm::vec2 v2, float z0, float z1, float z2);
	[[nodiscard]] glm::vec2 interpolate(const glm::vec2 &v0, const glm::vec2 &v1, float t) const;
	void drawScanline(int y, const glm::vec2 &left, const glm::vec2 &right, float z0, float z1, float z2);

	std::vector<float> shadowMap;
};

#endif // SHADOW_MAP_RENDERER_H
