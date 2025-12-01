#ifndef LIGHT_H
#define LIGHT_H

#include <glm/glm.hpp>

enum class LightType {
	Directional,
	Point,
	Spot
};

struct Light {
	LightType type;

	glm::vec3 position;
	glm::vec3 direction;

	glm::vec3 ambient;
	glm::vec3 diffuse;
	glm::vec3 specular;

	float constantAttenuation;
	float linearAttenuation;
	float quadraticAttenuation;

	float cutoffAngle;
	float outerCutoffAngle;

	static Light createDirectional(const glm::vec3 &dir) {
		Light light{};
		light.type = LightType::Directional;
		light.direction = glm::normalize(dir);
		light.position = glm::vec3(0);

		light.ambient = glm::vec3(0.2f, 0.2f, 0.2f);
		light.diffuse = glm::vec3(0.8f, 0.8f, 0.7f);
		light.specular = glm::vec3(1.0f, 1.0f, 0.9f);

		light.constantAttenuation = 1.0f;
		light.linearAttenuation = 0.0f;
		light.quadraticAttenuation = 0.0f;

		light.cutoffAngle = 0.0f;
		light.outerCutoffAngle = 0.0f;

		return light;
	}

	static Light createPoint(const glm::vec3 &pos) {
		Light light{};
		light.type = LightType::Point;
		light.position = pos;
		light.direction = glm::vec3(0);

		light.ambient = glm::vec3(0.1f, 0.1f, 0.1f);
		light.diffuse = glm::vec3(0.8f, 0.8f, 0.8f);
		light.specular = glm::vec3(1.0f, 1.0f, 1.0f);

		light.constantAttenuation = 1.0f;
		light.linearAttenuation = 0.09f;
		light.quadraticAttenuation = 0.032f;

		light.cutoffAngle = 0.0f;
		light.outerCutoffAngle = 0.0f;

		return light;
	}
};

#endif // LIGHT_H
