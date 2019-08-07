#include "mainwindow.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent) :
	QMainWindow(parent),
	ui(new Ui::MainWindow)
{
	ui->setupUi(this);
	setGeometry(400, 100, 600, 610); // 1120
	/* 暂停，继续控制键 */
	QMenu *editMenu = ui->menuBar->addMenu(tr("edit"));
	QAction *pauseAction = editMenu->addAction(tr("pause"), this, SLOT(pauseEvent()));
	QAction *continueAction = editMenu->addAction(tr("continue"), this, SLOT(continueEvent()));
	ui->mainToolBar->addAction(pauseAction);
	ui->mainToolBar->addAction(continueAction);

	/* 产生随机粒子 */
	srand(time(NULL));
	/* 初始化界面 */
	administer = new Administer;
	initplot();
	/* SLOT槽中的函数必须被定义在private slots:下才能调用 */
	connect(&eventTimer, SIGNAL(timeout()), this, SLOT(UpdatePosition()));
	eventTimer.start(100);

}
void MainWindow::initplot()
{
	/* 对点参数的设置，实线蓝色 */
	QPen pen(Qt::SolidLine);
	pen.setColor(Qt::blue);
	pen.setWidth(20);
	/* 将点添加到画板上去 */
	QCPGraph *graph0 = ui->customPlot->addGraph();
	graph0->setPen(pen);
	graph0->setLineStyle(QCPGraph::lsNone);
	graph0->setScatterStyle(QCPScatterStyle(QCPScatterStyle::ssDisc, 2));

	/* 初始位置信息 */
	QVector<double> xCoord,yCoord;
	std::stringstream ss;
	for(int i = 0;i < 10;++i){
		double x,y;
		x = rand()%(10+1);
		y = rand()%(10+1);
		xCoord.push_back(x);
		yCoord.push_back(y);

		Node node;
		ss<<i;
		ss>>node.name;
		ss.clear();

		QCPItemText* RSU = new QCPItemText(ui->customPlot);
		RSU->position->setCoords(x,y);
		RSU->setText(node.name);

		node.BPIT = 10;
		node.posx = x;
		node.posy = y;
		node.radius = 10;
		node.speed = 1;
		administer->AllNode.push_back(node);
	}
	/* 获取邻居节点信息 */
	for(int i = 0;i < administer->AllNode.size();++i )
		administer->UpdateNeighber(&administer->AllNode[i]);

	/* 用于设置x轴与y轴的刻度标签 */
	ui->customPlot->xAxis2->setVisible(true);
	ui->customPlot->xAxis2->setTickLabels(false);
	ui->customPlot->yAxis2->setVisible(true);
	ui->customPlot->yAxis2->setTickLabels(false);

	/* 不加这两句话则当鼠标滚轮时,x轴上侧与y轴右侧范围不会变化 */
	connect(ui->customPlot->xAxis, SIGNAL(rangeChanged(QCPRange)), ui->customPlot->xAxis2, SLOT(setRange(QCPRange)));
	connect(ui->customPlot->yAxis, SIGNAL(rangeChanged(QCPRange)), ui->customPlot->yAxis2, SLOT(setRange(QCPRange)));

	/* 设置x,y轴的范围 */
	ui->customPlot->xAxis->setRange(-2, 11);
	ui->customPlot->yAxis->setRange(-2, 11);

	/* 利用鼠标滚轮自动调整大小 */
	ui->customPlot->setInteractions(QCP::iRangeDrag | QCP::iRangeZoom);
	graph0->setData(xCoord,yCoord);
	ui->customPlot->replot();
}
void MainWindow::UpdatePosition()
{
	if(!running)
		return;
	/* 用于清除数据，其中graph(0)中的0代表的是初始的那张图 */
	ui->customPlot->graph(0)->data().data()->clear();
	ui->customPlot->clearItems();
	cMessage* msg;
	for(int i = 0;i < administer->AllNode.size();++i){
		administer->AllNode[i].handleMessage(msg,administer);
		QCPItemText* RSU = new QCPItemText(ui->customPlot);
		RSU->position->setCoords(administer->AllNode[i].posx,administer->AllNode[i].posy);
		RSU->setText(administer->AllNode[i].name);
	}
	QVector<double> xCoord,yCoord;
	for(int i = 0;i < administer->AllNode.size();++i){
		xCoord.push_back(administer->AllNode[i].posx);
		yCoord.push_back(administer->AllNode[i].posy);
	}
	ui->customPlot->graph(0)->setData(xCoord,yCoord);
	ui->customPlot->replot();

}
void MainWindow::pauseEvent()
{
	running = 0;
	qDebug().nospace()<<"pauseEvent()";
}
void MainWindow::continueEvent()
{
	running = 1;
	qDebug().nospace()<<"continueEvent()";
}
MainWindow::~MainWindow()
{
	delete ui;
}
