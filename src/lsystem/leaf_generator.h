#ifndef LEAF_GENERATOR_H
#define LEAF_GENERATOR_H

#include "mesh.h"
#include <glm/gtc/quaternion.hpp>
#include <QVector>

class LeafGenerator {
public:
	struct Instance {
		glm::vec3 position;
		glm::quat rotation;
		glm::vec3 scale = glm::vec3(1.0f);
	};

	struct LeafMesh {
		Mesh prototype;
		QVector<Instance> instances;
	};

	LeafMesh generate(const QVector<glm::vec3>& positions, const QVector<glm::vec3>& normals);

private:
	float leafSize = 0.2f;
	float rotationVariation = 0.5f;
};
#endif // LEAF_GENERATOR_H