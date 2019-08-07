#include "node.h"
void Administer::UpdateNeighber(Node* node)
{
	node->NeighberNode = new std::vector<Node>;
	for(int i = 0;i < AllNode.size();++i){
		if(strcmp(node->name,AllNode[i].name))
		if(Distance(node->posx,node->posy,AllNode[i].posx,AllNode[i].posy) <= node->radius){
			node->NeighberNode->push_back(AllNode[i]);
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
	msg->speed += 0.01;
	msg->posx = rand()%(10+1);
	msg->posy = rand()%(10+1);
	msg->radius = radius;
	administer->handleMessage(msg,this);
	delete [] msg;
}
void Node::recvMessage(cMessage* msg)
{
	//src_x = msg->src_x;
	//src_y = msg->src_y;
	//dest_x = msg->dest_x;
	//dest_y = msg->dest_y;
	speed = msg->speed;
	radius = msg->radius;
	//strcpy(name,msg->name);
	BPIT = msg->BPIT;
	posx = msg->posx;
	posy = msg->posy;
}
