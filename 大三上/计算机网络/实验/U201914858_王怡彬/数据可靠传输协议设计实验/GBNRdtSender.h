#pragma once
#include "RdtSender.h"
#include "DataStructure.h"
class GBNRdtSender :
	public RdtSender
{
private:
	int base; //����ţ������δȷ�Ϸ�������
	int nextSeqnum;	//��һ��������������
	const int wndsize; //�������ڴ�С��ʵ�齨��Ϊ4
	const int seqsize; //��Ŵ�С��ʵ�齨�������Ϊ3λ����Ϊ8
	Packet* const sendBuf; //���ͻ����������淢�͵ı��ģ������ش�

private:
	void Init();
	bool isInWindow(int seqnum);
	void printSlideWindow();

public:
	GBNRdtSender();
	GBNRdtSender(int wsize, int sSize);
	virtual ~GBNRdtSender();

	bool getWaitingState();
	bool send(const Message& message);
	void timeoutHandler(int seqNum);
	void receive(const Packet& ackPkt);
};

