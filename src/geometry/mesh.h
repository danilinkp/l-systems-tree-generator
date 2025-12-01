#ifndef MESH_H
#define MESH_H

#define GLM_ENABLE_EXPERIMENTAL
#include <glm/glm.hpp>
#include <QDebug>

struct Vertex {
  glm::vec3 position;
  glm::vec3 normal;
  glm::vec3 color;
  glm::vec2 texCoord;

  Vertex()
	  : position(0.0f), normal(0.0f, 1.0f, 0.0f), color(1.0f), texCoord(0.0f) {}

  Vertex(const glm::vec3 &pos, const glm::vec3 &norm, const glm::vec3 &col, const glm::vec2 &uv = glm::vec2(0.0f))
	  : position(pos), normal(norm), color(col), texCoord(uv) {}
};

struct VertexGroup {
  std::vector<int> indices;
  glm::vec3 avgNormal;
};

struct Triangle {
  uint32_t i0, i1, i2;

  Triangle(uint32_t a, uint32_t b, uint32_t c) : i0(a), i1(b), i2(c) {}
};

class Mesh {
public:
  QVector<Vertex> vertices;
  QVector<Triangle> triangles;

  void clear();

  void addVertex(const glm::vec3 &pos, const glm::vec3 &normal, const glm::vec3 &color);
  void addVertex(const glm::vec3 &pos, const glm::vec3 &normal, const glm::vec3 &color, const glm::vec2& uv);

  void addTriangle(uint32_t i0, uint32_t i1, uint32_t i2);
};

#endif // MESH_H