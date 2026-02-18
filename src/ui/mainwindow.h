#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QGraphicsScene>
#include <QImage>
#include <QTimer>
#include <QStandardItemModel>

#include "scene.h"
#include "scene_renderer.h"
#include "camera_manager.h"
#include "turtle_interpreter_3_d.h"
#include "leaf_generator.h"
#include "l_system_generator.h"

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
	Q_OBJECT

public:
	explicit MainWindow(QWidget *parent = nullptr);
	~MainWindow() override;
	void initializeRenderer();

private slots:
	void onGenerateTree();
	void onPresetChanged(const QString& name);
	void onOrbitCameraToggled(bool checked);
	void onFreeCameraToggled(bool checked);
	void onShadowsToggled(bool checked);
	void onAddRuleClicked();
	void onDeleteRule();

private:
	Ui::MainWindow *ui;
	std::unique_ptr<QGraphicsScene> scene2D;
	QGraphicsPixmapItem* pixmapItem = nullptr;

	std::unique_ptr<SceneRenderer> renderer;
	Scene scene3D;
	CameraManager cameraManager;

	bool isDragging = false;
	QPoint lastMousePos;

	QStandardItemModel* rulesModel;

	void setupUi();
	void connectSignals();
	void loadPreset(const QString& name);
	void buildAndRenderTree();
	void render();
	void updateImage(const QImage& frame);
	void updateLight();
	void setupRulesTable();
	void populateSymbolsComboBox();
	void clearRulesTable();
	void addRuleToTable(QChar symbol, const QString& rule);

protected:
	void keyPressEvent(QKeyEvent* event) override;
	void mousePressEvent(QMouseEvent* event) override;
	void mouseReleaseEvent(QMouseEvent* event) override;
	void mouseMoveEvent(QMouseEvent* event) override;
	void wheelEvent(QWheelEvent* event) override;
	void resizeEvent(QResizeEvent* event) override;
};

#endif // MAINWINDOW_H