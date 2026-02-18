#ifndef TURTLEINTERPRETER3D_H
#define TURTLEINTERPRETER3D_H

#define GLM_ENABLE_EXPERIMENTAL
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/string_cast.hpp>
#include "mesh.h"
#include <QStack>
#include <QDebug>
#include <QImage>
#include <memory>
#include <random>

struct BranchSegment {
	glm::vec3 position;
	glm::vec3 forward;
	float radius;
};

struct Branch {
	QVector<BranchSegment> segments;
	int depth;
};

struct TreeNode {
	glm::vec3 position;
	glm::vec3 direction;
	float radius = 0.0f;
	QVector<std::shared_ptr<TreeNode> > children;
	QVector<BranchSegment> branchSegments;
	int depth = 0;
	std::weak_ptr<TreeNode> parent;
	QVector<glm::vec3> splinePoints;
	QList<float> splineRadii;
	bool isTerminal = false;
	bool hasLeaf = false;
};

class TurtleInterpreter3D {
public:
	struct TreeMeshes {
		Mesh trunk;
		QVector<glm::vec3> leafPositions;
		QVector<glm::vec3> leafNormals;
	};

	TurtleInterpreter3D();

	void setStepLength(float length);
	void setAngle(float degrees);
	void setBaseRadius(float radius);
	void setRadiusDecay(float decay);
	void setMinLeafRadius(float radius);
	void setGravityFactor(float factor);
	void setRadialSegments(int segments);
	void setSplineResolution(float resolution) { splineResolution = resolution; }
	void setBranchBendFactor(float factor) { branchBendFactor = factor; }
	void setRadiusVariation(float variation) { radiusVariation = variation; }

	void setBarkTexture(const QImage &texture);
	void setLeafTexture(const QImage &texture);

	Mesh interpret(const QString &commands);
	TreeMeshes interpretTree(const QString &commands);
	void reset();

private:
	void rotateAroundAxis(const glm::vec3 &axis, float angle);
	void buildTree(const QString &commands);
	void computeRadii(const std::shared_ptr<TreeNode> &node);
	void generateSplines(std::shared_ptr<TreeNode> node);
	Mesh generateMesh() const;
	void addTube(Mesh &mesh, const QVector<BranchSegment> &segments, const glm::vec3 &color, int segmentsPerRing) const;
	static void computeFrenetFrame(const glm::vec3 &forward, glm::vec3 &right, glm::vec3 &up);
	void collectLeafPositions(std::shared_ptr<TreeNode> node);
	void collectLeafPositionsAndNormals(std::shared_ptr<TreeNode> node,
	                                    QVector<glm::vec3> &positions,
	                                    QVector<glm::vec3> &normals);

	struct TurtleState3D {
		glm::vec3 position;
		glm::vec3 heading;
		glm::vec3 left;
		glm::vec3 up;
		int depth;
	};

	TurtleState3D state;
	QStack<TurtleState3D> stateStack;
	std::shared_ptr<TreeNode> root;
	std::shared_ptr<TreeNode> currentNode;
	QVector<glm::vec3> leafPositions;

	float stepLength;
	float angle;
	float baseRadius;
	float radiusDecay;
	float minLeafRadius;
	float gravityFactor;
	int radialSegments;

	float angleVariation = 3.0f;
	float splineResolution = 2.0f;
	float branchBendFactor = 0.02f;
	float radiusVariation = 0.05f;

	QImage barkTexture;
	QImage leafTexture;

	mutable std::mt19937 rng;
};

#endif // TURTLEINTERPRETER3D_H
