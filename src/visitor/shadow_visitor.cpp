#include "shadow_visitor.h"
#include "mesh_object.h"
#include "instanced_mesh_object.h"
#include "plane_object.h"

#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/quaternion.hpp>

ShadowVisitor::ShadowVisitor(ShadowMapRenderer& renderer, const glm::mat4& lightMVP)
	: shadowRenderer(renderer), lightMVP(lightMVP) {
}

void ShadowVisitor::visit(MeshObject& obj) {
	glm::mat4 model = glm::translate(glm::mat4(1.0f), obj.getPosition());
	glm::mat4 mvp = lightMVP * model;
	shadowRenderer.renderMesh(obj.getMesh(), mvp);
}

void ShadowVisitor::visit(InstancedMeshObject& obj) {
	const auto& prototype = obj.getPrototype();

	for (const auto& instance : obj.getInstances()) {
		glm::mat4 model = glm::translate(glm::mat4(1.0f), instance.position);
		model = model * glm::toMat4(instance.rotation);
		model = glm::scale(model, instance.scale);
		glm::mat4 mvp = lightMVP * model;
		shadowRenderer.renderMesh(prototype, mvp);
	}
}

void ShadowVisitor::visit(PlaneObject &obj) {
	glm::mat4 model = glm::translate(glm::mat4(1.0f), obj.getPosition());
	glm::mat4 mvp = lightMVP * model;
	shadowRenderer.renderMesh(obj.getMesh(), mvp);
}

