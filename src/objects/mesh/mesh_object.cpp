#include "mesh_object.h"
#include "base_visitor.h"

void MeshObject::accept(BaseVisitor &visitor) {
	visitor.visit(*this);
}
