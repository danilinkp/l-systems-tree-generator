#include "lighting.h"

glm::vec3 Lighting::calculatePhong(const glm::vec3 &fragPos,
                                   const glm::vec3 &normal,
                                   const glm::vec3 &viewPos,
                                   const glm::vec3 &baseColor,
                                   const Light &light,
                                   const Material &material) {
	glm::vec3 N = glm::normalize(normal);
	glm::vec3 V = glm::normalize(viewPos - fragPos);

	glm::vec3 ambient = light.ambient * material.ambient * baseColor;

	glm::vec3 L;
	float attenuation = 1.0f;

	if (light.type == LightType::Directional) {
		L = -glm::normalize(light.direction);
	} else {
		L = glm::normalize(light.position - fragPos);
		float distance = glm::length(light.position - fragPos);
		attenuation = 1.0f / (
			light.constantAttenuation +
			light.linearAttenuation * distance +
			light.quadraticAttenuation * distance * distance
		);
	}

	float diff = glm::dot(N, L);

	float frontDiff = diff;
	float backDiff = glm::dot(-N, L);

	if (backDiff > frontDiff && backDiff > 0.0f) {
		diff = std::max(0.0f, backDiff) * 0.9f;
	} else {
		diff = std::max(0.0f, frontDiff);
	}

	glm::vec3 diffuse = light.diffuse * diff * material.diffuse * baseColor;

	glm::vec3 specular(0.0f);

	if (diff > 0.0f) {
		glm::vec3 H = glm::normalize(L + V);
		float spec = std::pow(std::max(0.0f, glm::dot(N, H)), material.shininess);
		specular = light.specular * spec * material.specular;
	}

	glm::vec3 result = ambient + (diffuse + specular) * attenuation;
	return glm::clamp(result, 0.0f, 1.0f);
}

glm::vec3 Lighting::calculateSimple(
	const glm::vec3 &normal,
	const glm::vec3 &baseColor,
	const Light &light
) {
	glm::vec3 N = glm::normalize(normal);
	glm::vec3 ambient = light.ambient * baseColor * 0.3f;

	glm::vec3 L = (light.type == LightType::Directional)
		              ? -glm::normalize(light.direction)
		              : glm::normalize(light.position);

	float diff = std::max(0.0f, glm::dot(N, L));
	glm::vec3 diffuse = light.diffuse * diff * baseColor;

	glm::vec3 result = ambient + diffuse;
	return glm::clamp(result, 0.0f, 1.0f);
}

glm::vec3 Lighting::calculateMultipleLights(const glm::vec3 &fragPos,
                                            const glm::vec3 &normal,
                                            const glm::vec3 &viewPos,
                                            const glm::vec3 &baseColor,
                                            const std::vector<Light> &lights,
                                            const Material &material,
                                            float shadowFactor) {
	glm::vec3 result(0.0f);

	for (const auto &light : lights) {
		glm::vec3 lightContribution = calculatePhong(fragPos, normal, viewPos, baseColor, light, material);

		glm::vec3 ambientPart = light.ambient * material.ambient * baseColor;
		glm::vec3 diffuseSpecularPart = lightContribution - ambientPart;

		glm::vec3 shadowedDiffuseSpec = diffuseSpecularPart * shadowFactor;

		result += ambientPart + shadowedDiffuseSpec;
	}

	return glm::clamp(result, 0.0f, 1.0f);
}
