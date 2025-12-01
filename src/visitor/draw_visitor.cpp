#include "draw_visitor.h"
#include "mesh_object.h"
#include "instanced_mesh_object.h"
#include "plane_object.h"

#define GLM_ENABLE_EXPERIMENTAL
#include <glm/gtc/quaternion.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/quaternion.hpp>


DrawVisitor::DrawVisitor(Rasterizer& r, Camera& cam)
	: rasterizer(r), camera(cam) {}

void DrawVisitor::visit(MeshObject& obj) {
	glm::mat4 model = glm::translate(glm::mat4(1.0f), obj.getPosition());
	glm::mat4 mvp = camera.getProjectionMatrix(rasterizer.getWidth(), rasterizer.getHeight()) * camera.getViewMatrix() * model;

	rasterizer.renderMesh(obj.getMesh(), mvp, camera.getPosition(), obj.getTexture());
}

void DrawVisitor::visit(InstancedMeshObject& obj) {
	const auto& proto = obj.getPrototype();

    for (const auto& inst : obj.getInstances()) {
        glm::mat4 model = glm::translate(glm::mat4(1.0f), inst.position);
        model = model * glm::toMat4(inst.rotation);
        model = glm::scale(model, inst.scale);

        Mesh transformedMesh = proto;

        glm::mat3 normalMatrix = glm::transpose(glm::inverse(glm::mat3(model)));

        for (auto& vertex : transformedMesh.vertices) {
            glm::vec4 worldPos = model * glm::vec4(vertex.position, 1.0f);
            vertex.position = glm::vec3(worldPos);

            vertex.normal = glm::normalize(normalMatrix * vertex.normal);
        }

        glm::mat4 vp = camera.getProjectionMatrix(
            rasterizer.getWidth(),
            rasterizer.getHeight()
        ) * camera.getViewMatrix();


        rasterizer.renderMesh(transformedMesh, vp, camera.getPosition(), obj.getTexture());
    }
}

void DrawVisitor::visit(PlaneObject& obj) {
	glm::mat4 view = camera.getViewMatrix();
	glm::mat4 proj = camera.getProjectionMatrix(
		rasterizer.getWidth(),
		rasterizer.getHeight()
	);

	glm::mat4 model = glm::translate(glm::mat4(1.0f), obj.getPosition());
	glm::mat4 mvp = proj * view * model;

	rasterizer.renderMesh(
		obj.getMesh(),
		mvp,
		camera.getPosition(),
		obj.getTexture()
	);
}
