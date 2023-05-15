#include "stdafx.h"
#include "TcpRdtSender.h"
#include "Global.h"

void TcpRdtSender::Init()
{
	base = 0;
	nextSeqnum = 0;
	dupAckNum = 0;
}

bool TcpRdtSender::isInWindow(int seqnum)
{
	if (base < (base + wndsize) % seqsize)
	{
		return seqnum >= base && seqnum < (base + wndsize) % seqsize;
	}
	else
	{
		return seqnum >= base || seqnum < (base + wndsize) % seqsize;
	}
}

void TcpRdtSender::printSlideWindow()
{
	int i;
	for (i = 0; i < seqsize; i++)
	{
		if (i == base)
			std::cout << "[";
		std::cout << i;
		if (i == nextSeqnum)
			std::cout << "*";
		if (i == (base + wndsize - 1) % seqsize)
			std::cout << "]";
		std::cout << " ";
	}
	std::cout << std::endl;
}

TcpRdtSender::TcpRdtSender() :
	wndsize(4), seqsize(8), sendBuf(new Packet[seqsize])
{
	Init();
}

TcpRdtSender::TcpRdtSender(int wsize, int ssize) :
	wndsize(wsize), seqsize(ssize), sendBuf(new Packet[ssize])
{
	Init();
}


TcpRdtSender::~TcpRdtSender()
{
}

bool TcpRdtSender::getWaitingState()
{
	return (base + wndsize) % seqsize == (nextSeqnum) % seqsize;
}

bool TcpRdtSender::send(const Message& message)
{
	if (getWaitingState())
	{
		std::cout << "��������\n\n";
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
			pns->startTimer(SENDER, Configuration::TIME_OUT, 0);
		pns->sendToNetworkLayer(RECEIVER, sendBuf[nextSeqnum]);
		//������ϣ�����״̬
		nextSeqnum = (nextSeqnum + 1) % seqsize;
		std::cout << "���ͷ����ͺ󴰿ڣ�";
		printSlideWindow();
		std::cout << std::endl;
		return true;
	}
}

void TcpRdtSender::timeoutHandler(int seqNum)
{
	pns->sendToNetworkLayer(RECEIVER, sendBuf[base]);
	pns->stopTimer(SENDER, 0);
	pns->startTimer(SENDER, Configuration::TIME_OUT, 0);
	pUtils->printPacket("��ʱ�ش�:", sendBuf[base]);
}

void TcpRdtSender::receive(const Packet& ackPkt)
{
	int checkSum = pUtils->calculateCheckSum(ackPkt);
	if (checkSum != ackPkt.checksum)
	{
		pUtils->printPacket("���ͷ�û����ȷ�յ�ȷ��", ackPkt);
		return;
	}
	else
	{
		//if (ackPkt.acknum >= base)
		if (isInWindow(ackPkt.acknum))
		{
			base = (ackPkt.acknum + 1) % seqsize;
			pns->stopTimer(SENDER, 0);
			if (base != nextSeqnum)
			{
				pns->startTimer(SENDER, Configuration::TIME_OUT, 0);
			}
			dupAckNum = 0;
			pUtils->printPacket("���ͷ���ȷ�յ�ȷ��", ackPkt);
			std::cout << "��������״̬��";
			printSlideWindow();
			std::cout << std::endl;
		}
		else
		{//�Ѿ�ȷ�ϵ������ack
			dupAckNum = (dupAckNum + 1) % 3;
			if (dupAckNum == 2)
			{//�����ش�
				pns->sendToNetworkLayer(RECEIVER, sendBuf[base]);
				std::cout << "\n�յ�������������ack�������ش�\n\n";
			}
		}
	}
}
