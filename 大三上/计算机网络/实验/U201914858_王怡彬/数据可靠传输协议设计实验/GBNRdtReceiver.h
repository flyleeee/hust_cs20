#pragma once
#include "RdtReceiver.h"
class GBNRdtReceiver :
	public RdtReceiver
{
private:
	int expectSequenceNumberRcvd;	// �ڴ��յ�����һ���������
	Packet lastAckPkt;				//�ϴη��͵�ȷ�ϱ���
	const int seqsize;

private:
	void Init();

public:
	GBNRdtReceiver(int sSize);
	GBNRdtReceiver();
	virtual ~GBNRdtReceiver();

public:

	void receive(const Packet& packet); //���ձ��ģ�����NetworkService����
};
