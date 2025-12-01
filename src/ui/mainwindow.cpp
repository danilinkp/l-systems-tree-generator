#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QGraphicsPixmapItem>
#include <QDebug>
#include <QMouseEvent>
#include <glm/gtx/quaternion.hpp>

#include "free_camera.h"
#include "mesh_object.h"
#include "instanced_mesh_object.h"
#include "orbit_camera.h"
#include "plane_object.h"
#include "texture_loader.h"

MainWindow::MainWindow(QWidget *parent)
	: QMainWindow(parent)
	  , ui(new Ui::MainWindow)
	  , scene2D(std::make_unique<QGraphicsScene>(this)) {
	ui->setupUi(this);
	setupUi();
	connectSignals();

	QTimer::singleShot(0, this, &MainWindow::initializeRenderer);

	ui->trees_combo_box->setCurrentIndex(5);
	loadPreset("Дерево 6");
	buildAndRenderTree();
}

MainWindow::~MainWindow() {
	delete ui;
}

void MainWindow::initializeRenderer() {
	int width = ui->graphicsView->width();
	int height = ui->graphicsView->height();

	if (width > 0 && height > 0) {
		renderer = std::make_unique<SceneRenderer>(width, height);
		updateLight();
		buildAndRenderTree();
	}
}

void MainWindow::setupUi() {
	ui->graphicsView->setScene(scene2D.get());
	ui->graphicsView->setRenderHint(QPainter::Antialiasing);
	ui->graphicsView->setViewportUpdateMode(QGraphicsView::FullViewportUpdate);

	// L-система
	ui->base_radius_spin_box->setRange(0.01, 2.0);
	ui->base_radius_spin_box->setSingleStep(0.05);
	ui->base_radius_spin_box->setDecimals(2);
	ui->base_radius_spin_box->setValue(0.3);

	ui->radius_decay_spin_box->setRange(0.1, 0.99);
	ui->radius_decay_spin_box->setSingleStep(0.05);
	ui->radius_decay_spin_box->setDecimals(2);
	ui->radius_decay_spin_box->setValue(0.7);

	ui->min_leaf_radius_spin_box->setRange(0.001, 0.1);
	ui->min_leaf_radius_spin_box->setSingleStep(0.005);
	ui->min_leaf_radius_spin_box->setDecimals(3);
	ui->min_leaf_radius_spin_box->setValue(0.01);

	ui->gravity_factor_spin_box->setRange(0.0, 0.5);
	ui->gravity_factor_spin_box->setSingleStep(0.01);
	ui->gravity_factor_spin_box->setDecimals(3);
	ui->gravity_factor_spin_box->setValue(0.01);

	ui->radial_segments_spin_box->setRange(3, 20);
	ui->radial_segments_spin_box->setSingleStep(1);
	ui->radial_segments_spin_box->setValue(5);

	ui->intensity_spin_box->setRange(0.0, 5.0);
	ui->intensity_spin_box->setSingleStep(0.1);
	ui->intensity_spin_box->setDecimals(2);
	ui->intensity_spin_box->setValue(1.0);

	ui->x_light_spin_box->setRange(-10.0, 10.0);
	ui->x_light_spin_box->setSingleStep(0.1);
	ui->x_light_spin_box->setDecimals(2);
	ui->x_light_spin_box->setValue(1.0);

	ui->y_light_spin_box->setRange(-10.0, 10.0);
	ui->y_light_spin_box->setSingleStep(0.1);
	ui->y_light_spin_box->setDecimals(2);
	ui->y_light_spin_box->setValue(-2.0);

	ui->z_light_spin_box->setRange(-10.0, 10.0);
	ui->z_light_spin_box->setSingleStep(0.1);
	ui->z_light_spin_box->setDecimals(2);
	ui->z_light_spin_box->setValue(1.0);

	ui->enable_shadows_check_box->setChecked(true);

	ui->trees_combo_box->addItem("Дерево 1");
	ui->trees_combo_box->addItem("Дерево 2");
	ui->trees_combo_box->addItem("Дерево 3");
	ui->trees_combo_box->addItem("Дерево 4");
	ui->trees_combo_box->addItem("Дерево 5");
	ui->trees_combo_box->addItem("Дерево 6");
	ui->trees_combo_box->addItem("Дерево 7");
	ui->trees_combo_box->addItem("Дерево 8");

	ui->orbit_camera_radio_btn->setChecked(true);

	setFocusPolicy(Qt::StrongFocus);
	setFocus();
}

void MainWindow::connectSignals() {
	connect(ui->pushButton, &QPushButton::clicked, this, &MainWindow::onGenerateTree);
	connect(ui->trees_combo_box, &QComboBox::currentTextChanged, this, &MainWindow::onPresetChanged);
	connect(ui->orbit_camera_radio_btn, &QRadioButton::toggled, this, &MainWindow::onOrbitCameraToggled);
	connect(ui->free_camera_radio_btn, &QRadioButton::toggled, this, &MainWindow::onFreeCameraToggled);

	connect(ui->enable_shadows_check_box, &QCheckBox::toggled, this, &MainWindow::onShadowsToggled);

	connect(ui->light_push_button, &QPushButton::clicked, this, &MainWindow::updateLight);
}

void MainWindow::loadPreset(const QString &name) {
	if (name == "Дерево 1") {
		ui->axiome_edit->setText("F");
		ui->rule_edit->setText("F[+FL]F[-FL]FL");
		ui->iterations_spin_box->setValue(3);
		ui->angle_spin_box->setValue(25.0);
		ui->step_spin_box->setValue(1.0);

		ui->base_radius_spin_box->setValue(0.3);
		ui->radius_decay_spin_box->setValue(0.7);
		ui->min_leaf_radius_spin_box->setValue(0.01);
		ui->gravity_factor_spin_box->setValue(0.02);
		ui->radial_segments_spin_box->setValue(6);
	} else if (name == "Дерево 2") {
		ui->axiome_edit->setText("F");
		ui->rule_edit->setText("F[&&+F][&&-F][&+F][&-F]");
		ui->iterations_spin_box->setValue(3);
		ui->angle_spin_box->setValue(22.0);
		ui->step_spin_box->setValue(0.9);

		ui->base_radius_spin_box->setValue(0.25);
		ui->radius_decay_spin_box->setValue(0.75);
		ui->min_leaf_radius_spin_box->setValue(0.008);
		ui->gravity_factor_spin_box->setValue(0.005);
		ui->radial_segments_spin_box->setValue(8);
	} else if (name == "Дерево 3") {
		ui->axiome_edit->setText("F");
		ui->rule_edit->setText("F[&+FL][&-FL][^+FL][^-FL]");
		ui->iterations_spin_box->setValue(4);
		ui->angle_spin_box->setValue(28.0);
		ui->step_spin_box->setValue(1.10);

		ui->base_radius_spin_box->setValue(0.3);
		ui->radius_decay_spin_box->setValue(0.49);
		ui->min_leaf_radius_spin_box->setValue(0.035);
		ui->gravity_factor_spin_box->setValue(0.01);
		ui->radial_segments_spin_box->setValue(12);
	} else if (name == "Дерево 4") {
		ui->axiome_edit->setText("F");
		ui->rule_edit->setText("F[&&&+FL][&&&-FL]");
		ui->iterations_spin_box->setValue(3);
		ui->angle_spin_box->setValue(20.0);
		ui->step_spin_box->setValue(0.8);

		ui->base_radius_spin_box->setValue(0.4);
		ui->radius_decay_spin_box->setValue(0.65);
		ui->min_leaf_radius_spin_box->setValue(0.015);
		ui->gravity_factor_spin_box->setValue(0.08);
		ui->radial_segments_spin_box->setValue(5);
	} else if (name == "Дерево 5") {
		ui->axiome_edit->setText("F");
		ui->rule_edit->setText("F[F[&+FL][&-FL][^+FL][^-FL]]");
		ui->iterations_spin_box->setValue(2);
		ui->angle_spin_box->setValue(30.0);
		ui->step_spin_box->setValue(1.2);

		ui->base_radius_spin_box->setValue(0.2);
		ui->radius_decay_spin_box->setValue(0.85);
		ui->min_leaf_radius_spin_box->setValue(0.02);
		ui->gravity_factor_spin_box->setValue(0.03);
		ui->radial_segments_spin_box->setValue(8);
	} else if (name == "Дерево 6") {
		ui->axiome_edit->setText("F");
		ui->rule_edit->setText("F[+FL][-FL][&FL][^FL]");
		ui->iterations_spin_box->setValue(4);
		ui->angle_spin_box->setValue(40.0);
		ui->step_spin_box->setValue(1.25);

		ui->base_radius_spin_box->setValue(0.25);
		ui->radius_decay_spin_box->setValue(0.52);
		ui->min_leaf_radius_spin_box->setValue(0.025);
		ui->gravity_factor_spin_box->setValue(0.025);
		ui->radial_segments_spin_box->setValue(12);
	} else if (name == "Дерево 7") {
		ui->axiome_edit->setText("F");
		ui->rule_edit->setText("F[+F][-F][&F][^F]");
		ui->iterations_spin_box->setValue(3);
		ui->angle_spin_box->setValue(24.0);
		ui->step_spin_box->setValue(0.95);

		ui->base_radius_spin_box->setValue(0.22);
		ui->radius_decay_spin_box->setValue(0.78);
		ui->min_leaf_radius_spin_box->setValue(0.007);
		ui->gravity_factor_spin_box->setValue(0.01);
		ui->radial_segments_spin_box->setValue(6);
	} else if (name == "Дерево 8") {
		ui->axiome_edit->setText("F");
		ui->rule_edit->setText("F[+FL][FL][-FL]");
		ui->iterations_spin_box->setValue(3);
		ui->angle_spin_box->setValue(26.0);
		ui->step_spin_box->setValue(0.88);

		ui->base_radius_spin_box->setValue(0.28);
		ui->radius_decay_spin_box->setValue(0.7);
		ui->min_leaf_radius_spin_box->setValue(0.013);
		ui->gravity_factor_spin_box->setValue(0.018);
		ui->radial_segments_spin_box->setValue(6);
	}
}

void MainWindow::buildAndRenderTree() {
	try {
		QString axiom = ui->axiome_edit->text();
		QString rule = ui->rule_edit->text();
		int iterations = ui->iterations_spin_box->value();
		float angle = ui->angle_spin_box->value();
		float step = ui->step_spin_box->value();

		float baseRadius = ui->base_radius_spin_box->value();
		float radiusDecay = ui->radius_decay_spin_box->value();
		float minLeafRadius = ui->min_leaf_radius_spin_box->value();
		float gravityFactor = ui->gravity_factor_spin_box->value();
		int radialSegments = ui->radial_segments_spin_box->value();

		LSystemGenerator lsys;
		lsys.setAxiom(axiom);
		lsys.addRule('F', rule);
		lsys.setIterations(iterations);
		QString commands = lsys.generate();

		static QImage barkTex = TextureLoader::loadTexture("../textures/bark_2.jpg");
		static QImage grassTex = TextureLoader::loadTexture("../textures/grass_2.jpg");

		TurtleInterpreter3D turtle;
		turtle.setStepLength(step);
		turtle.setAngle(angle);
		turtle.setBaseRadius(baseRadius);
		turtle.setRadiusDecay(radiusDecay);
		turtle.setMinLeafRadius(minLeafRadius);
		turtle.setGravityFactor(gravityFactor);
		turtle.setRadialSegments(radialSegments);
		turtle.setBarkTexture(barkTex);

		auto treeData = turtle.interpretTree(commands);

		LeafGenerator leafGen;
		auto leafMeshData = leafGen.generate(treeData.leafPositions, treeData.leafNormals);

		scene3D.clear();

		scene3D.addObject(std::make_unique<MeshObject>(std::move(treeData.trunk), &barkTex));

		auto leafObject = std::make_unique<InstancedMeshObject>(
			std::move(leafMeshData.prototype),
			std::move(leafMeshData.instances)
		);

		Lighting::Material leafMaterial;
		leafMaterial.ambient = glm::vec3(0.3f);
		leafMaterial.diffuse = glm::vec3(0.9f);
		leafMaterial.specular = glm::vec3(0.05f);
		leafMaterial.shininess = 3.0f;

		leafObject->setMaterial(leafMaterial);
		scene3D.addObject(std::move(leafObject));

		auto ground = std::make_unique<PlaneObject>(40.0f, 30, &grassTex);
		ground->setPosition(glm::vec3(0.0f, 0.0f, 0.0f));
		scene3D.addObject(std::move(ground));

		updateLight();

		if (auto *orbitCam = dynamic_cast<OrbitCamera *>(&cameraManager.getActiveCamera())) {
			glm::vec3 center = SceneRenderer::computeSceneCenter(scene3D);
			orbitCam->setTarget(center);
		}

		render();
	} catch (const std::exception &e) {
		qDebug() << "ERROR:" << e.what();
	}
}

void MainWindow::updateLight() {
	if (!renderer) return;

	glm::vec3 direction(
		ui->x_light_spin_box->value(),
		ui->y_light_spin_box->value(),
		ui->z_light_spin_box->value()
	);

	float intensity = ui->intensity_spin_box->value();

	float length = glm::length(direction);
	if (length < 0.001f) {
		qDebug() << "WARNING: Light direction is zero! Setting default.";
		direction = glm::vec3(1.0f, -4.0f, 1.0f);
	}

	direction = glm::normalize(direction);

	Light sunLight = Light::createDirectional(direction);
	sunLight.diffuse = glm::vec3(intensity);
	sunLight.specular = glm::vec3(intensity * 0.8f);
	sunLight.ambient = glm::vec3(0.3f);

	scene3D.clearLights();
	scene3D.addLight(sunLight);

	renderer->setSunLight(sunLight);

	render();
}

void MainWindow::render() {
	if (!renderer)
		return;

	qDebug() << "Camera position: " << cameraManager.getActiveCamera().getPosition().x << " " << cameraManager.
			getActiveCamera().getPosition().y << " " << cameraManager.getActiveCamera().getPosition().z;

	QImage result = renderer->render(scene3D, cameraManager.getActiveCamera());
	updateImage(result);
}

void MainWindow::updateImage(const QImage &frame) {
	if (!renderer) return;

	if (frame.isNull()) return;

	QPixmap pixmap = QPixmap::fromImage(frame);
	if (pixmapItem)
		pixmapItem->setPixmap(pixmap);
	else {
		scene2D->clear();
		pixmapItem = scene2D->addPixmap(pixmap);
	}
	scene2D->setSceneRect(pixmap.rect());
	ui->graphicsView->fitInView(scene2D->sceneRect(), Qt::KeepAspectRatio);
}


void MainWindow::onGenerateTree() {
	buildAndRenderTree();
}

void MainWindow::onPresetChanged(const QString &name) {
	loadPreset(name);
	buildAndRenderTree();
}

void MainWindow::onOrbitCameraToggled(bool checked) {
	if (checked) {
		cameraManager.switchToOrbit();
		render();
	}
}

void MainWindow::onFreeCameraToggled(bool checked) {
	if (checked) {
		cameraManager.switchToFree();
		render();
	}
}

void MainWindow::onShadowsToggled(bool checked) {
	if (renderer) {
		renderer->setShadowsEnabled(checked);
		render();
	}
}


void MainWindow::keyPressEvent(QKeyEvent *event) {
	Camera &cam = cameraManager.getActiveCamera();

	if (auto *orbit = dynamic_cast<OrbitCamera *>(&cam)) {
		switch (event->key()) {
		case Qt::Key_W: orbit->orbit(0, -0.1f);
			break;
		case Qt::Key_S: orbit->orbit(0, 0.1f);
			break;
		case Qt::Key_A: orbit->orbit(0.1f, 0);
			break;
		case Qt::Key_D: orbit->orbit(-0.1f, 0);
			break;
		case Qt::Key_Q: orbit->zoom(0.5f);
			break;
		case Qt::Key_E: orbit->zoom(-0.5f);
			break;
		case Qt::Key_1: orbit->setViewFront();
			break;
		case Qt::Key_2: orbit->setViewSide();
			break;
		case Qt::Key_3: orbit->setViewTop();
			break;
		default: return;
		}
		render();
	} else if (auto *free = dynamic_cast<FreeCamera *>(&cam)) {
		float dt = 0.016f;
		float amount = 2.0f * dt;
		switch (event->key()) {
		case Qt::Key_W: free->moveForward(amount);
			break;
		case Qt::Key_S: free->moveForward(-amount);
			break;
		case Qt::Key_A: free->moveRight(-amount);
			break;
		case Qt::Key_D: free->moveRight(amount);
			break;
		case Qt::Key_Q: free->moveUp(-amount);
			break;
		case Qt::Key_E: free->moveUp(amount);
			break;
		default: return;
		}
		render();
	}

	QMainWindow::keyPressEvent(event);
}

void MainWindow::mousePressEvent(QMouseEvent *event) {
	if (event->button() == Qt::LeftButton) {
		isDragging = true;
		lastMousePos = event->pos();
	}
	QMainWindow::mousePressEvent(event);
}

void MainWindow::mouseReleaseEvent(QMouseEvent *event) {
	if (event->button() == Qt::LeftButton) {
		isDragging = false;
	}
	QMainWindow::mouseReleaseEvent(event);
}

void MainWindow::mouseMoveEvent(QMouseEvent *event) {
	if (isDragging) {
		QPoint delta = event->pos() - lastMousePos;
		lastMousePos = event->pos();

		Camera &cam = cameraManager.getActiveCamera();
		if (auto *orbit = dynamic_cast<OrbitCamera *>(&cam)) {
			orbit->orbit(-delta.x() * 0.01f, -delta.y() * 0.01f);
			render();
		}
	}
	QMainWindow::mouseMoveEvent(event);
}

void MainWindow::wheelEvent(QWheelEvent *event) {
	float delta = event->angleDelta().y() < 0 ? -1.0f : 1.0f;
	Camera &cam = cameraManager.getActiveCamera();
	if (auto *orbit = dynamic_cast<OrbitCamera *>(&cam)) {
		orbit->zoom(delta);
		render();
	}
	QMainWindow::wheelEvent(event);
}

void MainWindow::resizeEvent(QResizeEvent *event) {
	QMainWindow::resizeEvent(event);
	int width = ui->graphicsView->width();
	int height = ui->graphicsView->height();
	if (width > 0 && height > 0 && renderer) {
		renderer->resize(width, height);
		render();
	}
}
