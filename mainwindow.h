#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QCustomPlot/qcustomplot.h>
#include <time.h>
#include <sstream>
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
	void pauseEvent();
	void continueEvent();

private:
	QTimer eventTimer;
	Administer* administer;
	int running;
	Ui::MainWindow *ui;


};

#endif // MAINWINDOW_H
