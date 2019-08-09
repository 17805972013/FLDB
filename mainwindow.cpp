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
	eventTimer.start(300);

}
void MainWindow::initplot()
{
	gx = 50;
	_gx = -10;
	gy = 50;
	_gy = -10;
	node_num = 30;
	Innercircle = 4;
	interval_time = 0;
	/* 对点参数的设置，实线蓝色 */
	QPen pen(Qt::SolidLine);
	pen.setColor(Qt::blue);
	pen.setWidth(1);
	/* 将点添加到画板上去 */
	QCPGraph *graph0 = ui->customPlot->addGraph();
	graph0->setPen(pen);
	graph0->setLineStyle(QCPGraph::lsNone);
	graph0->setScatterStyle(QCPScatterStyle(QCPScatterStyle::ssDisc, 2));

	/* 初始位置信息 */
	QVector<double> xCoord,yCoord;
	std::stringstream ss;
	for(int i = 0;i < node_num;++i){
		double x,y;
		x = rand()%(40);
		y = rand()%(40);
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
		node.speed = 0;
		node.flags = false;
		node.NeighberNode = NULL;


		administer->AllNode.push_back(node);
	}
	/* 记录源节点与目的节点 */
	int src = rand()%node_num;
	int dest = rand()%node_num;
	while(1){
		if(src != dest){
			administer->src = &administer->AllNode[src];
			administer->dest = &administer->AllNode[dest];
			break;
		}
		src = rand()%node_num;
		dest = rand()%node_num;
	}
	/* 消息到达位置 */
	administer->AllNode[src].flags = true;

			/*  画图操作 */
			QPen _pen;
			_pen.setColor(Qt::darkCyan);
			QCPItemEllipse *picture = new QCPItemEllipse(ui->customPlot);
			picture->topLeft->setCoords(administer->src->posx - administer->src->radius,administer->src->posy + administer->src->radius);
			picture->bottomRight->setCoords(administer->src->posx + administer->src->radius,administer->src->posy - administer->src->radius);
			picture->setPen(_pen);
			/* 源节点 */
			_pen.setColor(Qt::red);
			QCPItemEllipse *_picture = new QCPItemEllipse(ui->customPlot);
			_picture->topLeft->setCoords(administer->src->posx - Innercircle,administer->src->posy + Innercircle);
			_picture->bottomRight->setCoords(administer->src->posx + Innercircle,administer->src->posy - Innercircle);
			_picture->setPen(_pen);
			/* 目的节点 */
			_pen.setColor(Qt::darkMagenta);
//			_pen(Qt::DashLine);
			QCPItemEllipse *__picture = new QCPItemEllipse(ui->customPlot);
			__picture->topLeft->setCoords(administer->dest->posx - Innercircle,administer->dest->posy + Innercircle);
			__picture->bottomRight->setCoords(administer->dest->posx + Innercircle,administer->dest->posy - Innercircle);
			__picture->setPen(_pen);

	/* 获取邻居节点信息 */
		for(std::vector<Node>::iterator iter = administer->AllNode.begin();iter != administer->AllNode.end();++iter)
			administer->UpdateNeighber(&(*iter));


	/* 用于设置x轴与y轴的刻度标签 */
	ui->customPlot->xAxis2->setVisible(true);
	ui->customPlot->xAxis2->setTickLabels(false);
	ui->customPlot->yAxis2->setVisible(true);
	ui->customPlot->yAxis2->setTickLabels(false);

	/* 不加这两句话则当鼠标滚轮时,x轴上侧与y轴右侧范围不会变化 */
	connect(ui->customPlot->xAxis, SIGNAL(rangeChanged(QCPRange)), ui->customPlot->xAxis2, SLOT(setRange(QCPRange)));
	connect(ui->customPlot->yAxis, SIGNAL(rangeChanged(QCPRange)), ui->customPlot->yAxis2, SLOT(setRange(QCPRange)));

	/* 设置x,y轴的范围 */
	ui->customPlot->xAxis->setRange(_gx, gx);
	ui->customPlot->yAxis->setRange(_gy, gy);

	/* 利用鼠标滚轮自动调整大小 */
	ui->customPlot->setInteractions(QCP::iRangeDrag | QCP::iRangeZoom);
	graph0->setData(xCoord,yCoord);


	ui->customPlot->replot();
}
void MainWindow::UpdatePosition()
{
	if(!running)
		return;
	++interval_time;
	gx = administer->src->posx+30;
	_gx = administer->src->posx-30;
	gy = administer->src->posy+30;
	_gy = administer->src->posy-30;
	/* 设置x,y轴的范围 */
	ui->customPlot->xAxis->setRange(_gx, gx);
	ui->customPlot->yAxis->setRange(_gy, gy);
	/* 用于清除数据，其中graph(0)中的0代表的是初始的那张图 */
	ui->customPlot->graph(0)->data().data()->clear();
	ui->customPlot->clearItems();
	cMessage* msg = NULL;
	for(std::vector<Node>::iterator iter = administer->AllNode.begin();iter != administer->AllNode.end();++iter){
		(*iter).handleMessage(msg,administer);
		QCPItemText* RSU = new QCPItemText(ui->customPlot);
		RSU->position->setCoords((*iter).posx,(*iter).posy);
		RSU->setText((*iter).name);
	}
	QVector<double> xCoord,yCoord;
	for(std::vector<Node>::iterator iter = administer->AllNode.begin();iter != administer->AllNode.end();++iter){
		xCoord.push_back((*iter).posx);
		yCoord.push_back((*iter).posy);

	}
	/* 消息到达位置 */
	Greedy_Right_Method();
	if(administer->dest->flags){
		administer->src = administer->dest;
		administer->dest = &administer->AllNode[rand()%node_num];
		while(1){
			if(strcmp(administer->dest->name,administer->src->name))
				break;
			administer->dest = &administer->AllNode[rand()%node_num];
		}
	}
	/*  源节点 */
	QPen _pen;
	_pen.setColor(Qt::red);
	QCPItemEllipse *_picture = new QCPItemEllipse(ui->customPlot);
	_picture->topLeft->setCoords(administer->src->posx - Innercircle,administer->src->posy + Innercircle);
	_picture->bottomRight->setCoords(administer->src->posx + Innercircle,administer->src->posy - Innercircle);
	_picture->setPen(_pen);
	/* 目的节点 */
	_pen.setColor(Qt::darkMagenta);
	QCPItemEllipse *__picture = new QCPItemEllipse(ui->customPlot);
	__picture->topLeft->setCoords(administer->dest->posx - Innercircle,administer->dest->posy + Innercircle);
	__picture->bottomRight->setCoords(administer->dest->posx + Innercircle,administer->dest->posy - Innercircle);
	__picture->setPen(_pen);

	ui->customPlot->graph(0)->setData(xCoord,yCoord);
	/*  画图操作 */
	#ifdef NoRE_SEND
	for(std::vector<Node>::iterator iter = administer->AllNode.begin();iter != administer->AllNode.end();++iter)
		if(iter->flags){
				QPen _pen;
				_pen.setColor(Qt::darkCyan);
				QCPItemEllipse *picture = new QCPItemEllipse(ui->customPlot);
				picture->topLeft->setCoords(iter->posx - iter->radius,iter->posy + iter->radius);
				picture->bottomRight->setCoords(iter->posx + iter->radius,iter->posy - iter->radius);
				picture->setPen(_pen);
				break;
		}
	#endif
	#ifdef RE_SEND
	bool _reset = false;
	for(std::vector<Node>::iterator iter = administer->AllNode.begin();iter != administer->AllNode.end();++iter)
		if(interval_time == 100){
			interval_time = 0;
			_reset = true;
		}
	else
		if(iter->flags){
			if(_reset){
				iter->flags = false;
				iter = administer->AllNode.begin();
				administer->src->flags = true;
				_reset = false;
			}
				QPen _pen;
				_pen.setColor(Qt::darkCyan);
				QCPItemEllipse *picture = new QCPItemEllipse(ui->customPlot);
				picture->topLeft->setCoords(iter->posx - iter->radius,iter->posy + iter->radius);
				picture->bottomRight->setCoords(iter->posx + iter->radius,iter->posy - iter->radius);
				picture->setPen(_pen);
				break;
		}
	#endif
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
void MainWindow::Greedy_Right_Method()
{
	/* 判断选择那种方法，贪婪或是右手,为真贪婪，否则右手 */
	bool sel = false;
	std::vector<Node>::iterator iter;
	for(iter = administer->AllNode.begin();iter != administer->AllNode.end();++iter){

		double distance = Distance(administer->dest->posx,administer->dest->posy,iter->posx,iter->posy);
		if(iter->flags){
			if(strcmp(administer->dest->name,iter->name) == 0)
				return;
			bool refresh = false;
			for(std::vector<Node>::iterator _iter = iter->NeighberNode->begin();_iter != iter->NeighberNode->end();++_iter){
				if(Distance(administer->dest->posx,administer->dest->posy,_iter->posx,_iter->posy) < distance ){
					sel = true;
					distance = Distance(administer->dest->posx,administer->dest->posy,_iter->posx,_iter->posy);
					refresh = true;//用于记录上一次的数据
				}
				if(refresh){
					for(std::vector<Node>::iterator __iter = administer->AllNode.begin();__iter != administer->AllNode.end();++__iter){
						if(strcmp(_iter->name,__iter->name) == 0){
							__iter->flags = true;
						}
						else
							__iter->flags = false;
					}
					refresh = false;
				}
			}
			if(distance == 0)
				return;
			if(sel){
				qDebug().nospace()<<"Greedy";
			}
			else
			{
				/* 右手转发策略 */
				double the_ta = 2*acos(-1.0);
				bool _refresh = true;
				refresh = false;
				for(std::vector<Node>::iterator _iter = iter->NeighberNode->begin();_iter != iter->NeighberNode->end();++_iter){
					double a = Distance(iter->posx,iter->posy,administer->dest->posx,administer->dest->posy);
					double b = Distance(iter->posx,iter->posy,_iter->posx,_iter->posy);
					double c = Distance(_iter->posx,_iter->posy,administer->dest->posx,administer->dest->posy);
					if(acos((a*a+b*b-c*c)/(2*a*b)) < the_ta){
						the_ta = acos((a*a+b*b-c*c)/(2*a*b));
						refresh = true;

					}
					if(refresh){
						for(std::vector<Node>::iterator __iter = administer->AllNode.begin();__iter != administer->AllNode.end();++__iter){
							if(strcmp(_iter->name,__iter->name) == 0){
								__iter->flags = true;
							}
							else
								__iter->flags = false;
						}
						refresh = false;
					}
					if(_refresh){
						qDebug().nospace()<<"RightHand";
						_refresh = false;
					}
				}

			}
			break;
		}
	}

}
MainWindow::~MainWindow()
{
	delete ui;
}
