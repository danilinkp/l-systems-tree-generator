#ifndef L_SYS_TREE_GENERATOR_OBJECTS_SCENE_OBJECT_H
#define L_SYS_TREE_GENERATOR_OBJECTS_SCENE_OBJECT_H

#include <glm/glm.hpp>

class BaseVisitor;

class SceneObject {
protected:
	const size_t id;
	glm::vec3 position{};
	static size_t nextId;

public:
	SceneObject() : id(nextId++), position(0.0f) {
	}

	virtual ~SceneObject() = default;

	size_t getId() const { return id; }

	virtual glm::vec3 getPosition() const { return position; }
	virtual void setPosition(const glm::vec3 &pos) { position = pos; }

	virtual void accept(BaseVisitor &visitor) = 0;
};

#endif //L_SYS_TREE_GENERATOR_OBJECTS_SCENE_OBJECT_H
