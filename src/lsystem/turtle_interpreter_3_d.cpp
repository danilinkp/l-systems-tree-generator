#include "turtle_interpreter_3_d.h"
#include <iostream>
#include <random>
#include <glm/gtx/rotate_vector.hpp>

TurtleInterpreter3D::TurtleInterpreter3D()
	: state(), stepLength(1.0f), angle(glm::radians(25.7f)), baseRadius(0.1f), radiusDecay(0.7f), minLeafRadius(0.02f),
	  gravityFactor(0.05f), radialSegments(12) {
	reset();
}

void TurtleInterpreter3D::reset() {
	state.position = glm::vec3(0.0f, 0.0f, 0.0f);
	state.heading = glm::vec3(0.0f, 1.0f, 0.0f);
	state.left = glm::vec3(-1.0f, 0.0f, 0.0f);
	state.up = glm::vec3(0.0f, 0.0f, 1.0f);
	state.depth = 0;

	stateStack.clear();
	leafPositions.clear();
	root = std::make_shared<TreeNode>();
	root->position = state.position;
	root->direction = state.heading;
	currentNode = root;
}

void TurtleInterpreter3D::setStepLength(float length) {
	stepLength = length;
}

void TurtleInterpreter3D::setAngle(float degrees) {
	angle = glm::radians(degrees);
}

void TurtleInterpreter3D::setBaseRadius(float radius) {
	baseRadius = radius;
}

void TurtleInterpreter3D::setRadiusDecay(float decay) {
	radiusDecay = decay;
}

void TurtleInterpreter3D::setMinLeafRadius(float radius) {
	minLeafRadius = radius;
}

void TurtleInterpreter3D::setGravityFactor(float factor) {
	gravityFactor = factor;
}

void TurtleInterpreter3D::setRadialSegments(int segments) {
	radialSegments = segments;
}

Mesh TurtleInterpreter3D::interpret(const QString &commands) {
	reset();

	buildTree(commands);
	computeRadii(root);
	generateSplines(root);

	return generateMesh();
}

TurtleInterpreter3D::TreeMeshes TurtleInterpreter3D::interpretTree(const QString &commands) {
	reset();

	buildTree(commands);
	computeRadii(root);
	generateSplines(root);

	Mesh trunk = generateMesh();

	QVector<glm::vec3> leafPositions;
	QVector<glm::vec3> leafNormals;

	collectLeafPositionsAndNormals(root, leafPositions, leafNormals);

	return {std::move(trunk), std::move(leafPositions), std::move(leafNormals)};
}

void TurtleInterpreter3D::buildTree(const QString &commands) {
	float currentRadius = baseRadius * std::pow(radiusDecay, state.depth);
	currentNode->splinePoints.append(state.position);
	currentNode->splineRadii.append(currentRadius);

	for (const QChar &c : commands) {
		switch (c.toLatin1()) {
		case 'F':
		case 'G': {
			float currentLength = stepLength;

			float lengthDecayFactor = 0.8f;
			float depthMultiplier = std::pow(lengthDecayFactor, static_cast<float>(state.depth));

			if (state.depth == 0)
				depthMultiplier *= 2.42f;
			else if (state.depth == 1)
				depthMultiplier *= 1.2f;

			currentLength *= depthMultiplier;

			state.position += state.heading * currentLength;
			currentRadius = baseRadius * std::pow(radiusDecay, state.depth);
			currentNode->splinePoints.append(state.position);
			currentNode->splineRadii.append(currentRadius);
			break;
		}
		case 'f': {
			state.position += state.heading * stepLength;
			break;
		}
		case 'L': {
			if (state.depth == 0 || state.depth == 1)
				currentNode->hasLeaf = false;
			else
				currentNode->hasLeaf = true;
			break;
		}
		case '+': {
			static std::random_device rd;
			static std::mt19937 gen(rd());
			std::normal_distribution angleNoise(0.0f, angleVariation * 0.1f);
			float actualAngle = angle + glm::radians(angleNoise(gen));
			rotateAroundAxis(state.up, actualAngle);
			break;
		}
		case '-': {
			static std::random_device rd;
			static std::mt19937 gen(rd());
			std::normal_distribution angleNoise(0.0f, angleVariation * 0.1f);
			float actualAngle = -angle + glm::radians(angleNoise(gen));
			rotateAroundAxis(state.up, actualAngle);
			break;
		}
		case '&': {
			static std::random_device rd;
			static std::mt19937 gen(rd());
			std::normal_distribution angleNoise(0.0f, angleVariation * 0.1f);
			float actualAngle = angle + glm::radians(angleNoise(gen));
			rotateAroundAxis(state.left, actualAngle);
			break;
		}
		case '^': {
			static std::random_device rd;
			static std::mt19937 gen(rd());
			std::normal_distribution angleNoise(0.0f, angleVariation * 0.1f);
			float actualAngle = -angle + glm::radians(angleNoise(gen));
			rotateAroundAxis(state.left, actualAngle);
			break;
		}
		case '\\': {
			static std::random_device rd;
			static std::mt19937 gen(rd());
			std::normal_distribution angleNoise(0.0f, angleVariation * 0.1f);
			float actualAngle = angle + glm::radians(angleNoise(gen));
			rotateAroundAxis(state.heading, actualAngle);
			break;
		}
		case '/': {
			static std::random_device rd;
			static std::mt19937 gen(rd());
			std::normal_distribution angleNoise(0.0f, angleVariation * 0.1f);
			float actualAngle = -angle + glm::radians(angleNoise(gen));
			rotateAroundAxis(state.heading, actualAngle);
			break;
		}
		case '|': rotateAroundAxis(state.up, glm::radians(180.0f));
			break;
		case '[': {
			stateStack.push(state);
			auto child = std::make_shared<TreeNode>();
			child->position = state.position;
			child->direction = state.heading;
			child->depth = state.depth + 1;
			child->parent = currentNode;
			currentNode->children.append(child);
			currentNode = child;
			state.depth++;

			currentRadius = baseRadius * std::pow(radiusDecay, state.depth);
			currentNode->splinePoints.append(state.position);
			currentNode->splineRadii.append(currentRadius);
			break;
		}
		case ']': {
			if (!stateStack.isEmpty()) {
				state = stateStack.pop();
				currentNode = currentNode->parent.lock();
			}
			break;
		}
		}
	}
}

void TurtleInterpreter3D::computeRadii(const std::shared_ptr<TreeNode> &node) {
	if (node->children.isEmpty()) {
		node->radius = minLeafRadius;
		return;
	}

	for (auto &child : node->children)
		computeRadii(child);

	float sumChildAreas = 0.0f;
	for (auto &child : node->children)
		sumChildAreas += child->radius * child->radius;

	node->radius = std::sqrt(sumChildAreas);
}

void TurtleInterpreter3D::setBarkTexture(const QImage &texture) {
	barkTexture = texture;
}

void TurtleInterpreter3D::setLeafTexture(const QImage &texture) {
	leafTexture = texture;
}

void TurtleInterpreter3D::generateSplines(std::shared_ptr<TreeNode> node) {
	if (node->splinePoints.size() < 2) return;

	QVector<BranchSegment> interpolatedSegments;
	int resolution = static_cast<int>(splineResolution);

	for (int i = 0; i < node->splinePoints.size() - 1; i++) {
		glm::vec3 p0 = i == 0 ? node->splinePoints[i] : node->splinePoints[i - 1];
		glm::vec3 p1 = node->splinePoints[i];
		glm::vec3 p2 = node->splinePoints[i + 1];
		glm::vec3 p3 = i + 2 < node->splinePoints.size() ? node->splinePoints[i + 2] : p2;

		float r0 = i == 0 ? node->splineRadii[i] : node->splineRadii[i - 1];
		float r1 = node->splineRadii[i];
		float r2 = node->splineRadii[i + 1];
		float r3 = i + 2 < node->splineRadii.size() ? node->splineRadii[i + 2] : r2;

		for (int j = 0; j <= resolution; j++) {
			float t = static_cast<float>(j) / resolution;

			glm::vec3 pos = 0.5f * (
				2.0f * p1 +
				(-p0 + p2) * t +
				(2.0f * p0 - 5.0f * p1 + 4.0f * p2 - p3) * t * t +
				(-p0 + 3.0f * p1 - 3.0f * p2 + p3) * t * t * t
			);

			glm::vec3 deriv = 0.5f * (
				-p0 + p2 +
				(2.0f * p0 - 4.0f * p1 + 2.0f * p2) * t +
				(-p0 + 3.0f * p1 - 3.0f * p2 + p3) * t * t
			);
			glm::vec3 forward = glm::normalize(deriv);

			float radius = 0.5f * (
				2.0f * r1 +
				(-r0 + r2) * t +
				(2.0f * r0 - 5.0f * r1 + 4.0f * r2 - r3) * t * t +
				(-r0 + 3.0f * r1 - 3.0f * r2 + r3) * t * t * t
			);

			float normalizedPos = static_cast<float>(i * resolution + j) / (node->splinePoints.size() * resolution);

			float radiusDecayFactor = 1.0f - normalizedPos * (0.2f + 0.1f * node->depth);

			static std::random_device rd;
			static std::mt19937 gen(rd());
			std::normal_distribution radiusNoise(1.0f, radiusVariation);
			float radiusVariationFactor = radiusNoise(gen);
			radiusVariationFactor = glm::clamp(radiusVariationFactor, 0.9f, 1.1f);

			radius *= radiusDecayFactor * radiusVariationFactor;
			radius = std::max(0.001f, radius);

			BranchSegment seg{};
			seg.position = pos;
			seg.forward = forward;
			seg.radius = radius;
			interpolatedSegments.append(seg);
		}
	}

	node->branchSegments = interpolatedSegments;

	float segmentCount = node->branchSegments.size();
	for (int i = 0; i < node->branchSegments.size(); i++) {
		float factor = static_cast<float>(i) / segmentCount;

		float gravityEffect = gravityFactor * node->depth * factor * factor;

		float bendAngle = branchBendFactor * node->depth * std::sin(factor * M_PI * 2.0f);

		node->branchSegments[i].position.y -= gravityEffect;

		if (i > 0 && i < node->branchSegments.size() - 1) {
			glm::vec3 right, up;
			computeFrenetFrame(node->branchSegments[i].forward, right, up);
			node->branchSegments[i].position += right * bendAngle * 0.5f;
		}
	}

	for (auto &child : node->children)
		generateSplines(child);
}

void TurtleInterpreter3D::collectLeafPositions(std::shared_ptr<TreeNode> node) {
	if (node->hasLeaf && !node->branchSegments.isEmpty()) {
		int segmentCount = node->branchSegments.size();

		int leafCount = std::max(5, segmentCount / 2);

		for (int i = 0; i < leafCount; i++) {
			float t = leafCount == 1 ? 0.5f : static_cast<float>(i) / (leafCount - 1);

			t = std::pow(t, 1.5f);

			t = glm::clamp(t, 0.0f, 1.0f);

			int segIdx = static_cast<int>(t) * (segmentCount - 1);
			segIdx = std::clamp(segIdx, 0, segmentCount - 1);

			const auto &seg = node->branchSegments[segIdx];

			glm::vec3 right, up;
			computeFrenetFrame(seg.forward, right, up);

			static std::random_device rd;
			static std::mt19937 gen(rd());
			std::uniform_real_distribution<float> angleDist(0.0f, 2.0f * M_PI);
			std::uniform_real_distribution radiusDist(0.9f, 1.2f);
			std::uniform_real_distribution normalDist(0.8f, 1.3f);

			float angle = angleDist(gen);
			float radialOffset = seg.radius * radiusDist(gen);
			float normalOffset = radialOffset * normalDist(gen);

			glm::vec3 radialDir = std::cos(angle) * right + std::sin(angle) * up;
			glm::vec3 leafPos = seg.position + radialDir * normalOffset;

			leafPositions.append(leafPos);
		}
	}

	for (auto &child : node->children)
		collectLeafPositions(child);
}

void TurtleInterpreter3D::collectLeafPositionsAndNormals(
	std::shared_ptr<TreeNode> node,
	QVector<glm::vec3>& positions,
	QVector<glm::vec3>& normals)
{
	 if (node->hasLeaf && !node->branchSegments.isEmpty()) {
       int segmentCount = node->branchSegments.size();
       int startSegment = std::max(0, segmentCount - 3);

       static std::random_device rd;
       static std::mt19937 gen(rd());
       std::uniform_real_distribution<float> angleDist(0.0f, 2.0f * M_PI);
       std::uniform_real_distribution radiusDist(1.5f, 2.5f);
       std::uniform_real_distribution forwardDist(0.0f, 2.0f);
       std::uniform_real_distribution tiltDist(0.3f, 0.7f);

       for (int segIdx = startSegment; segIdx < segmentCount; segIdx++) {
          const auto& seg = node->branchSegments[segIdx];

          int leavesOnSegment = 1 + (rand() % 2);

          for (int i = 0; i < leavesOnSegment; i++) {
             glm::vec3 right, up;
             computeFrenetFrame(seg.forward, right, up);

             float forwardOffset = seg.radius * forwardDist(gen);

             float angle = angleDist(gen);

             float radialOffset = seg.radius * radiusDist(gen);

             glm::vec3 radialDir = std::cos(angle) * right + std::sin(angle) * up;

             glm::vec3 leafPos = seg.position
                               + seg.forward * forwardOffset
                               + radialDir * radialOffset;

             auto upBias = glm::vec3(0.0f, 0.5f, 0.0f);
             float tilt = tiltDist(gen);
             glm::vec3 leafNormal = glm::normalize(
                 radialDir * tilt + upBias * (1.0f - tilt)
             );

             positions.append(leafPos);
             normals.append(leafNormal);
          }
       }
    }

	for (auto& child : node->children)
		collectLeafPositionsAndNormals(child, positions, normals);
}

Mesh TurtleInterpreter3D::generateMesh() const {
	Mesh mesh;
	glm::vec3 brownColor(0.45f, 0.25f, 0.1f);

	struct ConnectionInfo {
		std::shared_ptr<TreeNode> node;
		QVector<uint32_t> vertexIndices;
	};

	QMap<std::shared_ptr<TreeNode>, ConnectionInfo> connections;

	std::function<void(std::shared_ptr<TreeNode>)> traverse = [&](const std::shared_ptr<TreeNode> &node) {
		if (!node->branchSegments.isEmpty()) {
			addTube(mesh, node->branchSegments, brownColor, radialSegments);

			ConnectionInfo info;
			info.node = node;
			uint32_t endRingStart = mesh.vertices.size() - radialSegments;
			for (int i = 0; i < radialSegments; i++) {
				info.vertexIndices.append(endRingStart + i);
			}
			connections[node] = info;

			for (auto &child : node->children) {
				if (connections.contains(node)) {
					uint32_t childStartIdx = mesh.vertices.size();
					traverse(child);

					auto &parentRing = connections[node].vertexIndices;
					uint32_t childFirstRing = childStartIdx;

					for (int i = 0; i < radialSegments; i++) {
						uint32_t i0 = parentRing[i];
						uint32_t i1 = parentRing[(i + 1) % radialSegments];
						uint32_t i2 = childFirstRing + i;
						uint32_t i3 = childFirstRing + ((i + 1) % radialSegments);

						mesh.addTriangle(i0, i2, i1);
						mesh.addTriangle(i1, i2, i3);
					}
				} else
					traverse(child);
			}
		}
	};

	traverse(root);

	return mesh;
}

void TurtleInterpreter3D::addTube(Mesh &mesh,
                                  const QVector<BranchSegment> &segments,
                                  const glm::vec3 &color,
                                  int segmentsPerRing) const {
	if (segments.size() < 2)
		return;

	uint32_t baseVertexIdx = mesh.vertices.size();

	float totalLength = 0.0f;
	QVector<float> segLengths;
	segLengths.resize(segments.size());
	segLengths[0] = 0.0f;
	for (int i = 1; i < segments.size(); i++) {
		float len = glm::length(segments[i].position - segments[i - 1].position);
		totalLength += len;
		segLengths[i] = totalLength;
	}

	auto sampleBarkTexture = [&](float u, float v) -> glm::vec3 {
		if (barkTexture.isNull())
			return color;

		u = u - std::floor(u);
		v = v - std::floor(v);

		int x = static_cast<int>(u * (barkTexture.width() - 1));
		int y = static_cast<int>(v * (barkTexture.height() - 1));

		x = std::clamp(x, 0, barkTexture.width() - 1);
		y = std::clamp(y, 0, barkTexture.height() - 1);

		QRgb pixel = barkTexture.pixel(x, y);
		return glm::vec3(qRed(pixel) / 255.0f, qGreen(pixel) / 255.0f, qBlue(pixel) / 255.0f);
	};

	std::random_device rd;
	std::mt19937 gen(rd());
	std::uniform_real_distribution noiseDist(-1.0f, 1.0f);

	for (int i = 0; i < segments.size(); i++) {
		const auto &seg = segments[i];
		glm::vec3 right, up;
		computeFrenetFrame(seg.forward, right, up);

		for (int j = 0; j < segmentsPerRing; j++) {
			float angle = 2.0f * M_PI * j / segmentsPerRing;
			glm::vec3 radialDir = std::cos(angle) * right + std::sin(angle) * up;

			float detailNoise = noiseDist(gen) * 0.03f;
			float radiusVar = 1.0f + detailNoise;

			float nodeThickness = 1.0f;
			if (i > 0 && i < segments.size() - 1) {
				float distToPrev = glm::length(segments[i].position - segments[i - 1].position);
				float distToNext = glm::length(segments[i + 1].position - segments[i].position);
				if (std::abs(distToPrev - distToNext) > 0.3f)
					nodeThickness = 1.15f;
			}

			Vertex v;
			v.position = seg.position + radialDir * seg.radius * radiusVar * nodeThickness;
			v.normal = glm::normalize(radialDir);

			float u = angle / (2.0f * M_PI);
			float v_coord = totalLength > 0.0f ? segLengths[i] / totalLength : 0.0f;
			v.texCoord = glm::vec2(u * 4.0f, v_coord * 3.0f);

			v.color = sampleBarkTexture(v.texCoord.x, v.texCoord.y);

			float heightFactor = static_cast<float>(i) / segments.size();
			float colorMult = 0.7f + heightFactor * 0.4f;
			v.color *= colorMult;

			mesh.vertices.append(v);
		}
	}

	for (int i = 0; i < segments.size() - 1; i++) {
		for (int j = 0; j < segmentsPerRing; j++) {
			uint32_t i0 = baseVertexIdx + i * segmentsPerRing + j;
			uint32_t i1 = baseVertexIdx + i * segmentsPerRing + (j + 1) % segmentsPerRing;
			uint32_t i2 = baseVertexIdx + (i + 1) * segmentsPerRing + j;
			uint32_t i3 = baseVertexIdx + (i + 1) * segmentsPerRing + (j + 1) % segmentsPerRing;

			mesh.addTriangle(i0, i1, i2);
			mesh.addTriangle(i1, i3, i2);
		}
	}
}

void TurtleInterpreter3D::computeFrenetFrame(const glm::vec3 &forward, glm::vec3 &right, glm::vec3 &up) {
	glm::vec3 arbitrary = (std::abs(forward.y) < 0.99f) ? glm::vec3(0, 1, 0) : glm::vec3(1, 0, 0);
	right = glm::normalize(glm::cross(arbitrary, forward));
	up = glm::cross(forward, right);
}

void TurtleInterpreter3D::rotateAroundAxis(const glm::vec3 &axis, float angle) {
	state.heading = glm::rotate(state.heading, angle, axis);
	state.left = glm::rotate(state.left, angle, axis);
	state.up = glm::rotate(state.up, angle, axis);
	state.heading = glm::normalize(state.heading);
	state.left = glm::normalize(state.left);
	state.up = glm::normalize(state.up);
}
