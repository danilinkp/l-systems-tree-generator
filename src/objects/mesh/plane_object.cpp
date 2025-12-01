#include "plane_object.h"

#include <iostream>

#include "base_visitor.h"

PlaneObject::PlaneObject(float size, int segments, const QImage* texture)
    : texture(texture), position(0.0f, 0.0f, 0.0f) {
    generatePlaneMesh(size, segments);
}

void PlaneObject::accept(BaseVisitor& visitor) {
    visitor.visit(*this);
}

void PlaneObject::generatePlaneMesh(float size, int segments) {
    mesh.vertices.clear();
    mesh.triangles.clear();
    
    float halfSize = size * 0.5f;
    float step = size / segments;

    for (int z = 0; z <= segments; ++z) {
        for (int x = 0; x <= segments; ++x) {
            float xPos = -halfSize + x * step;
            float zPos = -halfSize + z * step;

            float u = static_cast<float>(x) / segments;
            float v = static_cast<float>(z) / segments;

            Vertex vertex;
            vertex.position = glm::vec3(xPos, 0.0f, zPos);
            vertex.normal = glm::vec3(0.0f, 1.0f, 0.0f);
            vertex.texCoord = glm::vec2(u * 5.0f, v * 5.0f);
            vertex.color = glm::vec3(0.65f, 0.85f, 0.6f);

            mesh.vertices.push_back(vertex);
        }
    }

    for (int z = 0; z < segments; ++z) {
        for (int x = 0; x < segments; ++x) {
            int topLeft = z * (segments + 1) + x;
            int topRight = topLeft + 1;
            int bottomLeft = (z + 1) * (segments + 1) + x;
            int bottomRight = bottomLeft + 1;

            Triangle tri1(topLeft, topRight, bottomLeft);
            Triangle tri2(topRight, bottomRight, bottomLeft);

            mesh.triangles.push_back(tri2);
            mesh.triangles.push_back(tri1);
        }
    }
}
