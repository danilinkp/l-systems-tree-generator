#ifndef TEXTURE_LOADER_H
#define TEXTURE_LOADER_H

#include <QImage>
#include <glm/glm.hpp>

class TextureLoader {
public:
	static QImage loadTexture(const QString& filepath);

	static glm::vec3 sampleTexture(const QImage& texture, float u, float v);

	static glm::vec3 sampleTextureBilinear(const QImage& texture, float u, float v);

	static QImage createGrassTexture(int width, int height);
};

#endif // TEXTURE_LOADER_H