#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QGraphicsPixmapItem>
#include <QDebug>
#include <QMouseEvent>
#include <QHeaderView>
#include <QMessageBox>
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
	  , scene2D(std::make_unique<QGraphicsScene>(this))
	  , rulesModel(new QStandardItemModel(this)) {
	ui->setupUi(this);
	setupUi();
	setupRulesTable();
	populateSymbolsComboBox();
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

void MainWindow::setupRulesTable() {
	rulesModel->setHorizontalHeaderLabels({"Символ", "Правило"});
	ui->rules_table->setModel(rulesModel);

	// Настройка внешнего вида таблицы
	ui->rules_table->horizontalHeader()->setStretchLastSection(true);
	ui->rules_table->setSelectionBehavior(QAbstractItemView::SelectRows);
	ui->rules_table->setSelectionMode(QAbstractItemView::SingleSelection);
	ui->rules_table->setEditTriggers(QAbstractItemView::DoubleClicked | QAbstractItemView::EditKeyPressed);

	// Установка ширины колонок
	ui->rules_table->setColumnWidth(0, 80);

	// Контекстное меню для удаления правил
	ui->rules_table->setContextMenuPolicy(Qt::CustomContextMenu);
	connect(ui->rules_table, &QTableView::customContextMenuRequested,
			this, &MainWindow::onDeleteRule);
}

void MainWindow::populateSymbolsComboBox() {
	ui->symbols_combo_box->clear();

	// Добавляем буквы латинского алфавита (заглавные)
	for (char c = 'A'; c <= 'Z'; ++c) {
		ui->symbols_combo_box->addItem(QString(c));
	}
}

void MainWindow::clearRulesTable() {
	rulesModel->removeRows(0, rulesModel->rowCount());
}

void MainWindow::addRuleToTable(QChar symbol, const QString& rule) {
	QList<QStandardItem*> items;

	QStandardItem* symbolItem = new QStandardItem(symbol);
	symbolItem->setEditable(false); // Символ не редактируется

	QStandardItem* ruleItem = new QStandardItem(rule);

	items << symbolItem << ruleItem;
	rulesModel->appendRow(items);
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
	ui->trees_combo_box->addItem("Дерево 9 (Сложное)");
	ui->trees_combo_box->addItem("Дерево 10 (Кустарник)");
	ui->trees_combo_box->addItem("Дерево 11 (Пышное)");
	ui->trees_combo_box->addItem("Дерево 12 (3D Ветвистое)");
	ui->trees_combo_box->addItem("Дерево 13 (3D Ветвистое)");
	ui->trees_combo_box->addItem("Дерево 14 (Реалистичная Ель)");
	ui->trees_combo_box->addItem("Дерево 15 (Дуб)");
	ui->trees_combo_box->addItem("Дерево 16 (Плакучая Ива)");

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

	// Новые подключения
	connect(ui->add_rule_btn, &QPushButton::clicked, this, &MainWindow::onAddRuleClicked);
}

void MainWindow::loadPreset(const QString &name) {
	clearRulesTable();

	if (name == "Дерево 1") {
		ui->axiome_edit->setText("F");
		addRuleToTable('F', "F[+FL]F[-FL]FL");
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
		addRuleToTable('F', "F[&&+F][&&-F][&+F][&-F]");
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
		addRuleToTable('F', "F[&+FL][&-FL][^+FL][^-FL]");
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
		addRuleToTable('F', "F[&&&+FL][&&&-FL]");
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
		addRuleToTable('F', "F[F[&+FL][&-FL][^+FL][^-FL]]");
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
		addRuleToTable('F', "F[+FL][-FL][&FL][^FL]");
		ui->iterations_spin_box->setValue(2);
		ui->angle_spin_box->setValue(40.0);
		ui->step_spin_box->setValue(1.25);

		ui->base_radius_spin_box->setValue(0.25);
		ui->radius_decay_spin_box->setValue(0.52);
		ui->min_leaf_radius_spin_box->setValue(0.025);
		ui->gravity_factor_spin_box->setValue(0.025);
		ui->radial_segments_spin_box->setValue(12);
	} else if (name == "Дерево 7") {
		ui->axiome_edit->setText("F");
		addRuleToTable('F', "F[+F][-F][&F][^F]");
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
		addRuleToTable('F', "F[^+FL]&[FL]^[-FL]");
		ui->iterations_spin_box->setValue(4);
		ui->angle_spin_box->setValue(30.0);
		ui->step_spin_box->setValue(1.8);

		ui->base_radius_spin_box->setValue(0.28);
		ui->radius_decay_spin_box->setValue(0.55);
		ui->min_leaf_radius_spin_box->setValue(0.013);
		ui->gravity_factor_spin_box->setValue(0.00);
		ui->radial_segments_spin_box->setValue(6);
	} else if (name == "Дерево 9 (Сложное)") {
		// Сложное дерево с разветвлением через два символа
		addRuleToTable('A', "F[&+B][&-B][^+B][^-B]FA");
		addRuleToTable('B', "F[+C][-C][&C][^C]FB");
		addRuleToTable('C', "F[+L][-L][&L][^L]");
		addRuleToTable('F', "F");
		ui->axiome_edit->setText("A");
		ui->iterations_spin_box->setValue(5);
		ui->angle_spin_box->setValue(24.0);
		ui->step_spin_box->setValue(1.2);

		ui->base_radius_spin_box->setValue(0.28);
		ui->radius_decay_spin_box->setValue(0.65);
		ui->min_leaf_radius_spin_box->setValue(0.015);
		ui->gravity_factor_spin_box->setValue(0.015);
		ui->radial_segments_spin_box->setValue(8);
	} else if (name == "Дерево 10 (Кустарник)") {
		// Кустарниковая форма с множественными правилами
		ui->axiome_edit->setText("FX");
		addRuleToTable('F', "FF");
		addRuleToTable('X', "F[+XL]F[-XL][&XL][^XL]");
		ui->iterations_spin_box->setValue(4);
		ui->angle_spin_box->setValue(35.0);
		ui->step_spin_box->setValue(0.75);

		ui->base_radius_spin_box->setValue(0.2);
		ui->radius_decay_spin_box->setValue(0.55);
		ui->min_leaf_radius_spin_box->setValue(0.02);
		ui->gravity_factor_spin_box->setValue(0.008);
		ui->radial_segments_spin_box->setValue(6);
	} else if (name == "Дерево 11 (Пышное)") {
		// Пышное дерево с тремя типами ветвей
		ui->axiome_edit->setText("A");
		addRuleToTable('A', "F[&B][^C]A");
		addRuleToTable('B', "F[+BL][-BL]");
		addRuleToTable('C', "F[&CL][^CL]");
		ui->iterations_spin_box->setValue(3);
		ui->angle_spin_box->setValue(28.0);
		ui->step_spin_box->setValue(1.0);

		ui->base_radius_spin_box->setValue(0.3);
		ui->radius_decay_spin_box->setValue(0.6);
		ui->min_leaf_radius_spin_box->setValue(0.018);
		ui->gravity_factor_spin_box->setValue(0.02);
		ui->radial_segments_spin_box->setValue(10);
	} else if (name == "Дерево 12 (3D Ветвистое)") {
		// Трёхмерная версия из скриншота с полным 3D ветвлением
		ui->axiome_edit->setText("FFX");
		addRuleToTable('F', "F");
		addRuleToTable('X', "F[-F+F[Y]-[X]]+[+F+F[X]-[Y]]");
		addRuleToTable('Y', "F[-F+F][+F+FY]");
		ui->iterations_spin_box->setValue(4);
		ui->angle_spin_box->setValue(15.0);
		ui->step_spin_box->setValue(1.0);

		ui->base_radius_spin_box->setValue(0.25);
		ui->radius_decay_spin_box->setValue(0.6);
		ui->min_leaf_radius_spin_box->setValue(0.015);
		ui->gravity_factor_spin_box->setValue(0.01);
		ui->radial_segments_spin_box->setValue(8);
	} else if (name == "Дерево 13 (3D Ветвистое)") {
		// Профессиональное 3D дерево с полным объёмным ветвлением
		ui->axiome_edit->setText("FX");
		addRuleToTable('F', "FF");
		addRuleToTable('X', "F[&+XL][&-XL][^+YL][^-YL]");
		addRuleToTable('Y', "F[+YL][-YL]");
		ui->iterations_spin_box->setValue(4);
		ui->angle_spin_box->setValue(22.0);
		ui->step_spin_box->setValue(0.9);

		ui->base_radius_spin_box->setValue(0.28);
		ui->radius_decay_spin_box->setValue(0.58);
		ui->min_leaf_radius_spin_box->setValue(0.018);
		ui->gravity_factor_spin_box->setValue(0.015);
		ui->radial_segments_spin_box->setValue(10);
	} else if (name == "Дерево 14 (Реалистичная Ель)") {
		// Коническая форма ели с ярусным ветвлением
		ui->axiome_edit->setText("A");
		addRuleToTable('A', "F[&&&B][&&&C][&&&D]A");
		addRuleToTable('B', "F[++BL][--BL]");
		addRuleToTable('C', "F[+CL][-CL]");
		addRuleToTable('D', "F[^^DL]");
		ui->iterations_spin_box->setValue(5);
		ui->angle_spin_box->setValue(18.0);
		ui->step_spin_box->setValue(0.7);

		ui->base_radius_spin_box->setValue(0.22);
		ui->radius_decay_spin_box->setValue(0.55);
		ui->min_leaf_radius_spin_box->setValue(0.012);
		ui->gravity_factor_spin_box->setValue(0.005);
		ui->radial_segments_spin_box->setValue(8);
	} else if (name == "Дерево 15 (Дуб)") {
		// Массивный дуб с широкой кроной
		ui->axiome_edit->setText("FFA");
		addRuleToTable('F', "FF");
		addRuleToTable('A', "F[&+B][&-C][^+D][^-E]A");
		addRuleToTable('B', "F[++BL][&BL]");
		addRuleToTable('C', "F[--CL][&CL]");
		addRuleToTable('D', "F[^DL][+DL]");
		addRuleToTable('E', "F[^EL][-EL]");
		ui->iterations_spin_box->setValue(3);
		ui->angle_spin_box->setValue(25.0);
		ui->step_spin_box->setValue(1.2);

		ui->base_radius_spin_box->setValue(0.35);
		ui->radius_decay_spin_box->setValue(0.62);
		ui->min_leaf_radius_spin_box->setValue(0.022);
		ui->gravity_factor_spin_box->setValue(0.028);
		ui->radial_segments_spin_box->setValue(12);
	} else if (name == "Дерево 16 (Плакучая Ива)") {
		// Плакучая ива с изящными свисающими ветвями
		ui->axiome_edit->setText("FA");
		addRuleToTable('F', "FF");
		addRuleToTable('A', "F[&+XL][&-YL][&ZL]A");
		addRuleToTable('X', "F[&+XL]");
		addRuleToTable('Y', "F[&-YL]");
		addRuleToTable('Z', "F[&ZL]");
		ui->iterations_spin_box->setValue(5);
		ui->angle_spin_box->setValue(12.0);
		ui->step_spin_box->setValue(0.85);

		ui->base_radius_spin_box->setValue(0.26);
		ui->radius_decay_spin_box->setValue(0.72);
		ui->min_leaf_radius_spin_box->setValue(0.008);
		ui->gravity_factor_spin_box->setValue(0.045);
		ui->radial_segments_spin_box->setValue(8);
	}
}

void MainWindow::buildAndRenderTree() {
	try {
		QString axiom = ui->axiome_edit->text();
		int iterations = ui->iterations_spin_box->value();
		float angle = ui->angle_spin_box->value();
		float step = ui->step_spin_box->value();

		float baseRadius = ui->base_radius_spin_box->value();
		float radiusDecay = ui->radius_decay_spin_box->value();
		float minLeafRadius = ui->min_leaf_radius_spin_box->value();
		float gravityFactor = ui->gravity_factor_spin_box->value();
		int radialSegments = ui->radial_segments_spin_box->value();

		// Создание L-системы и добавление всех правил из таблицы
		LSystemGenerator lsys;
		lsys.setAxiom(axiom);

		for (int row = 0; row < rulesModel->rowCount(); ++row) {
			QString symbol = rulesModel->item(row, 0)->text();
			QString rule = rulesModel->item(row, 1)->text();

			if (!symbol.isEmpty() && !rule.isEmpty()) {
				lsys.addRule(symbol[0].toLatin1(), rule);
			}
		}

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
		QMessageBox::critical(this, "Ошибка", QString("Не удалось построить дерево: %1").arg(e.what()));
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

void MainWindow::onAddRuleClicked() {
	QString symbol = ui->symbols_combo_box->currentText();
	QString rule = ui->rule_edit->text();

	if (symbol.isEmpty()) {
		QMessageBox::warning(this, "Предупреждение", "Выберите символ из списка!");
		return;
	}

	if (rule.isEmpty()) {
		QMessageBox::warning(this, "Предупреждение", "Введите правило!");
		return;
	}

	// Проверяем, есть ли уже правило для этого символа
	for (int row = 0; row < rulesModel->rowCount(); ++row) {
		if (rulesModel->item(row, 0)->text() == symbol) {
			auto reply = QMessageBox::question(this, "Подтверждение",
				QString("Правило для символа '%1' уже существует. Заменить?").arg(symbol),
				QMessageBox::Yes | QMessageBox::No);

			if (reply == QMessageBox::Yes) {
				rulesModel->item(row, 1)->setText(rule);
				ui->rule_edit->clear();
				return;
			} else {
				return;
			}
		}
	}

	// Добавляем новое правило
	addRuleToTable(symbol[0], rule);
	ui->rule_edit->clear();
}

void MainWindow::onDeleteRule() {
	QModelIndex currentIndex = ui->rules_table->currentIndex();

	if (!currentIndex.isValid()) {
		QMessageBox::information(this, "Информация", "Выберите правило для удаления!");
		return;
	}

	int row = currentIndex.row();
	QString symbol = rulesModel->item(row, 0)->text();

	auto reply = QMessageBox::question(this, "Подтверждение",
		QString("Удалить правило для символа '%1'?").arg(symbol),
		QMessageBox::Yes | QMessageBox::No);

	if (reply == QMessageBox::Yes) {
		rulesModel->removeRow(row);
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