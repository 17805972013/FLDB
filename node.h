#ifndef NODE_H
#define NODE_H
#include <vector>
#include "cmessage.h"
#include "utils.h"
#include <unistd.h>
#include <string.h>
#include <vector>
class Node;
class Administer
{
public:
	/* 当前节点信息 */
	std::vector<Node> AllNode;
	/* 更新节点的邻居表信息 */
	void UpdateNeighber(Node* node);
	/* 处理消息 */
	void handleMessage(cMessage* msg,Node* node);
	/* 记录源节点与目的节点 */
	Node *src;
	Node *dest;
};
class Node
{
public:
	/* 消息到达位置 */
	bool flags;       //flags为假表示消息未到达，否则代表消息到达当前位置
	/* 源节点位置信息 */
	double src_x,src_y;
	/* 目的节点位置信息 */
	double dest_x,dest_y;
	/* 节点广播时间间隔  */
	double BPIT;
	/* 节点移动速度 */
	double speed;
	/* 节点通信范围 */
	double radius;
	/* 节点名称 */
	char name[50];
	/* 位置信息 */
	double posx,posy;
	/* 邻居表 */
	std::vector<Node>* NeighberNode;
	/* 拒绝表 */
	std::vector<Node> DefyTable;
	/* 接收消息 */
	void recvMessage(cMessage* msg);
	/* 消息处理 */
	void handleMessage(cMessage* msg,Administer* administer);

};
#endif // NODE_H
