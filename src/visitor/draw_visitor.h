#ifndef DRAW_VISITOR_H
#define DRAW_VISITOR_H

#include "base_visitor.h"
#include "rasterizer.h"
#include "camera.h"

class DrawVisitor : public BaseVisitor {
public:
	DrawVisitor(Rasterizer& r, Camera& cam);

	void visit(MeshObject& obj) override;
	void visit(InstancedMeshObject& obj) override;
	void visit(PlaneObject& obj) override;
private:
	Rasterizer& rasterizer;
	Camera& camera;
};


#endif // DRAW_VISITOR_H