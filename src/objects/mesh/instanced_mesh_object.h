#ifndef INSTANCED_MESH_OBJECT_H
#define INSTANCED_MESH_OBJECT_H

#include "scene_object.h"
#include "mesh.h"
#include <glm/gtc/quaternion.hpp>
#include <QVector>
#include <QImage>
#include "leaf_generator.h"
#include "lighting.h"

class InstancedMeshObject : public SceneObject {
public:
	struct Instance {
		glm::vec3 position;
		glm::quat rotation;
		glm::vec3 scale = glm::vec3(1.0f);
	};

	InstancedMeshObject(Mesh prototype, QVector<Instance> instances)
		: prototype_(std::move(prototype)), instances_(std::move(instances)), texture(nullptr) {
	}

	InstancedMeshObject(Mesh prototype, QVector<Instance> instances, const QImage *tex)
		: prototype_(std::move(prototype)), instances_(std::move(instances)), texture(tex) {
	}

	InstancedMeshObject(Mesh prototype,
	                    const QVector<LeafGenerator::Instance> &leafInstances,
	                    const QImage *tex = nullptr);

	[[nodiscard]] const Mesh &getPrototype() const { return prototype_; }
	[[nodiscard]] const QVector<Instance> &getInstances() const { return instances_; }

	[[nodiscard]] const QImage *getTexture() const { return texture; }
	void setTexture(const QImage *tex) { texture = tex; }

	void setMaterial(const Lighting::Material &mat) { material = mat; }

	const Lighting::Material &getMaterial() const { return material; }

	void accept(BaseVisitor &visitor) override;

private:
	Mesh prototype_;
	QVector<Instance> instances_;
	const QImage *texture;
	Lighting::Material material;
};

#endif // INSTANCED_MESH_OBJECT_H
