#ifndef BASE_VISITOR_H
#define BASE_VISITOR_H

class PlaneObject;
class SceneObject;
class MeshObject;
class InstancedMeshObject;

class BaseVisitor {
public:
	virtual ~BaseVisitor() = default;
	virtual void visit(MeshObject& obj) = 0;
	virtual void visit(InstancedMeshObject& obj) = 0;
	virtual void visit(PlaneObject& obj) = 0;
};

#endif // BASE_VISITOR_H