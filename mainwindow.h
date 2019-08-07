#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QCustomPlot/qcustomplot.h>
#include <time.h>
#include "node.h"
namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
	Q_OBJECT

public:
	explicit MainWindow(QWidget *parent = 0);
	void initplot();

	~MainWindow();
private slots:
	void UpdatePosition();

private:
	Ui::MainWindow *ui;
	QTimer eventTimer;
	Administer administer;

};

#endif // MAINWINDOW_H
