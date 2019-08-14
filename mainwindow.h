#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QCustomPlot/qcustomplot.h>
#include <time.h>
#include <sstream>
#include "node.h"
#define RE_SEND
//#define NoRE_SEND
#define FLDB
#define DT
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
	std::vector<double> NMS(double speed);
	std::vector<double> NoNNs(int Neighbernum);
	int FLDB_Rule(int nms_pos,int non_pos);
	double BPITs(int bp_pos,double act);
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
	int interval_num;



};
#endif // MAINWINDOW_H
