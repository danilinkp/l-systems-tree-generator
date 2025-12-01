#include "leaf_generator.h"
#include <random>
#include <thread>
#include <vector>
#include <algorithm>

LeafGenerator::LeafMesh LeafGenerator::generate(
	const QVector<glm::vec3> &positions,
	const QVector<glm::vec3> &normals) {
	Mesh prototype;

	glm::vec3 darkGreen(0.05f, 0.35f, 0.05f);
	glm::vec3 green(0.15f, 0.6f, 0.1f);
	glm::vec3 lightGreen(0.2f, 0.7f, 0.15f);
	glm::vec3 yellowEdge(0.3f, 0.65f, 0.1f);

	float width = leafSize * 0.3f;
	float height = leafSize * 0.6f;
	float tipOffset = leafSize * 0.1f;

	Vertex v0(glm::vec3(0, -height * 0.3f, 0), glm::vec3(0, 0, 1), darkGreen, glm::vec2(0.5f, 0.1f));
	Vertex v1(glm::vec3(-width, 0, 0), glm::vec3(0, 0, 1), yellowEdge, glm::vec2(0.1f, 0.5f));

	Vertex v2(glm::vec3(0, height + tipOffset, 0), glm::vec3(0, 0, 1), lightGreen, glm::vec2(0.5f, 0.9f));

	Vertex v3(glm::vec3(width, 0, 0), glm::vec3(0, 0, 1), green, glm::vec2(0.9f, 0.5f));

	prototype.vertices.append(v0);
	prototype.vertices.append(v1);
	prototype.vertices.append(v2);
	prototype.vertices.append(v3);

	prototype.addTriangle(0, 1, 2);
	prototype.addTriangle(0, 2, 3);

	glm::vec3 backTint(0.7f, 0.7f, 0.7f);
	Vertex v0b(glm::vec3(0, -height * 0.3f, 0), glm::vec3(0, 0, -1), darkGreen * backTint, glm::vec2(0.5f, 0.1f));
	Vertex v1b(glm::vec3(-width, 0, 0), glm::vec3(0, 0, -1), yellowEdge * backTint, glm::vec2(0.1f, 0.5f));
	Vertex v2b(glm::vec3(0, height + tipOffset, 0), glm::vec3(0, 0, -1), lightGreen * backTint, glm::vec2(0.5f, 0.9f));
	Vertex v3b(glm::vec3(width, 0, 0), glm::vec3(0, 0, -1), green * backTint, glm::vec2(0.9f, 0.5f));

	prototype.vertices.append(v0b);
	prototype.vertices.append(v1b);
	prototype.vertices.append(v2b);
	prototype.vertices.append(v3b);

	prototype.addTriangle(4, 6, 5);
	prototype.addTriangle(4, 7, 6);

	QVector<Instance> instances;
	instances.resize(positions.size());

	const size_t numLeaves = positions.size();
	
	if (numLeaves < 50) {
		std::random_device rd;
		std::mt19937 gen(rd());
		std::uniform_real_distribution rotDist(-1.0f, 1.0f);
		std::uniform_real_distribution sizeDist(0.8f, 1.2f);

		for (int i = 0; i < positions.size(); i++) {
			Instance inst{};
			inst.position = positions[i];

			glm::vec3 normal = glm::normalize(normals[i]);

			glm::vec3 up = glm::vec3(0, 1, 0);
			glm::vec3 right = glm::normalize(glm::cross(up, normal));
			if (glm::length(right) < 0.001f) {
				right = glm::vec3(1, 0, 0);
				up = glm::normalize(glm::cross(normal, right));
			}
			else
				up = glm::normalize(glm::cross(normal, right));

			glm::mat3 orient(right, up, normal);

			float rx = rotDist(gen) * rotationVariation * 0.5f;
			float ry = rotDist(gen) * rotationVariation * 1.0f;
			float rz = rotDist(gen) * rotationVariation * 0.5f;

			glm::quat qX = glm::angleAxis(rx, glm::vec3(1, 0, 0));
			glm::quat qY = glm::angleAxis(ry, glm::vec3(0, 1, 0));
			glm::quat qZ = glm::angleAxis(rz, glm::vec3(0, 0, 1));

			glm::quat baseRotation = glm::quat_cast(glm::mat4(orient));

			glm::quat flipRotation = glm::angleAxis(glm::radians(180.0f), glm::vec3(1, 0, 0));

			inst.rotation = qZ * qX * qY * flipRotation * baseRotation;

			float scale = sizeDist(gen);
			inst.scale = glm::vec3(scale);

			instances[i] = inst;
		}
	} else {
		constexpr size_t numThreads = 8;
		std::vector<std::thread> threads;
		const size_t leavesPerThread = (numLeaves + numThreads - 1) / numThreads;

		for (size_t t = 0; t < numThreads; ++t) {
			threads.emplace_back([&positions, &normals, &instances, t, leavesPerThread, numLeaves, this]() {
				size_t start = t * leavesPerThread;
				size_t end = std::min(start + leavesPerThread, numLeaves);

				std::random_device rd;
				std::mt19937 gen(rd() + static_cast<unsigned>(t));
				std::uniform_real_distribution rotDist(-1.0f, 1.0f);
				std::uniform_real_distribution sizeDist(0.8f, 1.2f);

				for (size_t i = start; i < end; i++) {
					Instance inst{};
					inst.position = positions[i];

					glm::vec3 normal = glm::normalize(normals[i]);

					glm::vec3 up = glm::vec3(0, 1, 0);
					glm::vec3 right = glm::normalize(glm::cross(up, normal));
					if (glm::length(right) < 0.001f) {
						right = glm::vec3(1, 0, 0);
						up = glm::normalize(glm::cross(normal, right));
					}
					else
						up = glm::normalize(glm::cross(normal, right));

					glm::mat3 orient(right, up, normal);

					float rx = rotDist(gen) * rotationVariation * 0.5f;
					float ry = rotDist(gen) * rotationVariation * 1.0f;
					float rz = rotDist(gen) * rotationVariation * 0.5f;

					glm::quat qX = glm::angleAxis(rx, glm::vec3(1, 0, 0));
					glm::quat qY = glm::angleAxis(ry, glm::vec3(0, 1, 0));
					glm::quat qZ = glm::angleAxis(rz, glm::vec3(0, 0, 1));

					glm::quat baseRotation = glm::quat_cast(glm::mat4(orient));

					glm::quat flipRotation = glm::angleAxis(glm::radians(180.0f), glm::vec3(1, 0, 0));

					inst.rotation = qZ * qX * qY * flipRotation * baseRotation;

					float scale = sizeDist(gen);
					inst.scale = glm::vec3(scale);

					instances[i] = inst;
				}
			});
		}

		for (auto& thread : threads) {
			thread.join();
		}
	}

	return {std::move(prototype), std::move(instances)};
}
