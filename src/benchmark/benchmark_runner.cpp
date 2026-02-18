#include "benchmark_runner.h"
#include <QFile>
#include <QTextStream>
#include <QDebug>
#include <chrono>
#include <functional>

#include "orbit_camera.h"

BenchmarkRunner::BenchmarkRunner(QObject *parent)
	: QObject(parent) {
	barkTexture = TextureLoader::loadTexture("../textures/bark_2.jpg");
	grassTexture = TextureLoader::loadTexture("../textures/grass_2.jpg");
}

void BenchmarkRunner::runFullSuite(const QString &outputFile) {
	qDebug() << "=== Starting Full Benchmark Suite ===";
	results.clear();

	const int numRuns = 10;
	const int fixedIterations = 5;

	// QVector<int> iterations = {2, 3, 4, 5, 6};
	// qDebug() << "\n--- Starting Stage 1: Iterations (n={2,3,4,5,6}) ---";
	// testIterations(iterations, 10);
	//
	// QVector<QPair<int, int>> resolutions = {
	// 	{256, 144},
	// 	{426, 240},
	// 	{640, 360},
	// 	{854, 480},
	// 	{1280, 720},
	// 	{1920, 1080},
	// 	{2560, 1440}
	// };
	// qDebug() << "\n--- Starting Stage 2: Resolution (r) at Iteration" << fixedIterations << "---";
	// testResolution(resolutions, fixedIterations, numRuns);
	//
	// QVector<int> shadowResolutions = {128, 256, 512, 1024, 2048, 4096};
	// qDebug() << "\n--- Starting Stage 3: Shadow Map Resolution (s) at Iteration" << fixedIterations << "---";
	// testShadowMapResolution(shadowResolutions, fixedIterations, numRuns);

	QVector<float> yawAngles = {
		0.0f, 15.0f, 30.0f, 45.0f, 60.0f, 75.0f, 90.0f, 105.0f, 120.0f, 135.0f, 150.0f, 165.0f, 180.0f, 195.0f, 210.0f,
		225.0f, 240.0f, 255.0f, 270.0f, 285.0f, 300.0f, 315.0f, 330.0f, 345.0f
	};
	qDebug() << "\n--- Starting Stage 4: Camera Orbit Angle ---";
	testCameraAngle(yawAngles, fixedIterations, numRuns);

	saveResults(outputFile);

	qDebug() << "=== Benchmark Suite Completed ===";
	emit benchmarkComplete(results);
}

void BenchmarkRunner::testIterations(const QVector<int> &iterations, int numRuns) {
	qDebug() << "\n=== Stage 1: Testing Iterations (single thread) ===";

	int current = 0;

	BenchmarkConfig originalConfig = config;

	config.imageWidth = originalConfig.imageWidth;
	config.imageHeight = originalConfig.imageHeight;

	for (int iter : iterations) {
		qDebug() << "\nTesting iterations:" << iter;

		QVector<BenchmarkResult> runs;

		for (int run = 0; run < numRuns; ++run) {
			current++;
			qDebug() << "  Run" << (run + 1) << "of" << numRuns;

			BenchmarkResult result = runSingleTest(iter);
			runs.append(result);
		}

		BenchmarkResult avg = averageResults(runs);
		results.append(avg);

		qDebug() << "  Average:" << avg.toString();
	}

	config = originalConfig;
}

void BenchmarkRunner::testResolution(const QVector<QPair<int, int> > &resolutions, int iterations, int numRuns) {
	qDebug() << "\n=== Stage 2: Testing Resolution (fixed Iterations: " << iterations << ") ===";

	BenchmarkConfig originalConfig = config;

	config.imageWidth = originalConfig.imageWidth;
	config.imageHeight = originalConfig.imageHeight;

	int total = resolutions.size() * numRuns;
	int current = 0;

	for (const auto &res : resolutions) {
		config.imageWidth = res.first;
		config.imageHeight = res.second;
		qDebug() << "\nTesting resolution:" << config.imageWidth << "x" << config.imageHeight;

		QVector<BenchmarkResult> runs;

		for (int run = 0; run < numRuns; ++run) {
			current++;
			emit progressUpdate(current,
			                    total,
			                    QString("Resolution %1x%2, Run %3/%4").arg(res.first).arg(res.second).arg(run + 1).arg(
				                    numRuns));

			qDebug() << "  Run" << (run + 1) << "of" << numRuns;

			BenchmarkResult result = runSingleTest(iterations);
			runs.append(result);
		}

		BenchmarkResult avg = averageResults(runs);
		results.append(avg);
		qDebug() << "  Average:" << avg.toString();
	}

	config = originalConfig;
}

void BenchmarkRunner::testShadowMapResolution(const QVector<int> &resolutions, int iterations, int numRuns) {
	qDebug() << "\n=== Stage 3: Testing Shadow Map Resolution (fixed Iterations: " << iterations << ") ===";

	BenchmarkConfig originalConfig = config;

	config.imageWidth = 1920;
	config.imageHeight = 1080;
	config.shadowsEnabled = true;

	int total = resolutions.size() * numRuns;
	int current = 0;

	for (int res : resolutions) {
		config.shadowMapResolution = res;
		qDebug() << "\nTesting shadow map resolution:" << config.shadowMapResolution << "x" << config.
				shadowMapResolution;

		QVector<BenchmarkResult> runs;

		for (int run = 0; run < numRuns; ++run) {
			current++;
			emit progressUpdate(current,
			                    total,
			                    QString("Shadow Res %1x%1, Run %2/%3").arg(res).arg(run + 1).arg(numRuns));

			qDebug() << "  Run" << (run + 1) << "of" << numRuns;

			BenchmarkResult result = runSingleTest(iterations);
			runs.append(result);
		}

		BenchmarkResult avg = averageResults(runs);
		results.append(avg);
		qDebug() << "  Average:" << avg.toString();
	}

	config = originalConfig;
}

void BenchmarkRunner::testCameraAngle(const QVector<float> &angles, int iterations, int numRuns) {
	qDebug() << "\n=== Stage 4: Testing Camera Orbit Angle (fixed Iterations:" << iterations << ") ===";

	BenchmarkConfig originalConfig = config;
	config.imageWidth = 1920;
	config.imageHeight = 1080;
	config.shadowsEnabled = true;
	config.shadowMapResolution = 2048; // или любое фиксированное значение, например 1024

	int total = angles.size() * numRuns;
	int current = 0;

	for (float yaw : angles) {
		qDebug() << "\nTesting camera yaw angle:" << yaw << "degrees";

		QVector<BenchmarkResult> runs;

		for (int run = 0; run < numRuns; ++run) {
			current++;
			emit progressUpdate(current,
			                    total,
			                    QString("Camera angle %1°, Run %2/%3").arg(yaw).arg(run + 1).arg(numRuns));

			qDebug() << "  Run" << (run + 1) << "of" << numRuns;

			// Создаём камеру и устанавливаем нужный угол
			OrbitCamera camera;
			camera.setTarget(glm::vec3(0.0f, 0.0f, 0.0f)); // фокус на центр сцены
			camera.setPosition(glm::vec3(15.0f, 8.0f, 15.0f)); // начальная позиция

			// Поворачиваем камеру на нужный угол по азимуту (yaw)
			// Сохраняем исходное состояние
			auto [initialPos, initialTarget] = camera.getState();
			// Применяем поворот: orbit(yaw, 0)
			camera.orbit(yaw, 0.0f);

			// Запускаем рендеринг с этой камерой
			auto genStart = std::chrono::high_resolution_clock::now();

			LSystemGenerator lsys;
			lsys.setAxiom(config.axiom);
			lsys.addRule('F', config.rule);
			lsys.setIterations(iterations);
			QString commands = lsys.generate();

			TurtleInterpreter3D turtle;
			turtle.setStepLength(config.stepLength);
			turtle.setAngle(config.angle);
			turtle.setBaseRadius(config.baseRadius);
			turtle.setRadiusDecay(config.radiusDecay);
			turtle.setMinLeafRadius(config.minLeafRadius);
			turtle.setGravityFactor(config.gravityFactor);
			turtle.setRadialSegments(config.radialSegments);
			turtle.setBarkTexture(barkTexture);

			auto treeData = turtle.interpretTree(commands);
			auto genEnd = std::chrono::high_resolution_clock::now();

			BenchmarkResult result;
			result.iterations = iterations;
			result.generationTimeMs = std::chrono::duration_cast<std::chrono::microseconds>(genEnd - genStart).count() /
					1000.0;
			result.triangleCount = treeData.trunk.triangles.size();
			result.leafCount = treeData.leafPositions.size();
			result.width = config.imageWidth;
			result.height = config.imageHeight;
			result.shadows = config.shadowMapResolution;

			// Сцена
			Scene scene;
			scene.addObject(std::make_unique<MeshObject>(std::move(treeData.trunk), &barkTexture));

			LeafGenerator leafGen;
			auto leafMeshData = leafGen.generate(treeData.leafPositions, treeData.leafNormals);
			auto leafObject = std::make_unique<InstancedMeshObject>(
				std::move(leafMeshData.prototype),
				std::move(leafMeshData.instances)
			);
			scene.addObject(std::move(leafObject));

			auto ground = std::make_unique<PlaneObject>(40.0f, 30, &grassTexture);
			ground->setPosition(glm::vec3(0.0f, 0.0f, 0.0f));
			scene.addObject(std::move(ground));

			Light sunLight = Light::createDirectional(glm::normalize(config.lightDirection));
			sunLight.diffuse = glm::vec3(1.0f);
			sunLight.specular = glm::vec3(0.8f);
			sunLight.ambient = glm::vec3(0.3f);
			scene.addLight(sunLight);

			SceneRenderer renderer(config.imageWidth, config.imageHeight);
			renderer.setShadowsEnabled(config.shadowsEnabled);
			renderer.setShadowMapSize(config.shadowMapResolution);
			renderer.setSunLight(sunLight);

			auto renderStart = std::chrono::high_resolution_clock::now();
			QImage frame = renderer.render(scene, camera); // ← передаём настроенную камеру
			auto renderEnd = std::chrono::high_resolution_clock::now();

			result.renderTimeMs = std::chrono::duration_cast<std::chrono::microseconds>(renderEnd - renderStart).count()
					/ 1000.0;
			result.cameraYaw = yaw; // <-- ДОБАВЬТЕ ЭТУ СТРОКУ

			runs.append(result);
		}

		BenchmarkResult avg = averageResults(runs);
		// Дополнительно сохраняем угол (если нужно — можно добавить поле в BenchmarkResult)
		// Пока будем кодировать угол через отрицательные значения итераций или через высоту — но лучше расширить структуру
		results.append(avg);
		qDebug() << "  Average:" << avg.toString();
	}

	config = originalConfig;
}

BenchmarkResult BenchmarkRunner::runSingleTest(int iterations) {
	BenchmarkResult result;
	result.iterations = iterations;

	auto genStart = std::chrono::high_resolution_clock::now();

	LSystemGenerator lsys;
	lsys.setAxiom(config.axiom);
	lsys.addRule('F', config.rule);
	lsys.setIterations(iterations);
	QString commands = lsys.generate();

	TurtleInterpreter3D turtle;
	turtle.setStepLength(config.stepLength);
	turtle.setAngle(config.angle);
	turtle.setBaseRadius(config.baseRadius);
	turtle.setRadiusDecay(config.radiusDecay);
	turtle.setMinLeafRadius(config.minLeafRadius);
	turtle.setGravityFactor(config.gravityFactor);
	turtle.setRadialSegments(config.radialSegments);
	turtle.setBarkTexture(barkTexture);

	auto treeData = turtle.interpretTree(commands);

	auto genEnd = std::chrono::high_resolution_clock::now();

	result.generationTimeMs = std::chrono::duration_cast<std::chrono::microseconds>(genEnd - genStart).count() / 1000.0;
	result.triangleCount = treeData.trunk.triangles.size();
	result.leafCount = treeData.leafPositions.size();

	Scene scene;
	scene.addObject(std::make_unique<MeshObject>(std::move(treeData.trunk), &barkTexture));

	LeafGenerator leafGen;
	auto leafMeshData = leafGen.generate(treeData.leafPositions, treeData.leafNormals);
	auto leafObject = std::make_unique<InstancedMeshObject>(
		std::move(leafMeshData.prototype),
		std::move(leafMeshData.instances)
	);
	scene.addObject(std::move(leafObject));

	auto ground = std::make_unique<PlaneObject>(40.0f, 30, &grassTexture);
	ground->setPosition(glm::vec3(0.0f, 0.0f, 0.0f));
	scene.addObject(std::move(ground));

	Light sunLight = Light::createDirectional(glm::normalize(config.lightDirection));
	sunLight.diffuse = glm::vec3(1.0f);
	sunLight.specular = glm::vec3(0.8f);
	sunLight.ambient = glm::vec3(0.3f);
	scene.addLight(sunLight);

	SceneRenderer renderer(config.imageWidth, config.imageHeight);
	renderer.setShadowsEnabled(config.shadowsEnabled);
	renderer.setShadowMapSize(config.shadowMapResolution);
	renderer.setSunLight(sunLight);

	CameraManager camManager;
	camManager.switchToOrbit();
	auto &camera = camManager.getActiveCamera();

	auto renderStart = std::chrono::high_resolution_clock::now();
	QImage frame = renderer.render(scene, camera);
	auto renderEnd = std::chrono::high_resolution_clock::now();

	// Время полного рендеринга кадра
	result.renderTimeMs = std::chrono::duration_cast<std::chrono::microseconds>(renderEnd - renderStart).count() /
			1000.0;

	return result;
}

BenchmarkResult BenchmarkRunner::averageResults(const QVector<BenchmarkResult> &runs) {
	if (runs.isEmpty()) {
		return BenchmarkResult();
	}

	BenchmarkResult avg = runs[0];
	avg.generationTimeMs = 0;
	avg.renderTimeMs = 0;

	for (const auto &run : runs) {
		avg.generationTimeMs += run.generationTimeMs;
		avg.renderTimeMs += run.renderTimeMs;
	}

	avg.generationTimeMs /= runs.size();
	avg.renderTimeMs /= runs.size();

	avg.width = config.imageWidth;
	avg.height = config.imageHeight;
	avg.shadows = config.shadowMapResolution;
	avg.cameraYaw = runs[0].cameraYaw;

	return avg;
}

void BenchmarkRunner::saveResults(const QString &filename) const {
	QFile file(filename);
	if (!file.open(QIODevice::WriteOnly | QIODevice::Text)) {
		qWarning() << "Cannot open file for writing:" << filename;
		return;
	}

	QTextStream out(&file);

	out << "Iterations,GenerationTime(ms),RenderTime(ms),Triangles,Leaves,wigth,height,shadowmap\n";

	for (const auto &result : results) {
		out << result.toCSVRow() << "\n";
	}

	file.close();
	qDebug() << "Benchmark results saved to:" << filename;
}

double BenchmarkRunner::measureTime(std::function<void()> func) {
	auto start = std::chrono::high_resolution_clock::now();
	func();
	auto end = std::chrono::high_resolution_clock::now();
	return std::chrono::duration_cast<std::chrono::microseconds>(end - start).count() / 1000.0;
}
