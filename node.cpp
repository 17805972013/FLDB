#include "node.h"
void Administer::initNode()
{

}
void Administer::UpdateNeighber(Node* node)
{
	for(int i = 0;i < AllNode.size();++i){
		if(strcmp(node->name,AllNode[i].name))
		if(Distance(node->posx,node->posy,AllNode[i].posx,AllNode[i].posy) <= node->radius){
			node->NeighberNode.push_back(AllNode[i]);
		}
	}
}
void Administer::handleMessage(cMessage* msg)
{

}
void Node::handleMessage(cMessage* msg)
{
	recvMessage(msg);
	usleep(1000*BPIT);
}
void Node::recvMessage(cMessage* msg)
{
	src_x = msg->src_x;
	src_y = msg->src_y;
	dest_x = msg->dest_x;
	dest_y = msg->dest_y;
	speed = msg->speed;
	radius = msg->radius;
	strcpy(name,msg->name);
	BPIT = msg->BPIT;
}
