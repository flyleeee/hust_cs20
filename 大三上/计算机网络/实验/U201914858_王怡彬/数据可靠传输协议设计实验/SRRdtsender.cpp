#include "stdafx.h"
#include "SRRdtSender.h"
#include "Tool.h"
#include "Global.h"

void SRRdtSender::Init()
{
	base = nextSeqnum = 0;
	for (int i = 0; i < seqsize; i++)
		bufStatus[i] = false;
}

void SRRdtSender::printSlideWindow()
{
	int i;
	for (i = 0; i < seqsize; i++)
	{
		if (i == base)
			std::cout << " [ ";
		if (isInWindow(i) && i == nextSeqnum)			//���ã�δ����
			std::cout << "|"  << i;
		else if (isInWindow(i) && bufStatus[i] == true)		//���ͣ���ȷ��
			std::cout << i << "*";
		else if (isInWindow(i))						//�ѷ��ͣ�δȷ��
			std::cout << i;
		if (i == (base + wndsize) % seqsize)
			std::cout << " ] ";
		if (isInWindow(i) == false)
			std::cout << i;						//�����ã�������
		std::cout << " ";
	}
	std::cout << std::endl;
}

//�ж�����Ƿ��ڴ�����
bool SRRdtSender::isInWindow(int seqnum)
{
	//return seqnum >= base && seqnum <= (base + wndsize) % seqsize;
	if (base < (base + wndsize) % seqsize)
		return seqnum >= base && seqnum < (base + wndsize) % seqsize;
	else
		return seqnum >= base || seqnum < (base + wndsize) % seqsize;
}

SRRdtSender::SRRdtSender(int sSize, int wsize) :
	seqsize(sSize), wndsize(wsize), sendBuf(new Packet[sSize]), bufStatus(new bool[sSize])
{
	Init();
}

SRRdtSender::SRRdtSender() :
	seqsize(8), wndsize(4), sendBuf(new Packet[8]), bufStatus(new bool[8])
{
	Init();
}

bool SRRdtSender::send(const Message& message)
{
	if (getWaitingState())
	{//�����������ȴ�ack
		std::cout << "��������\n\n";
		return false;
	}
	bufStatus[nextSeqnum] = false;
	sendBuf[nextSeqnum].acknum = -1;
	sendBuf[nextSeqnum].seqnum = nextSeqnum;
	memcpy(sendBuf[nextSeqnum].payload, message.data, sizeof(message.data));
	sendBuf[nextSeqnum].checksum = pUtils->calculateCheckSum(sendBuf[nextSeqnum]);
	pUtils->printPacket("���ͷ����ͱ���", sendBuf[nextSeqnum]);
	//���ͱ���
	pns->sendToNetworkLayer(RECEIVER, sendBuf[nextSeqnum]);
	//������ʱ����srЭ����ÿ�������Ӧһ����ʱ��
	pns->startTimer(SENDER, Configuration::TIME_OUT, nextSeqnum);
	//������ϣ�����״̬
	nextSeqnum = (nextSeqnum + 1) % seqsize;
	std::cout << "���ͷ����ͺ󴰿ڣ�";
	printSlideWindow();
	std::cout << std::endl;
	return true;
}


bool SRRdtSender::getWaitingState()
{
	return (base + wndsize) % seqsize == (nextSeqnum) % seqsize;
}

void SRRdtSender::receive(const Packet& ackPkt)
{
	int checksum = pUtils->calculateCheckSum(ackPkt);
	if (checksum != ackPkt.checksum)
	{
		pUtils->printPacket("���ͷ�û����ȷ�յ�ȷ��", ackPkt);
		return;
	}
	else
	{
		pns->stopTimer(SENDER, ackPkt.acknum);
		if (isInWindow(ackPkt.acknum))
		{
			//���´���
			bufStatus[ackPkt.acknum] = true;
			while (bufStatus[base] == true)
			{
				//�ƶ�base
				bufStatus[base] = false;
				base = (base + 1) % seqsize;
			}
			pUtils->printPacket("���ͷ���ȷ�յ�ȷ��", ackPkt);
			pns->stopTimer(SENDER, ackPkt.acknum);	
			std::cout << "���ͷ���������״̬��";
			printSlideWindow();
			std::cout << std::endl;
		}
	}
}

void SRRdtSender::timeoutHandler(int seqnum)
{
	pUtils->printPacket("��ʱ�ش�", sendBuf[seqnum]);
	pns->sendToNetworkLayer(RECEIVER, sendBuf[seqnum]);
	pns->stopTimer(SENDER, seqnum);
	pns->startTimer(SENDER, Configuration::TIME_OUT, seqnum);

}

SRRdtSender::~SRRdtSender()
{
	delete[] bufStatus;
	delete[] sendBuf;
}
