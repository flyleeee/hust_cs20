#pragma once
#include "RdtSender.h"
//#include "SeqStateEnum.h"
class SRRdtSender :
	public RdtSender
{
private:
	const int seqsize; //��Ŵ�С��ʵ�齨�������Ϊ3λ����Ϊ8
	const int wndsize; //�������ڴ�С��ʵ�齨��Ϊ4
	Packet* const sendBuf;//���ͻ����������ⷴ����������
	bool* const bufStatus;
	int base, nextSeqnum;

private:
	void Init();
	void printSlideWindow();
	bool isInWindow(int seqnum);

public:
	SRRdtSender(int sSize, int wsize);
	SRRdtSender();
	bool send(const Message& message);
	bool getWaitingState();
	void timeoutHandler(int seqnum);
	void receive(const Packet& ackPkt);
	virtual ~SRRdtSender();
};

