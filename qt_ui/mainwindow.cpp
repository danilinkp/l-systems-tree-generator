#include <QFileDialog>
#include <QMessageBox>
#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "create_drawer_factory.h"
#include "qt_drawer_factory.h"
#include "manager_solution.h"
#include "object_command.h"
#include "scene_command.h"
#include "camera_command.h"
#include "history_command.h"
#include "draw_manager.h"
#include "load_manager.h"
#include "scene_manager.h"
#include "transform_manager.h"


MainWindow::MainWindow(QWidget *parent)
	: QMainWindow(parent), ui(new Ui::MainWindow)
{
	ui->setupUi(this);
	setMinimumWidth(1800);

	facade = std::make_shared<Facade>();

	setupScene(this);

	connect(ui->cameras_combo_box, &QComboBox::currentIndexChanged, this, &MainWindow::choosedCamera);
	connect(ui->models_combo_box, &QComboBox::currentIndexChanged, this, &MainWindow::choosedModel);

	on_add_camera_btn_clicked();
}

MainWindow::~MainWindow()
{
	delete ui;
}

void MainWindow::renderScene()
{
	auto drawManager = ManagerSolution::getManager<DrawManager>();
	auto command = std::make_shared<RenderSceneCommand>(drawManager);
	facade->execute(command);
}

void MainWindow::setupScene(QWidget *parent)
{
	int screenWidth = ui->graphicsView->width();
	int screenHeight = ui->graphicsView->height();
	scene = std::make_shared<QGraphicsScene>(0, 0, screenWidth, screenHeight, parent);

	scene->addRect(scene->sceneRect());

	ui->graphicsView->setScene(scene.get());
	ui->graphicsView->setStyleSheet("background-color: white");

	std::shared_ptr<BaseDrawer> drawer = CreateDrawerFactory::create<QtDrawerFactory>(scene);

	auto drawManager = ManagerSolution::getManager<DrawManager>();

	drawManager->setDrawer(drawer);
	drawManager->drawScene();
}

void MainWindow::showError(const std::string &message)
{
	QMessageBox::warning(this, tr("Error"), tr(message.c_str()));
}

void MainWindow::on_load_model_btn_clicked()
{
	auto path = QFileDialog::getOpenFileName(this, tr("Open File"), "", tr("Model Files (*.txt *.csv)"));

	int objectId = -1;

	try
	{
		auto loadManager = ManagerSolution::getManager<LoadManager>();
		auto sceneManager = ManagerSolution::getManager<SceneManager>();

		auto command = std::make_shared<LoadModelCommand>(loadManager, sceneManager, path.toStdString());
		facade->execute(command);
		objectId = command->getObjectId();
		models.push_back(objectId);
	}
	catch (std::exception &e)
	{
		showError(e.what());
	}

	auto fileName = QFileInfo(path.toUtf8().data()).fileName();
	auto figName = QString("Model (") + QString::number(objectId) + QString(") load from ") + QString(fileName);
	ui->models_combo_box->addItem(figName, QVariant(objectId));


	auto drawManager = ManagerSolution::getManager<DrawManager>();
	auto command = std::make_shared<RenderSceneCommand>(drawManager);
	facade->execute(command);
}

void MainWindow::on_add_camera_btn_clicked()
{
	auto cameraManager = ManagerSolution::getManager<CameraManager>();
	auto command = std::make_shared<AddCameraCommand>(cameraManager, Point(0, 0, 0));
	facade->execute(command);
	int cameraId = command->getCameraId();

	std::cout << "cameraId: " << cameraId << std::endl;

	QString cameraName = QString("Camera ") + QString::number(cameraId);

	ui->cameras_combo_box->addItem(cameraName, QVariant(cameraId));
	ui->cameras_combo_box->setCurrentIndex(ui->cameras_combo_box->count() - 1);
}

void MainWindow::choosedCamera()
{
	int cameraId = ui->cameras_combo_box->currentData().toInt();

	try
	{
		selected_camera = cameraId;
		auto cameraManager = ManagerSolution::getManager<CameraManager>();
		auto command = std::make_shared<SetCameraCommand>(cameraManager, cameraId);
		facade->execute(command);
		renderScene();
	}
	catch (std::exception &e)
	{
		showError(e.what());
	}
}

void MainWindow::choosedModel()
{
	int objectId = ui->models_combo_box->currentData().toInt();

	selected_model = objectId;
}

void MainWindow::moveModel(int id, double dx, double dy, double dz)
{
	if (id != -1)
	{
		save_state();
		auto transformManager = ManagerSolution::getManager<TransformManager>();
		auto sceneManager = ManagerSolution::getManager<SceneManager>();
		auto command = std::make_shared<MoveObjectCommand>(sceneManager, transformManager, selected_model, dx, dy, dz);
		facade->execute(command);
		renderScene();
	}
}

void MainWindow::on_move_btn_clicked()
{
	bool dx_ok, dy_ok, dz_ok;
	double dx, dy, dz;

	dx = ui->dx_edit->text().toDouble(&dx_ok);
	dy = ui->dy_edit->text().toDouble(&dy_ok);
	dz = ui->dz_edit->text().toDouble(&dz_ok);

	if (!dx_ok || !dy_ok || !dz_ok)
	{
		showError("dx, dy or dz has wrong value!");
		return;
	}

	moveModel(selected_model, dx, dy, dz);
}

void MainWindow::scaleModel(int id, double sx, double sy, double sz)
{
	if (id != -1)
	{
		save_state();
		auto transformManager = ManagerSolution::getManager<TransformManager>();
		auto sceneManager = ManagerSolution::getManager<SceneManager>();
		auto command = std::make_shared<ScaleObjectCommand>(sceneManager, transformManager, id, sx, sy, sz);
		facade->execute(command);
		renderScene();
	}
}

void MainWindow::on_scale_btn_clicked()
{
	bool sx_ok, sy_ok, sz_ok;
	double sx, sy, sz;

	sx = ui->sx_edit->text().toDouble(&sx_ok);
	sy = ui->sy_edit->text().toDouble(&sy_ok);
	sz = ui->sz_edit->text().toDouble(&sz_ok);

	if (!sx_ok || !sy_ok || !sz_ok)
	{
		showError("sx, sy or sz has wrong value!");
		return;
	}

	scaleModel(selected_model, sx, sy, sz);
}

void MainWindow::rotateModel(int id, double ax, double ay, double az)
{
	if (id != -1)
	{
		save_state();
		auto transformManager = ManagerSolution::getManager<TransformManager>();
		auto sceneManager = ManagerSolution::getManager<SceneManager>();
		auto command = std::make_shared<RotateObjectCommand>(sceneManager, transformManager, id, ax, ay, az);
		facade->execute(command);
		renderScene();
	}
}

void MainWindow::on_rotate_btn_clicked()
{
	bool ax_ok, ay_ok, az_ok;
	double ax, ay, az;

	ax = ui->ax_edit->text().toDouble(&ax_ok);
	ay = ui->ay_edit->text().toDouble(&ay_ok);
	az = ui->az_edit->text().toDouble(&az_ok);

	if (!ax_ok || !ay_ok || !az_ok)
	{
		showError("ax, ay or az has wrong value!");
		return;
	}

	rotateModel(selected_model, ax, ay, az);
}

void MainWindow::on_up_move_camera_btn_clicked()
{
	auto cameraManager = ManagerSolution::getManager<CameraManager>();
	auto command = std::make_shared<MoveCameraCommand>(cameraManager, selected_camera, 0, -20, 0);
	facade->execute(command);
	renderScene();
}

void MainWindow::on_down_camera_btn_clicked()
{
	auto cameraManager = ManagerSolution::getManager<CameraManager>();
	auto command = std::make_shared<MoveCameraCommand>(cameraManager, selected_camera, 0, 20, 0);
	facade->execute(command);
	renderScene();
}

void MainWindow::on_right_camera_btn_clicked()
{
	auto cameraManager = ManagerSolution::getManager<CameraManager>();
	auto command = std::make_shared<MoveCameraCommand>(cameraManager, selected_camera, 20, 0, 0);
	facade->execute(command);
	renderScene();
}

void MainWindow::on_left_camera_btn_clicked()
{
	auto cameraManager = ManagerSolution::getManager<CameraManager>();
	auto command = std::make_shared<MoveCameraCommand>(cameraManager, selected_camera, -20, 0, 0);
	facade->execute(command);
	renderScene();
}

void MainWindow::on_spin_up_btn_clicked()
{
	auto cameraManager = ManagerSolution::getManager<CameraManager>();
	auto command = std::make_shared<RotateCameraCommand>(cameraManager, selected_camera, 0, 1);
	facade->execute(command);
	renderScene();
}

void MainWindow::on_spin_down_btn_clicked()
{
	auto cameraManager = ManagerSolution::getManager<CameraManager>();
	auto command = std::make_shared<RotateCameraCommand>(cameraManager, selected_camera, 0, -1);
	facade->execute(command);
	renderScene();
}

void MainWindow::on_spin_right_btn_clicked()
{
	auto cameraManager = ManagerSolution::getManager<CameraManager>();
	auto command = std::make_shared<RotateCameraCommand>(cameraManager, selected_camera, -1, 0);
	facade->execute(command);
	renderScene();
}

void MainWindow::on_spin_left_btn_clicked()
{
	auto cameraManager = ManagerSolution::getManager<CameraManager>();
	auto command = std::make_shared<RotateCameraCommand>(cameraManager, selected_camera, 1, 0);
	facade->execute(command);
	renderScene();
}

void MainWindow::on_remove_model_btn_clicked()
{
	if (ui->models_combo_box->count() == 0)
	{
		showError("No models!");
		return;
	}

	auto sceneManager = ManagerSolution::getManager<SceneManager>();
	auto command = std::make_shared<DeleteObjectCommand>(sceneManager, selected_model);
	facade->execute(command);
	ui->models_combo_box->removeItem(ui->models_combo_box->currentIndex());
	if (ui->models_combo_box->count() == 0)
		selected_model = -1;
	else
		selected_model = ui->models_combo_box->currentIndex();

	renderScene();
}

void MainWindow::on_remove_camera_btn_clicked()
{
	if (ui->cameras_combo_box->count() == 0)
	{
		showError("No cameras!");
		return;
	}

	auto cameraManager = ManagerSolution::getManager<CameraManager>();
	auto command = std::make_shared<DeleteCameraCommand>(cameraManager, selected_camera);
	facade->execute(command);
	ui->cameras_combo_box->removeItem(ui->cameras_combo_box->currentIndex());
	if (ui->cameras_combo_box->count() == 0)
		selected_camera = -1;
	else
		selected_camera = ui->cameras_combo_box->currentIndex();

	renderScene();
}

void MainWindow::on_back_btn_clicked()
{
	if (ui->models_combo_box->count() == 0)
	{
		showError("No models!");
		return;
	}

	auto historyManager = ManagerSolution::getManager<HistoryManager>();
	auto command = std::make_shared<RestoreStateCommand>(historyManager, selected_model);
	facade->execute(command);

	renderScene();
}

void MainWindow::save_state()
{
	auto historyManager = ManagerSolution::getManager<HistoryManager>();
	auto command = std::make_shared<SaveStateCommand>(historyManager, selected_model);
	facade->execute(command);
}

void MainWindow::on_make_composite_btn_clicked()
{
	if (ui->models_combo_box->count() < 2)
	{
		showError("Need at least 2 models to create composite!");
		return;
	}

	auto composite_name = ui->composite_name_edit->text().toStdString();

	if (composite_name.empty())
	{
		showError("Please enter composite name!");
		return;
	}

	try
	{
		std::vector<int> modelIds;
		for (int i = 0; i < ui->models_combo_box->count(); ++i)
		{
			int modelId = ui->models_combo_box->itemData(i).toInt();
			modelIds.push_back(modelId);
		}

		auto sceneManager = ManagerSolution::getManager<SceneManager>();
		auto command = std::make_shared<CompositeObjectCommand>(sceneManager, modelIds);
		facade->execute(command);

		compositeId = command->getCompositeId();

		ui->models_combo_box->clear();
		models.clear();

		QString compositeName = QString("Composite: ") + QString::fromStdString(composite_name);
		ui->models_combo_box->addItem(compositeName, QVariant(compositeId));

		models.push_back(compositeId);

		selected_model = compositeId;
		ui->models_combo_box->setCurrentIndex(0);

		ui->composite_name_edit->clear();

		renderScene();

		QMessageBox::information(this, tr("Success"),
								 tr("Composite created successfully!"));
	}
	catch (std::exception &e)
	{
		showError(e.what());
	}

}