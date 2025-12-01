#ifndef LIGHTING_H
#define LIGHTING_H

#include <glm/glm.hpp>
#include <algorithm>
#include <QDebug>
#include "light.h"

class Lighting {
public:
	struct Material {
		glm::vec3 ambient;
		glm::vec3 diffuse;
		glm::vec3 specular;
		float shininess;

		Material()
			: ambient(0.2f),
			  diffuse(0.8f),
			  specular(0.3f),
			  shininess(10.0f) {
		}
	};

	static glm::vec3 calculatePhong(const glm::vec3 &fragPos,
	                                const glm::vec3 &normal,
	                                const glm::vec3 &viewPos,
	                                const glm::vec3 &baseColor,
	                                const Light &light,
	                                const Material &material = Material());

	static glm::vec3 calculateSimple(const glm::vec3 &normal,
	                                 const glm::vec3 &baseColor,
	                                 const Light &light);

	static glm::vec3 calculateMultipleLights(const glm::vec3 &fragPos,
	                                         const glm::vec3 &normal,
	                                         const glm::vec3 &viewPos,
	                                         const glm::vec3 &baseColor,
	                                         const std::vector<Light> &lights,
	                                         const Material &material = Material(),
	                                         float shadowFactor = 1.0f);
};

#endif // LIGHTING_H
