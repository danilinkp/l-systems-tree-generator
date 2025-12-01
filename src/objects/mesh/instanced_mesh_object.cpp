#include "instanced_mesh_object.h"
#include "base_visitor.h"

InstancedMeshObject::InstancedMeshObject(Mesh prototype, const QVector<LeafGenerator::Instance>& leafInstances, const QImage* tex)
	: prototype_(std::move(prototype)), texture(tex) {
	for (const auto& leafInst : leafInstances) {
		Instance inst;
		inst.position = leafInst.position;
		inst.rotation = leafInst.rotation;
		inst.scale = leafInst.scale;
		instances_.append(inst);
	}
}

void InstancedMeshObject::accept(BaseVisitor& visitor) {
	visitor.visit(*this);
}