#include "mesh.h"

void Mesh::clear() {
  vertices.clear();
  triangles.clear();
}

void Mesh::addVertex(const glm::vec3 &pos, const glm::vec3 &normal, const glm::vec3 &color, const glm::vec2 &uv) {
  vertices.append(Vertex(pos, normal, color, uv));
}

void Mesh::addVertex(const glm::vec3 &pos, const glm::vec3 &normal, const glm::vec3 &color) {
  addVertex(pos, normal, color, glm::vec2(0.0f));
}

void Mesh::addTriangle(uint32_t i0, uint32_t i1, uint32_t i2) {
  triangles.append(Triangle(i0, i1, i2));
}