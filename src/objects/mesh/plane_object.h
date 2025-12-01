#ifndef PLANE_OBJECT_H
#define PLANE_OBJECT_H

#include "scene_object.h"
#include "mesh.h"
#include <QImage>

class PlaneObject : public SceneObject {
public:
	explicit PlaneObject(float size = 20.0f, int segments = 10, const QImage* texture = nullptr);

	void accept(BaseVisitor& visitor) override;

	const Mesh& getMesh() const { return mesh; }
	const QImage* getTexture() const { return texture; }

	glm::vec3 getPosition() const override { return position; }
	void setPosition(const glm::vec3& pos) override { position = pos; }

private:
	Mesh mesh;
	const QImage* texture;
	glm::vec3 position;

	void generatePlaneMesh(float size, int segments);
};

#endif // PLANE_OBJECT_H