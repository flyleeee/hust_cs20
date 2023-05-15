#pragma once
#include "RdtSender.h"
class TcpRdtSender :
	public RdtSender
{
private:
	//bool waitingState;
	int base;							//�����
	int nextSeqnum;						//��һ��������������
	const int wndsize;					//�������ڴ�С
	const int seqsize;					//��Ŵ�С
	Packet* const sendBuf;				//���ͻ�����
	int dupAckNum;							//�յ�3������ack�����ش�

private:
	void Init();
	bool isInWindow(int seqnum);
	void printSlideWindow();

public:
	TcpRdtSender();
	TcpRdtSender(int wsize, int ssize);
	virtual ~TcpRdtSender();

public:
	bool getWaitingState();
	bool send(const Message& message);
	void timeoutHandler(int seqNum);
	void receive(const Packet& ackPkt);
};

