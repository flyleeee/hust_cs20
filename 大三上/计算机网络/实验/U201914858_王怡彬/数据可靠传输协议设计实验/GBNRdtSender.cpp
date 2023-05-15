#include "stdafx.h"
#include "GBNRdtSender.h"
#include "Global.h"

enum SeqStateEnum
{
	CONFIRMED,					//�ѱ�ȷ��
	AVAILABLE,					//���ã���δ����
	SENT,						//���ͣ���δȷ��
	UNAVAILABLE					//������
};

//��ʼ����������״̬
void GBNRdtSender::Init()
{
	base = 0;
	nextSeqnum = 0;
}

bool GBNRdtSender::isInWindow(int seqnum)
{
	if (base < (base + wndsize) % seqsize)
		return seqnum >= base && seqnum < (base + wndsize) % seqsize;
	else
		return seqnum >= base || seqnum < (base + wndsize) % seqsize;
}

GBNRdtSender::GBNRdtSender() :
	wndsize(4), seqsize(8), sendBuf(new Packet[8])
{
	Init();
}

GBNRdtSender::GBNRdtSender(int wsize, int sSize) :
	wndsize(wsize), seqsize(sSize), sendBuf(new Packet[sSize])
{
	Init();
}


GBNRdtSender::~GBNRdtSender()
{
	delete[] sendBuf;
}

//�ϲ����send������������
bool GBNRdtSender::send(const Message& message)
{
	if (getWaitingState())
	{//�����������޷�������������
		std::cout << " �������� \n\n";
		return false;
	}
	else
	{
		sendBuf[nextSeqnum].acknum = -1;
		sendBuf[nextSeqnum].seqnum = nextSeqnum;
		memcpy(sendBuf[nextSeqnum].payload, message.data, sizeof(message.data));
		sendBuf[nextSeqnum].checksum = pUtils->calculateCheckSum(sendBuf[nextSeqnum]);
		pUtils->printPacket("���ͷ����ͱ���", sendBuf[nextSeqnum]);
		if (base == nextSeqnum)
		{
			pns->startTimer(SENDER, Configuration::TIME_OUT, 0);
		}
		pns->sendToNetworkLayer(RECEIVER, sendBuf[nextSeqnum]);
		//������ϣ�����״̬
		nextSeqnum = (nextSeqnum + 1) % seqsize;
		//��ʾ��������
		std::cout << "���ͷ����ͺ󴰿ڣ�";
		printSlideWindow();
		std::cout << std::endl;
		return true;
	}
}

//�����Ƿ��ڵȴ�״̬������������true�����򷵻�false
bool GBNRdtSender::getWaitingState()
{
	//����ʵ���ĵ���gbnЭ���л������������޷������ϲ�Ӧ������
	return (base + wndsize) %  this->seqsize == (nextSeqnum) % seqsize;
}

//����ack
void GBNRdtSender::receive(const Packet& ackPkt)
{
	int checkSum = pUtils->calculateCheckSum(ackPkt);
	if (checkSum != ackPkt.checksum)
	{
		pUtils->printPacket("���ͷ�û����ȷ�յ�ȷ��", ackPkt);
	}
	else
	{
		base = (ackPkt.acknum + 1) % seqsize;//�ۻ�ȷ��
		if (base == nextSeqnum)
		{
			pns->stopTimer(SENDER, 0);
		}
		else
		{
			//������ʱ��
			pns->stopTimer(SENDER, 0);
			pns->startTimer(SENDER, Configuration::TIME_OUT, 0);
		}
		pUtils->printPacket("���ͷ���ȷ�յ�ȷ��", ackPkt);
		std::cout << "��������״̬��";
		printSlideWindow();
		std::cout << std::endl;
	}
}

//����ʱ
void GBNRdtSender::timeoutHandler(int seqNum)
{
	//�ط������Ѿ�������δȷ�ϵķ���
	if (nextSeqnum == base)
	{
		//��ʱ��������������
		return;
	}
	else
	{
		pns->startTimer(SENDER, Configuration::TIME_OUT, 0);//������ʱ�������¼�ʱ
		int i;
		for (i = base; i != nextSeqnum; i = (i + 1) % seqsize)
		{
			pns->sendToNetworkLayer(RECEIVER, sendBuf[i]);
			pUtils->printPacket("��ʱ�ش�:", sendBuf[i]);
		}
	}
}

void GBNRdtSender::printSlideWindow()
{
	
	for (int i = 0; i < seqsize; i++)
	{
		if (i == base)
			std::cout << " [ ";
		std::cout << i;
		if (i == nextSeqnum)
			std::cout << "*";
		if (i == (base + wndsize - 1) % seqsize)
			std::cout << " ] ";
		std::cout << " ";
	}
	std::cout << std::endl;
}

