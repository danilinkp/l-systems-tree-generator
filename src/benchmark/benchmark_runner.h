#ifndef BENCHMARK_RUNNER_H
#define BENCHMARK_RUNNER_H

#include <QObject>
#include <QVector>
#include <QString>
#include <QPair>
#include <chrono>
#include <glm/glm.hpp>

// Заглушки для отсутствующих заголовков (предполагаем, что они существуют)
#include "scene.h"
#include "scene_renderer.h"
#include "camera_manager.h"
#include "l_system_generator.h"
#include "turtle_interpreter_3_d.h"
#include "leaf_generator.h"
#include "texture_loader.h"
#include "mesh_object.h"
#include "instanced_mesh_object.h"
#include "plane_object.h"

struct BenchmarkResult {
	int iterations;
	double generationTimeMs;
	double renderTimeMs;
	int triangleCount;
	int leafCount;

	int width = 0;
	int height = 0;
	int shadows = 0;

	float cameraYaw = -1.0f; // -1 означает "не задано"

	QString toString() const {
		return QString("Iter: %1, Gen: %2ms, Render: %3ms, Tris: %4, Leaves: %5")
				.arg(iterations)
				.arg(generationTimeMs, 0, 'f', 2)
				.arg(renderTimeMs, 0, 'f', 2)
				.arg(triangleCount)
				.arg(leafCount);
	}

	QString toCSVRow() const {
		return QString("%1,%2,%3,%4,%5,%6,%7,%8,%9")
				.arg(iterations)
				.arg(generationTimeMs, 0, 'f', 3)
				.arg(renderTimeMs, 0, 'f', 3)
				.arg(triangleCount)
				.arg(leafCount)
				.arg(width)
				.arg(height)
				.arg(shadows)
		.arg(cameraYaw, 0, 'f', 1); // новое поле

	}
};

class BenchmarkRunner : public QObject {
	Q_OBJECT

public:
	explicit BenchmarkRunner(QObject *parent = nullptr);

	void runFullSuite(const QString &outputFile = "benchmark_results.csv");

	// Этап 1: Глубина L-системы
	void testIterations(const QVector<int> &iterations, int numRuns = 5);

	// Этап 2: Разрешение финального изображения
	void testResolution(const QVector<QPair<int, int> > &resolutions, int iterations, int numRuns = 5);

	// Этап 3: Разрешение карты теней
	void testShadowMapResolution(const QVector<int> &resolutions, int iterations, int numRuns = 5);

	// Этап 4: Угол поворота камеры (орбитальный обзор)
	void testCameraAngle(const QVector<float> &angles, int iterations, int numRuns = 5);

	const QVector<BenchmarkResult> &getResults() const { return results; }

	void saveResults(const QString &filename) const;

signals:
	void progressUpdate(int current, int total, const QString &message);
	void benchmarkComplete(const QVector<BenchmarkResult> &results);

private:
	struct BenchmarkConfig {
		QString axiom = "F";
		QString rule = "F[+FL][-FL][&FL][^FL]";
		float angle = 25.0f;
		float stepLength = 1.2f;
		float baseRadius = 0.25f;
		float radiusDecay = 0.6f;
		float minLeafRadius = 0.02f;
		float gravityFactor = 0.01f;
		int radialSegments = 8;
		int imageWidth = 1920;
		int imageHeight = 1080;
		glm::vec3 lightDirection = glm::vec3(-1.0f, -2.0f, -1.0f);
		bool shadowsEnabled = true;
		// Новое поле для этапа 3: разрешение карты теней
		int shadowMapResolution = 2048;
	};

	BenchmarkResult runSingleTest(int iterations);
	BenchmarkResult averageResults(const QVector<BenchmarkResult> &runs);

	double measureTime(std::function<void()> func);

	BenchmarkConfig config;
	QVector<BenchmarkResult> results;

	QImage barkTexture;
	QImage grassTexture;
};

#endif // BENCHMARK_RUNNER_H
