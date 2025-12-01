#ifndef L_SYS_TREE_GENERATOR_OBJECTS_MESH_OBJECT_H
#define L_SYS_TREE_GENERATOR_OBJECTS_MESH_OBJECT_H

#include "../scene_object.h"
#include "mesh.h"
#include <QImage>

class MeshObject : public SceneObject {
public:
	explicit MeshObject(Mesh mesh)
		: mesh(std::move(mesh)), texture(nullptr) {}

	MeshObject(Mesh mesh, const QImage* tex)
		: mesh(std::move(mesh)), texture(tex) {}

	[[nodiscard]] const Mesh &getMesh() const { return mesh; }
	Mesh &getMesh() { return mesh; }

	void setMesh(Mesh newMesh) { mesh = std::move(newMesh); }

	[[nodiscard]] const QImage* getTexture() const { return texture; }
	void setTexture(const QImage* tex) { texture = tex; }

	void accept(BaseVisitor &visitor) override;

private:
	Mesh mesh;
	const QImage* texture;
};

#endif //L_SYS_TREE_GENERATOR_OBJECTS_MESH_OBJECT_H