#ifndef L_SYS_TREE_GENERATOR_SHADOW_VISITOR_H
#define L_SYS_TREE_GENERATOR_SHADOW_VISITOR_H

#include "base_visitor.h"
#include "shadow_map_renderer.h"
#include <glm/glm.hpp>
#include <glm/gtx/quaternion.hpp>

class ShadowVisitor : public BaseVisitor {
public:
	ShadowVisitor(ShadowMapRenderer& sr, const glm::mat4& mvp);

	void visit(MeshObject& obj) override;
	void visit(InstancedMeshObject& obj) override;
	void visit(PlaneObject& obj) override;

private:
	ShadowMapRenderer& shadowRenderer;
	glm::mat4 lightMVP;
};

#endif //L_SYS_TREE_GENERATOR_SHADOW_VISITOR_H