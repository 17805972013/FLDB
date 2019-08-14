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
	gx = 50;
	_gx = -10;
	gy = 50;
	_gy = -10;
	node_num = 30;
	Innercircle = 4;
	interval_num = 100;
	interval_time = 0;
	FLDB_set = true;
	FLDB_t = 0;
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

		node.BPIT = 0;
		node.posx = x;
		node.posy = y;
		node.radius = 15;
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
#ifdef DT
	administer->src->DefyTable.clear();
#endif
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
	FLDB_t += 0.1;//单位s
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
#ifdef DT
		administer->src->DefyTable.clear();
#endif
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
		if(interval_time == interval_num){
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
			/* FLDB策略 */
#ifdef FLDB
			if(FLDB_set){
				FLDB_set = false;
				FLDB_Method(&(*iter));
			}
			if(FLDB_t < iter->BPIT){
				return;
			}
			FLDB_set = true;
			FLDB_t = 0;
#endif
			/***********/
			if(strcmp(administer->dest->name,iter->name) == 0)
				return;
#ifdef DT
			Node *dnode = NULL;
			bool d_flags = false;
#endif
			bool refresh = false;
			for(std::vector<Node>::iterator _iter = iter->NeighberNode->begin();_iter != iter->NeighberNode->end();++_iter){
				if(Distance(administer->dest->posx,administer->dest->posy,_iter->posx,_iter->posy) < distance ){
#ifdef DT
			for(std::vector<Node>::iterator d_iter = administer->src->DefyTable.begin();d_iter != administer->src->DefyTable.end();++d_iter)
				if(strcmp(d_iter->name,_iter->name) == 0){
					d_flags = true;
				}
			if(d_flags){
				d_flags = false;
				qDebug().nospace()<<"DT";
				continue;
			}
#endif
					sel = true;
					distance = Distance(administer->dest->posx,administer->dest->posy,_iter->posx,_iter->posy);
					refresh = true;//用于记录上一次的数据
					_iter->flags = true;
				}
				if(refresh){
					for(std::vector<Node>::iterator __iter = administer->AllNode.begin();__iter != administer->AllNode.end();++__iter){
						if(strcmp(_iter->name,__iter->name) == 0){
							__iter->flags = true;
#ifdef DT
							dnode = &(*__iter);
#endif
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
#ifdef DT
							dnode = &(*__iter);
#endif
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
#ifdef DT
			if(strcmp(administer->src->name,iter->name) == 0 && interval_time == interval_num-1){
					administer->src->DefyTable.push_back(*dnode);
			}
#endif

			}
			break;
		}
	}

}
void MainWindow::FLDB_Method(Node* node)
{
	std::vector<double> Mem_NMS;
	std::vector<double> Mem_Non;
	std::vector<double> ActStr;
	Mem_NMS = NMS(node->speed);
	Mem_Non = NoNNs(node->NeighberNode->size());
	for(std::vector<double>::iterator iter = Mem_NMS.begin();iter != Mem_NMS.end();++iter)
		for(std::vector<double>::iterator _iter = Mem_Non.begin();_iter != Mem_Non.end();++_iter)
		if(*iter > *_iter)
			ActStr.push_back(*_iter);
		else
			ActStr.push_back(*iter);
	/*for(std::vector<double>::iterator iter = ActStr.begin();iter!=ActStr.end();++iter)
		qDebug().nospace()<<*iter;*/
	double Mem_sum = 0;
	node->BPIT = 0;
	std::vector<double>::iterator iter = ActStr.begin();
	for(int i = 0;i<(int)Mem_NMS.size();++i)
		for(int j = 0;j<(int)Mem_Non.size();++j){
			Mem_sum += *iter;
			node->BPIT += BPITs(FLDB_Rule(i,j),*iter);
			++iter;
		}
	node->BPIT /= Mem_sum;
	qDebug().nospace()<<"NMS:"<<node->speed<<" NoNNs:"<<node->NeighberNode->size()<<" BPIT:"<<node->BPIT;
}
std::vector<double> MainWindow::NMS(double speed)
{
	double a = 1.3;
	double b = 20.2;
	double c = 36.6;
	double d = 5.5;
	double e = 13.6;
	double f = 26.6;
	double g = 39.3;
	double h = 3.2;
	double i = 19.4;
	double j = 40;
	std::vector<double> Mem_NMS;
	/* Very low */
	if(0<=speed && speed<=d)
		Mem_NMS.push_back(1);
	else if(d<speed && speed<=(d+e)/2)
		Mem_NMS.push_back(1-((speed-d)/(e-d))*((speed-d)/(e-d)));
	else if((d+e)/2<speed && speed<=e)
		Mem_NMS.push_back(2*((speed-e)/(e-d))*((speed-e)/(e-d)));
	else
		Mem_NMS.push_back(0);
	/* Low */
	if(a<=speed && speed <=(a+b)/2)
		Mem_NMS.push_back((speed-a)/((a+b)/2-a));
	else if((a+b)/2<speed && speed<b)
		Mem_NMS.push_back((b-speed)/(b-(a+b)/2));
	else
		Mem_NMS.push_back(0);
	/* Medium */
	if(h<=speed && speed<=(h+c)/2)
		Mem_NMS.push_back((speed-h)/((h+c)/2-h));
	else if((h+c)/2<speed && speed<c)
		Mem_NMS.push_back((c-speed)/(c-(c+h)/2));
	else
		Mem_NMS.push_back(0);
	/* High */
	if(i<=speed && speed<=(i+g)/2)
		Mem_NMS.push_back((speed-i)/((i+g)/2-i));
	else if((i+g)/2<speed && speed<g)
		Mem_NMS.push_back((g-speed)/(g-(g+i)/2));
	else
		Mem_NMS.push_back(0);
	/* Very high */
	if(speed>=j)
		Mem_NMS.push_back(1);
	else if(f<=speed && speed<=(f+j)/2)
		Mem_NMS.push_back(2*((speed-f)/(j-f))*((speed-f)/(j-f)));
	else if((f+j)/2<speed && speed<j)
		Mem_NMS.push_back(1-2*((speed-j)/(j-f))*((speed-j)/(j-f)));
	else
		Mem_NMS.push_back(0);
	return Mem_NMS;
}
std::vector<double> MainWindow::NoNNs(int Neighbernum)
{
	double a = 2;
	double b = 3;
	double c = 9;
	double d = 16;
	double e = 23;
	double f = 29;
	double g = 30;
	std::vector<double> Mem_Non;
	/* Very small */
	if(0<=Neighbernum && Neighbernum<=a)
		Mem_Non.push_back(1);
	else if(a<Neighbernum && Neighbernum<=(a+c)/2)
		Mem_Non.push_back(1-((Neighbernum-c)/(c-a))*((Neighbernum-c)/(c-a)));
	else if((a+c)/2<Neighbernum && Neighbernum<=c)
		Mem_Non.push_back(2*((Neighbernum-c)/(c-a))*((Neighbernum-c)/(c-a)));
	else
		Mem_Non.push_back(0);
	/* Small */
	if(a<=Neighbernum && Neighbernum<=(a+d)/2)
		Mem_Non.push_back((Neighbernum-a)/((a+d)/2-a));
	else if((a+d)/2<Neighbernum && Neighbernum <= d)
		Mem_Non.push_back((d-Neighbernum)/(d-(a+d)/2));
	else
		Mem_Non.push_back(0);
	/* Medium */
	if(b<=Neighbernum && Neighbernum<=(b+f)/2)
		Mem_Non.push_back((Neighbernum-b)/((b+f)/2-b));
	else if((b+f)/2<Neighbernum && Neighbernum<=f)
		Mem_Non.push_back((f-Neighbernum)/(f-(b+f)/2));
	else
		Mem_Non.push_back(0);
	/* Large */
	if(d<=Neighbernum && Neighbernum<=(d+g)/2)
		Mem_Non.push_back((Neighbernum-d)/((d+g)/2-d));
	else if((d+g)/2<Neighbernum && Neighbernum<=g)
		Mem_Non.push_back((g-Neighbernum)/(g-(d+g)/2));
	else
		Mem_Non.push_back(0);
	/* Very Large */
	if(Neighbernum>=g)
		Mem_Non.push_back(1);
	else if(e<Neighbernum && Neighbernum<=(e+g)/2)
		Mem_Non.push_back(2*((Neighbernum-e)/(g-e))*((Neighbernum-e)/(g-e)));
	else if((e+g)/2<Neighbernum && Neighbernum<=g)
		Mem_Non.push_back(1-2*((Neighbernum-g)/(g-e))*((Neighbernum-g)/(g-e)));
	else
		Mem_Non.push_back(0);
	return Mem_Non;
}
int MainWindow::FLDB_Rule(int nms_pos, int non_pos)
{
	enum{s_verylow=0,s_low,s_medium,s_high,s_veryhigh};
	enum{non_verysmall=0,non_small,non_medium,non_large,non_verylarge};
	enum{bp_veryshort=0,bp_short,bp_medium,bp_long,bp_verylong};
	if(nms_pos == s_verylow && non_pos == non_verysmall)
		return bp_medium;
	if(nms_pos == s_verylow && non_pos == non_small)
		return bp_medium;
	if(nms_pos == s_verylow && non_pos == non_medium)
		return bp_long;
	if(nms_pos == s_verylow && non_pos == non_large)
		return bp_long;
	if(nms_pos == s_verylow && non_pos == non_verylarge)
		return bp_verylong;
	if(nms_pos == s_low && non_pos == non_verysmall)
		return bp_medium;
	if(nms_pos == s_low && non_pos == non_small)
		return bp_medium;
	if(nms_pos == s_low && non_pos == non_medium)
		return bp_medium;
	if(nms_pos == s_low && non_pos == non_large)
		return bp_long;
	if(nms_pos == s_low && non_pos == non_verylarge)
		return bp_verylong;
	if(nms_pos == s_medium && non_pos == non_verysmall)
		return bp_short;
	if(nms_pos == s_medium && non_pos == non_small)
		return bp_short;
	if(nms_pos == s_medium && non_pos == non_medium)
		return bp_medium;
	if(nms_pos == s_medium && non_pos == non_large)
		return bp_long;
	if(nms_pos == s_medium && non_pos == non_verylarge)
		return bp_long;
	if(nms_pos == s_high && non_pos == non_verysmall)
		return bp_veryshort;
	if(nms_pos == s_high && non_pos == non_small)
		return bp_short;
	if(nms_pos == s_high && non_pos == non_medium)
		return bp_medium;
	if(nms_pos == s_high && non_pos == non_large)
		return bp_medium;
	if(nms_pos == s_high && non_pos == non_verylarge)
		return bp_medium;
	if(nms_pos == s_veryhigh && non_pos == non_verysmall)
		return bp_veryshort;
	if(nms_pos == s_veryhigh && non_pos == non_small)
		return bp_short;
	if(nms_pos == s_veryhigh && non_pos == non_medium)
		return bp_short;
	if(nms_pos == s_veryhigh && non_pos == non_large)
		return bp_medium;
	if(nms_pos == s_veryhigh && non_pos == non_verylarge)
		return bp_medium;
	return bp_verylong;
}
double MainWindow::BPITs(int bp_pos,double act)
{
	enum{bp_veryshort=0,bp_short,bp_medium,bp_long,bp_verylong};
	switch(bp_pos)
	{
	/* Very short */
	case bp_veryshort:
	{
		double a = 0;
		double b = 2;
		return (a+b)*act/2;
	}
	/* Short */
	case bp_short:
	{
		double a = 0.5;
		double b = 2.9;
		return (a+b)*act/2;
	}
	/* Medium */
	case bp_medium:
	{
		double a = 1.8;
		double b = 4.3;
		return (a+b)*act/2;
	}
	/* Long */
	case bp_long:
	{
		double a = 3.1;
		double b = 5.2;
		return (a+b)*act/2;
	}
	/* Very long */
	case bp_verylong:
	{
		double a = 4;
		double b = 6;
		return (a+b)*act/2;
	}
	default:
		return -1;
	}
}
MainWindow::~MainWindow()
{
	delete ui;
}
