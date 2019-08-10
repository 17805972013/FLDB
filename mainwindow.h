#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QCustomPlot/qcustomplot.h>
#include <time.h>
#include <sstream>
#include "node.h"
//#define RE_SEND
#define NoRE_SEND
#define FLDB
class Schedule;
namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
	Q_OBJECT
public:
	explicit MainWindow(QWidget *parent = 0);
	void initplot();
	/* 贪婪右手转发策略 */
	void Greedy_Right_Method();
	/* FLDB转发策略 */
	bool FLDB_set;
	double FLDB_t;
	void FLDB_Method(Node* node);
	~MainWindow();
private slots:
	void UpdatePosition();
	void pauseEvent();
	void continueEvent();

private:
	QTimer eventTimer;
	int running;
	Administer* administer;
	Ui::MainWindow *ui;
	int node_num;
	double Innercircle;
	double gx;
	double _gx;
	double gy;
	double _gy;
	/* 重发时间间隔 */
	int interval_time;



};
#endif // MAINWINDOW_H
