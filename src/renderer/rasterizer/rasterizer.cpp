#include "rasterizer.h"
#include <glm/common.hpp>
#include <cmath>
#include <algorithm>
#include "draw_visitor.h"

Rasterizer::Rasterizer(int width, int height)
	: image(width, height, QImage::Format_RGB32),
	  zBuffer(std::make_shared<ZBuffer>(width, height)),
	  enableLighting(true),
	  currentTexture(nullptr),
	  trianglesDrawn(0),
	  trianglesCulled(0),
	  pixelsDrawn(0) {

	material.ambient = glm::vec3(0.2f);
	material.diffuse = glm::vec3(1.5f);
	material.specular = glm::vec3(0.2f);
	material.shininess = 16.0f;
}

void Rasterizer::beginFrame() {
	clear();

	shadowMapData = nullptr;
	shadowMapWidth = 0;
	shadowMapHeight = 0;
	useShadows = false;
}

void Rasterizer::renderMesh(const Mesh &mesh, const glm::mat4 &mvp, const glm::vec3 &cameraPos, const QImage* texture) {
	currentTexture = texture;

	for (const Triangle &tri : mesh.triangles) {
		const Vertex &v0 = mesh.vertices[tri.i0];
		const Vertex &v1 = mesh.vertices[tri.i1];
		const Vertex &v2 = mesh.vertices[tri.i2];

		ScreenVertex sv0 = transformVertex(v0, mvp);
		ScreenVertex sv1 = transformVertex(v1, mvp);
		ScreenVertex sv2 = transformVertex(v2, mvp);

		drawTriangle(sv0, sv1, sv2, cameraPos);
	}

	currentTexture = nullptr;
}

QImage Rasterizer::endFrame() {
	qDebug() << "Frame stats: Triangles drawn:" << trianglesDrawn
			<< "Culled:" << trianglesCulled
			<< "Pixels drawn:" << pixelsDrawn;

	return image;
}

void Rasterizer::clear() {
	image.fill(QColor(220, 230, 240));
	zBuffer->clear();
	trianglesDrawn = 0;
	trianglesCulled = 0;
	pixelsDrawn = 0;
}

glm::vec3 Rasterizer::sampleTexture(float u, float v) const {
	if (!currentTexture || currentTexture->isNull())
		return glm::vec3(1.0f);

	u = u - std::floor(u);
	v = v - std::floor(v);

	int x = static_cast<int>(u * (currentTexture->width() - 1));
	int y = static_cast<int>(v * (currentTexture->height() - 1));

	x = std::clamp(x, 0, currentTexture->width() - 1);
	y = std::clamp(y, 0, currentTexture->height() - 1);

	QRgb pixel = currentTexture->pixel(x, y);

	return glm::vec3(
		qRed(pixel) / 255.0f,
		qGreen(pixel) / 255.0f,
		qBlue(pixel) / 255.0f
	);
}

ScreenVertex Rasterizer::transformVertex(const Vertex &v, const glm::mat4 &mvp) const {
	glm::vec4 worldPos4 = mvp * glm::vec4(v.position, 1.0f);

	if (worldPos4.w <= 0.0f)
		return {};

	glm::vec3 clipPos = glm::vec3(worldPos4) / worldPos4.w;

	if (clipPos.z < -0.95f || clipPos.z > 1.0f)
		return {};

	if (worldPos4.w < 0.1f)
		return {};

	int w = image.width();
	int h = image.height();
	glm::vec2 screenPos(
		(clipPos.x * 0.5f + 0.5f) * w,
		(1.0f - (clipPos.y * 0.5f + 0.5f)) * h
	);

	return ScreenVertex{
		screenPos,
		clipPos.z,
		v.color,
		v.normal,
		v.position,
		v.texCoord
	};
}

void Rasterizer::drawTriangle(const ScreenVertex &v0,
                              const ScreenVertex &v1,
                              const ScreenVertex &v2,
                              const glm::vec3 &cameraPos) {
	float area2D = std::abs(
		(v1.position.x - v0.position.x) * (v2.position.y - v0.position.y) -
		(v2.position.x - v0.position.x) * (v1.position.y - v0.position.y)
	);

	if (area2D < 0.5f) {
		trianglesCulled++;
		return;
	}

	if (!isOnScreen(v0) && !isOnScreen(v1) && !isOnScreen(v2)) {
		trianglesCulled++;
		return;
	}

	fillTriangle(v0, v1, v2, cameraPos);
	trianglesDrawn++;
}

void Rasterizer::fillTriangle(ScreenVertex v0, ScreenVertex v1, ScreenVertex v2, const glm::vec3& cameraPos) {
	if (v0.position.y > v1.position.y) std::swap(v0, v1);
	if (v0.position.y > v2.position.y) std::swap(v0, v2);
	if (v1.position.y > v2.position.y) std::swap(v1, v2);

	float dy = v2.position.y - v0.position.y;
	if (dy < 0.5f) return;

	float area = std::abs(
		(v1.position.x - v0.position.x) * (v2.position.y - v0.position.y) -
		(v2.position.x - v0.position.x) * (v1.position.y - v0.position.y)
	) * 0.5f;

	if (area < 0.1f) return;

	float invTotalHeight = 1.0f / dy;

	float upperHeight = v1.position.y - v0.position.y;
	if (upperHeight > 0.1f) {
		int yStart = std::max(0, (int)std::ceil(v0.position.y - 0.5f));
		int yEnd = std::min(image.height() - 1, (int)std::floor(v1.position.y - 0.5f));

		for (int y = yStart; y <= yEnd; y++) {
			float py = y + 0.5f;

			float alpha = (py - v0.position.y) * invTotalHeight;
			alpha = glm::clamp(alpha, 0.0f, 1.0f);

			float beta = (py - v0.position.y) / upperHeight;
			beta = glm::clamp(beta, 0.0f, 1.0f);

			ScreenVertex edgeLong = interpolate(v0, v2, alpha);
			ScreenVertex edgeShort = interpolate(v0, v1, beta);

			if (edgeLong.position.x > edgeShort.position.x)
				std::swap(edgeLong, edgeShort);

			drawScanline(y, edgeLong, edgeShort, cameraPos);
		}
	}

	float lowerHeight = v2.position.y - v1.position.y;
	if (lowerHeight > 0.1f) {
		int yStart = std::max(0, (int)std::ceil(v1.position.y - 0.5f));
		int yEnd = std::min(image.height() - 1, (int)std::floor(v2.position.y - 0.5f));

		for (int y = yStart; y <= yEnd; y++) {
			float py = y + 0.5f;

			float alpha = (py - v0.position.y) * invTotalHeight;
			alpha = glm::clamp(alpha, 0.0f, 1.0f);

			float beta = (py - v1.position.y) / lowerHeight;
			beta = glm::clamp(beta, 0.0f, 1.0f);

			ScreenVertex edgeLong = interpolate(v0, v2, alpha);
			ScreenVertex edgeShort = interpolate(v1, v2, beta);

			if (edgeLong.position.x > edgeShort.position.x) {
				std::swap(edgeLong, edgeShort);
			}

			drawScanline(y, edgeLong, edgeShort, cameraPos);
		}
	}
}

void Rasterizer::drawScanline(int y, const ScreenVertex &left, const ScreenVertex &right, const glm::vec3 &cameraPos) {
	if (left.position.x > right.position.x)
		return;

	float dx = right.position.x - left.position.x;

	if (dx < 0.1f) {
		int x = (int)(left.position.x + 0.5f);
		if (x >= 0 && x < image.width()) {
			if (zBuffer->testAndSet(x, y, left.depth)) {
				QColor color = calculateColor(left, cameraPos);
				image.setPixel(x, y, color.rgb());
				pixelsDrawn++;
			}
		}
		return;
	}

	int xStart = std::max(0, static_cast<int>(std::floor(left.position.x + 0.5f)));
	int xEnd = std::min(image.width() - 1, static_cast<int>(std::floor(right.position.x + 0.5f)));

	float invDx = 1.0f / dx;
	QRgb* scanline = reinterpret_cast<QRgb*>(image.scanLine(y));

	for (int x = xStart; x <= xEnd; x++) {
		float px = x + 0.5f;
		float t = (px - left.position.x) * invDx;
		t = glm::clamp(t, 0.0f, 1.0f);

		ScreenVertex pixel = interpolate(left, right, t);

		if (pixel.depth >= 0.0f && pixel.depth <= 1.0f) {
			if (pixel.depth < 0.01f)
				continue;

			if (zBuffer->testAndSet(x, y, pixel.depth)) {
				QColor color = calculateColor(pixel, cameraPos);
				scanline[x] = color.rgb();
				pixelsDrawn++;
			}
		}
	}
}

ScreenVertex Rasterizer::interpolate(const ScreenVertex &v0, const ScreenVertex &v1, float t) const {
	ScreenVertex result;
	float s = 1.0f - t;

	result.position = v0.position * s + v1.position * t;

	float z0 = v0.depth;
	float z1 = v1.depth;

	if (z0 < 0.0001f) z0 = 0.0001f;
	if (z1 < 0.0001f) z1 = 0.0001f;

	float invZ0 = 1.0f / z0;
	float invZ1 = 1.0f / z1;
	float invZ = invZ0 * s + invZ1 * t;

	result.depth = 1.0f / invZ;

	float w0 = invZ0 * s;
	float w1 = invZ1 * t;
	float wSum = w0 + w1;

	if (wSum > 0.0001f) {
		w0 /= wSum;
		w1 /= wSum;

		result.color = v0.color * w0 + v1.color * w1;
		result.normal = v0.normal * w0 + v1.normal * w1;
		result.worldPos = v0.worldPos * w0 + v1.worldPos * w1;
		result.texCoord = v0.texCoord * w0 + v1.texCoord * w1;

		float normalLen = glm::length(result.normal);
		if (normalLen > 0.0001f)
			result.normal /= normalLen;
		else
			result.normal = glm::vec3(0, 1, 0);
	} else {
		result.color = v0.color * s + v1.color * t;
		result.normal = glm::normalize(v0.normal * s + v1.normal * t);
		result.worldPos = v0.worldPos * s + v1.worldPos * t;
		result.texCoord = v0.texCoord * s + v1.texCoord * t;
	}

	return result;
}

QColor Rasterizer::calculateColor(const ScreenVertex &pixel, const glm::vec3 &cameraPos) const {
	glm::vec3 textureColor = sampleTexture(pixel.texCoord.x, pixel.texCoord.y);

	glm::vec3 baseColor = pixel.color * textureColor;

	glm::vec3 finalColor;

	if (enableLighting && !lights.empty()) {
		glm::vec3 normal = pixel.normal;
		float normalLength = glm::length(normal);
		if (normalLength < 0.001f)
			normal = glm::vec3(0, 1, 0);
		else
			normal /= normalLength;

		float shadowFactor = 1.0f;

		if (useShadows && shadowMapData != nullptr && shadowMapWidth > 0 && shadowMapHeight > 0) {
			glm::vec4 lightSpacePos = lightMVP * glm::vec4(pixel.worldPos, 1.0f);

			if (lightSpacePos.w > 0.001f) {
				glm::vec3 ndcCoords = glm::vec3(lightSpacePos) / lightSpacePos.w;

				glm::vec3 shadowCoords;
				shadowCoords.x = ndcCoords.x * 0.5f + 0.5f;
				float ndcYNormalized = ndcCoords.y * 0.5f + 0.5f;
				shadowCoords.y = 1.0f - ndcYNormalized;
				shadowCoords.z = ndcCoords.z * 0.5f + 0.5f;

				if (shadowCoords.x >= 0.0f && shadowCoords.x <= 1.0f &&
					shadowCoords.y >= 0.0f && shadowCoords.y <= 1.0f &&
					shadowCoords.z >= 0.0f && shadowCoords.z <= 1.0f) {

					float shadow = 0.0f;
					int samples = 0;

					int filterSize = 3;
					float texelSize = 1.0f / static_cast<float>(shadowMapWidth);

					for (int x = -filterSize/2; x <= filterSize/2; ++x) {
						for (int y = -filterSize/2; y <= filterSize/2; ++y) {
							float sampleX = shadowCoords.x + x * texelSize;
							float sampleY = shadowCoords.y + y * texelSize;

							if (sampleX >= 0.0f && sampleX <= 1.0f &&
								sampleY >= 0.0f && sampleY <= 1.0f) {

								int shadowX = static_cast<int>(std::round(sampleX * (shadowMapWidth - 1)));
								int shadowY = static_cast<int>(std::round(sampleY * (shadowMapHeight - 1)));

								shadowX = std::clamp(shadowX, 0, shadowMapWidth - 1);
								shadowY = std::clamp(shadowY, 0, shadowMapHeight - 1);

								int shadowIdx = shadowY * shadowMapWidth + shadowX;
								int maxIdx = shadowMapWidth * shadowMapHeight;

								if (shadowIdx >= 0 && shadowIdx < maxIdx && shadowMapData != nullptr) {
									float shadowDepth = shadowMapData[shadowIdx];
									float fragmentDepth = shadowCoords.z;

									float bias = 0.001f;

									bool inShadow = fragmentDepth > shadowDepth + bias;

									if (inShadow) {
										shadow += 1.0f;
									}
									samples++;
								}
							}
						}
					}

					if (samples > 0) {
						float shadowAmount = shadow / static_cast<float>(samples);
						shadowFactor = 1.0f - shadowAmount * 0.8f;
					}
				}
			}
		}

		finalColor = Lighting::calculateMultipleLights(
			pixel.worldPos,
			normal,
			cameraPos,
			baseColor,
			lights,
			material,
			shadowFactor
		);
	} else
		finalColor = baseColor;

	return QColor(
		std::clamp(static_cast<int>(finalColor.r * 255), 0, 255),
		std::clamp(static_cast<int>(finalColor.g * 255), 0, 255),
		std::clamp(static_cast<int>(finalColor.b * 255), 0, 255)
	);
}

bool Rasterizer::isOnScreen(const ScreenVertex &sv) const {
	return sv.position.x >= -100 && sv.position.x < image.width() + 100 &&
			sv.position.y >= -100 && sv.position.y < image.height() + 100 &&
			sv.depth >= 0 && sv.depth <= 1;
}

void Rasterizer::setShadowMap(const float *data, int w, int h, const glm::mat4 &mvp) {
	shadowMapData = data;
	shadowMapWidth = w;
	shadowMapHeight = h;
	lightMVP = mvp;
	useShadows = data != nullptr;
}

void Rasterizer::setLightingEnabled(bool enabled) {
	enableLighting = enabled;
}

void Rasterizer::addLight(const Light &light) {
	lights.push_back(light);
}

void Rasterizer::clearLights() {
	lights.clear();
}

void Rasterizer::setMaterial(const Lighting::Material &mat) {
	material = mat;
}

int Rasterizer::getWidth() const {
	return image.width();
}

int Rasterizer::getHeight() const {
	return image.height();
}

void Rasterizer::clearShadowMap() {
	shadowMapData = nullptr;
	shadowMapWidth = 0;
	shadowMapHeight = 0;
	useShadows = false;
}