glm::vec3 Lighting::calculatePhong(const glm::vec3 &fragPos, const glm::vec3 &normal, const glm::vec3 &viewPos,
                                   const glm::vec3 &baseColor, const Light &light, const Material &material) {
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