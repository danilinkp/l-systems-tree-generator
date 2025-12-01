#ifndef MAIN_WINDOW_H
#define MAIN_WINDOW_H

#include <QMainWindow>
#include <QGraphicsScene>


QT_BEGIN_NAMESPACE
namespace Ui
{
class MainWindow;
}
QT_END_NAMESPACE

class MainWindow: public QMainWindow
{
Q_OBJECT
	void on_down_move_camera_btn_clicked();
public:
	explicit MainWindow(QWidget *parent = nullptr);
	~MainWindow() override;

private slots:
	void on_load_model_btn_clicked();
	void on_add_camera_btn_clicked();
	void on_move_btn_clicked();
	void on_scale_btn_clicked();
	void on_rotate_btn_clicked();
	void on_up_move_camera_btn_clicked();
	void on_down_camera_btn_clicked();
	void on_left_camera_btn_clicked();
	void on_right_camera_btn_clicked();
	void on_spin_up_btn_clicked();
	void on_spin_down_btn_clicked();
	void on_spin_right_btn_clicked();
	void on_spin_left_btn_clicked();
	void on_remove_model_btn_clicked();
	void on_remove_camera_btn_clicked();
	void on_back_btn_clicked();
	void on_make_composite_btn_clicked();

private:
	void setupScene(QWidget *parent);
	void showError(const std::string& message);
	void choosedCamera();
	void choosedModel();
	void renderScene();
	void moveModel(int id, double x, double y, double z);
	void scaleModel(int id, double sx, double sy, double sz);
	void rotateModel(int id, double ax, double ay, double az);
	void save_state();

private:
	Ui::MainWindow *ui;
	std::shared_ptr<QGraphicsScene> scene;
};


#endif //MAIN_WINDOW_H
