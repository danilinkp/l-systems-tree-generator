#ifndef RASTERIZER_H
#define RASTERIZER_H

#include <QImage>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include "mesh.h"
#include "z_buffer.h"
#include "light.h"
#include "lighting.h"
#include <atomic>
#include <vector>
#include <thread>
#include <mutex>
#include <limits>

struct ScreenVertex {
	glm::vec2 position;
	float depth;
	glm::vec3 color;
	glm::vec3 normal;
	glm::vec3 worldPos;
	glm::vec2 texCoord;

	ScreenVertex() : position(0), depth(0), color(0), normal(0, 1, 0), worldPos(0), texCoord(0) {}

	ScreenVertex(glm::vec2 position,
	             float depth,
	             const glm::vec3 color,
	             const glm::vec3 normal,
	             const glm::vec3 worldPos,
	             const glm::vec2 texCoord = glm::vec2(0))
		: position(position), depth(depth), color(color), normal(normal),
		  worldPos(worldPos), texCoord(texCoord) {}
};

class Rasterizer {
public:
	Rasterizer(int width, int height);

	void beginFrame();

	void renderMesh(const Mesh &mesh, const glm::mat4 &mvp, const glm::vec3 &cameraPos, const QImage* texture = nullptr);

	QImage endFrame();

	void setLightingEnabled(bool enabled);
	void addLight(const Light &light);
	void clearLights();
	void setMaterial(const Lighting::Material &mat);

	int getWidth() const;
	int getHeight() const;

	[[nodiscard]] QImage getCurrentFrame() const {
		return image;
	}

	void setShadowMap(const float* data, int w, int h, const glm::mat4& mvp);
	void enableShadows(bool enable) { useShadows = enable; }

	void clearShadowMap();

private:
	QImage image;
	std::shared_ptr<ZBuffer> zBuffer;

	std::vector<Light> lights;
	bool enableLighting;
	Lighting::Material material;

	const QImage* currentTexture;

	std::atomic<int> trianglesDrawn;
	std::atomic<int> trianglesCulled;
	std::atomic<int> pixelsDrawn;

	const float* shadowMapData = nullptr;
	int shadowMapWidth = 0;
	int shadowMapHeight = 0;
	glm::mat4 lightMVP;
	bool useShadows = false;

	void clear();
	[[nodiscard]] glm::vec3 sampleTexture(float u, float v) const;

	[[nodiscard]] ScreenVertex transformVertex(const Vertex &v, const glm::mat4 &mvp) const;
	void drawTriangle(const ScreenVertex &v0,
	                  const ScreenVertex &v1,
	                  const ScreenVertex &v2,
	                  const glm::vec3 &cameraPos);
	void fillTriangle(ScreenVertex v0, ScreenVertex v1, ScreenVertex v2, const glm::vec3 &cameraPos);
	void drawScanline(int y, const ScreenVertex &left, const ScreenVertex &right, const glm::vec3 &cameraPos);
	[[nodiscard]] ScreenVertex interpolate(const ScreenVertex &v0, const ScreenVertex &v1, float t) const;
	[[nodiscard]] QColor calculateColor(const ScreenVertex &pixel, const glm::vec3 &cameraPos) const;
	[[nodiscard]] bool isOnScreen(const ScreenVertex &sv) const;

};

#endif // RASTERIZER_H