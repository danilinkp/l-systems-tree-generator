#include "shadow_map_renderer.h"
#include <QDebug>
#include <algorithm>
#include <cmath>

ShadowMapRenderer::ShadowMapRenderer(int w, int h)
	: width(w), height(h) {
	zBuffer = std::make_shared<ZBuffer>(width, height);
	shadowMap.resize(w * h);
}

void ShadowMapRenderer::beginFrame() {
	zBuffer->clear();
	std::ranges::fill(shadowMap, 1.0f);
}

void ShadowMapRenderer::renderMesh(const Mesh &mesh, const glm::mat4 &mvp) {
	for (const auto &tri : mesh.triangles) {
			const Vertex &v0 = mesh.vertices[tri.i0];
			const Vertex &v1 = mesh.vertices[tri.i1];
			const Vertex &v2 = mesh.vertices[tri.i2];

			glm::vec4 clip0 = mvp * glm::vec4(v0.position, 1.0f);
			glm::vec4 clip1 = mvp * glm::vec4(v1.position, 1.0f);
			glm::vec4 clip2 = mvp * glm::vec4(v2.position, 1.0f);

			if (clip0.w <= 0 || clip1.w <= 0 || clip2.w <= 0) continue;

			glm::vec3 ndc0 = glm::vec3(clip0) / clip0.w;
			glm::vec3 ndc1 = glm::vec3(clip1) / clip1.w;
			glm::vec3 ndc2 = glm::vec3(clip2) / clip2.w;

			float z0 = ndc0.z * 0.5f + 0.5f;
			float z1 = ndc1.z * 0.5f + 0.5f;
			float z2 = ndc2.z * 0.5f + 0.5f;

			glm::vec2 p0(
				(ndc0.x * 0.5f + 0.5f) * width,
				(1.0f - (ndc0.y * 0.5f + 0.5f)) * height
			);
			glm::vec2 p1(
				(ndc1.x * 0.5f + 0.5f) * width,
				(1.0f - (ndc1.y * 0.5f + 0.5f)) * height
			);
			glm::vec2 p2(
				(ndc2.x * 0.5f + 0.5f) * width,
				(1.0f - (ndc2.y * 0.5f + 0.5f)) * height
			);

			fillTriangle(p0, p1, p2, z0, z1, z2);
		}
}

void ShadowMapRenderer::fillTriangle(glm::vec2 v0, glm::vec2 v1, glm::vec2 v2, float z0, float z1, float z2) {
	int minX = std::max(0, (int)std::floor(std::min({v0.x, v1.x, v2.x})));
	int maxX = std::min(width - 1, (int)std::ceil(std::max({v0.x, v1.x, v2.x})));
	int minY = std::max(0, (int)std::floor(std::min({v0.y, v1.y, v2.y})));
	int maxY = std::min(height - 1, (int)std::ceil(std::max({v0.y, v1.y, v2.y})));

	for (int y = minY; y <= maxY; ++y) {
		for (int x = minX; x <= maxX; ++x) {
			glm::vec2 p(x + 0.5f, y + 0.5f);

			float det = (v1.y - v2.y) * (v0.x - v2.x) + (v2.x - v1.x) * (v0.y - v2.y);
			if (std::abs(det) < 1e-8f) continue;

			float w0 = ((v1.y - v2.y) * (p.x - v2.x) + (v2.x - v1.x) * (p.y - v2.y)) / det;
			float w1 = ((v2.y - v0.y) * (p.x - v2.x) + (v0.x - v2.x) * (p.y - v2.y)) / det;
			float w2 = 1.0f - w0 - w1;

			if (w0 >= 0 && w1 >= 0 && w2 >= 0) {
				float z = w0 * z0 + w1 * z1 + w2 * z2;
				z = glm::clamp(z, 0.0f, 1.0f);

				int idx = y * width + x;
				if (idx >= 0 && idx < static_cast<int>(shadowMap.size())) {
					if (zBuffer->testAndSet(x, y, z)) {
						shadowMap[idx] = z;
					}
				}

			}
		}
	}
}

const float* ShadowMapRenderer::getShadowMapData() const {
	return shadowMap.data();
}

int ShadowMapRenderer::getWidth() const {
	return width;
}

int ShadowMapRenderer::getHeight() const {
	return height;
}