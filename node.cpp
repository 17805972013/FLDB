#include "node.h"
void Administer::UpdateNeighber(Node* node)
{
	node->NeighberNode = new std::vector<Node>;
	for(std::vector<Node>::iterator iter = AllNode.begin();iter != AllNode.end();++iter){
		if(strcmp(node->name,iter->name))
		if(Distance(node->posx,node->posy,iter->posx,iter->posy) <= node->radius){
			node->NeighberNode->push_back(*iter);
		}
	}
}
void Administer::handleMessage(cMessage* msg,Node* node)
{
	UpdateNeighber(node);
	node->recvMessage(msg);
}
void Node::handleMessage(cMessage* msg,Administer* administer)
{
	msg = new cMessage;
	/* 限定车辆的移动位置 */

	msg->speed = rand()%45;
	msg->posx = posx + speed*0.1;
	msg->posy = posy + speed*0.1;
	msg->BPIT = BPIT;
	msg->radius = radius;
	msg->src_x = administer->src->posx;
	msg->src_y = administer->src->posy;
	msg->dest_x = administer->dest->posx;
	msg->dest_y = administer->dest->posy;
	administer->handleMessage(msg,this);
	delete [] msg;
}
void Node::recvMessage(cMessage* msg)
{
	src_x = msg->src_x;
	src_y = msg->src_y;
	dest_x = msg->dest_x;
	dest_y = msg->dest_y;
	speed = msg->speed;
	radius = msg->radius;
	//strcpy(name,msg->name);
	BPIT = msg->BPIT;
	posx = msg->posx;
	posy = msg->posy;
}
