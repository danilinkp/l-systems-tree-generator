#include "texture_loader.h"
#include <QDebug>

QImage TextureLoader::loadTexture(const QString &filepath) {
	QImage texture(filepath);

	if (texture.isNull()) {
		qDebug() << "Failed to load texture:" << filepath;
		texture = QImage(256, 256, QImage::Format_RGB888);
		texture.fill(QColor(100, 70, 40)); // коричневый
	} else {
		qDebug() << "Loaded texture:" << filepath << "size:" << texture.width() << "x" << texture.height();
		texture = texture.convertToFormat(QImage::Format_RGB888);
	}

	return texture;
}

glm::vec3 TextureLoader::sampleTexture(const QImage &texture, float u, float v) {
	if (texture.isNull())
		return {0.5f, 0.35f, 0.2f};

	u = u - std::floor(u);
	v = v - std::floor(v);

	int x = static_cast<int>(u * (texture.width() - 1));
	int y = static_cast<int>(v * (texture.height() - 1));

	x = std::clamp(x, 0, texture.width() - 1);
	y = std::clamp(y, 0, texture.height() - 1);

	QRgb pixel = texture.pixel(x, y);

	return {
		qRed(pixel) / 255.0f,
		qGreen(pixel) / 255.0f,
		qBlue(pixel) / 255.0f
	};
}

glm::vec3 TextureLoader::sampleTextureBilinear(const QImage &texture, float u, float v) {
	if (texture.isNull()) {
		return {0.5f, 0.35f, 0.2f};
	}
	u = u - std::floor(u);
	v = v - std::floor(v);

	float fx = u * (texture.width() - 1);
	float fy = v * (texture.height() - 1);

	int x0 = static_cast<int>(fx);
	int y0 = static_cast<int>(fy);
	int x1 = (x0 + 1) % texture.width();
	int y1 = (y0 + 1) % texture.height();

	float tx = fx - x0;
	float ty = fy - y0;

	auto getColor = [&](int x, int y) -> glm::vec3 {
		QRgb pixel = texture.pixel(x, y);
		return glm::vec3(qRed(pixel) / 255.0f, qGreen(pixel) / 255.0f, qBlue(pixel) / 255.0f);
	};

	glm::vec3 c00 = getColor(x0, y0);
	glm::vec3 c10 = getColor(x1, y0);
	glm::vec3 c01 = getColor(x0, y1);
	glm::vec3 c11 = getColor(x1, y1);

	glm::vec3 c0 = glm::mix(c00, c10, tx);
	glm::vec3 c1 = glm::mix(c01, c11, tx);

	return glm::mix(c0, c1, ty);
}

QImage TextureLoader::createGrassTexture(int width, int height) {
	QImage texture(width, height, QImage::Format_RGB32);

	QColor baseGreen(90, 140, 70);
	QColor darkGreen(70, 120, 50);
	QColor lightGreen(110, 160, 80);

	for (int y = 0; y < height; ++y) {
		for (int x = 0; x < width; ++x) {
			int noise = (x * 127 + y * 311) % 100;

			QColor color;
			if (noise < 40) {
				color = darkGreen;
			} else if (noise < 80) {
				color = baseGreen;
			} else {
				color = lightGreen;
			}

			texture.setPixel(x, y, color.rgb());
		}
	}

	return texture;
}
