#include "stdafx.h"
#include "SRRdtReceiver.h"
#include "Global.h"

void SRRdtReceiver::Init()
{
	base = 0;
	//nextSeqnum = 0;
	for (int i = 0; i < seqsize; i++)
		bufStatus[i] = false;
	lastAckPkt.acknum = -1; //��ʼ״̬�£��ϴη��͵�ȷ�ϰ���ȷ�����Ϊ0��ʹ�õ���һ�����ܵ����ݰ�����ʱ��ȷ�ϱ��ĵ�ȷ�Ϻ�Ϊ0
	lastAckPkt.checksum = 0;
	lastAckPkt.seqnum = -1;	//���Ը��ֶ�
	memset(lastAckPkt.payload, '.', Configuration::PAYLOAD_SIZE);
	lastAckPkt.checksum = pUtils->calculateCheckSum(lastAckPkt);
}

void SRRdtReceiver::printSlideWindow()
{
	int i;
	for (i = 0; i < seqsize; i++)
	{
		if (i == base)
			std::cout << " [ ";
		if (isInWindow(i) && bufStatus[i] == true)
			std::cout << i << "*";
		else if (isInWindow(i))
			std::cout << i;
		if (i == (base + wndsize) % seqsize)
			std::cout << " ] ";
		if (isInWindow(i) == false)
			std::cout << i;
		std::cout << " ";
	}
	std::cout << std::endl;
}

bool SRRdtReceiver::isInWindow(int seqnum)
{
	//return seqnum >= base && seqnum <= (base + wndsize) % seqsize;
	//cout << seqnum << ':' << base << ':' << (base + wndsize) % seqsize << endl;
	if (base < (base + wndsize) % seqsize)
		return seqnum >= base && seqnum < (base + wndsize) % seqsize;
	else
		return seqnum >= base || seqnum < (base + wndsize) % seqsize;
}

SRRdtReceiver::SRRdtReceiver() :
	seqsize(8), wndsize(4), recvBuf(new Message[seqsize]), bufStatus(new bool[seqsize])
{
	Init();
}

SRRdtReceiver::SRRdtReceiver(int sSize, int wsize) :
	seqsize(sSize), wndsize(wsize), recvBuf(new Message[seqsize]), bufStatus(new bool[seqsize])
{
	Init();
}

void SRRdtReceiver::receive(const Packet& packet)
{
	int checksum = pUtils->calculateCheckSum(packet);
	if (checksum != packet.checksum)
	{
		//���ݰ��𻵣�������Ӧ��
		pUtils->printPacket("���շ�û����ȷ�յ����ͷ��ı���,����У�����", packet);
		return;
	}
	else
	{
		if (isInWindow(packet.seqnum) == false)
		{
			//������Ҫ�����ݰ���������Ӧ��
			pUtils->printPacket("���Ǵ����ڵķ��飬����", packet);
			lastAckPkt.acknum = packet.seqnum;
			lastAckPkt.seqnum = -1;
			memset(lastAckPkt.payload, '.', Configuration::PAYLOAD_SIZE);
			lastAckPkt.checksum = pUtils->calculateCheckSum(lastAckPkt);
			pns->sendToNetworkLayer(SENDER, lastAckPkt);
			return;
		}
		else
		{
			//���Լ���Ҫ�����ݰ�������ack�����»������ͻ�������

			if (packet.seqnum == base) 
			{
				Message msg;
				memcpy(msg.data, packet.payload, sizeof(packet.payload));
				int flag = base;  
				for (int i = (base + 1) % seqsize, j = 1; j < wndsize; j++, i = (i + 1) % seqsize) 
				{
					if (bufStatus[i] == true) 
						flag = i;
					else 
						break;
				}
				if (flag == base) 
				{
					pns->delivertoAppLayer(RECEIVER, msg);
				}
				else 
				{
					pns->delivertoAppLayer(RECEIVER, msg);
					for (int i = (base + 1) % seqsize, j = 0; j < (flag - base + seqsize) % seqsize; j++, i = (i + 1) % seqsize) {
						pns->delivertoAppLayer(RECEIVER, recvBuf[i]);
						bufStatus[i] = false;
					}
				}
				base = (flag + 1) % seqsize;
				std::cout << "\n���շ������ƶ���";
				printSlideWindow();
				std::cout << std::endl;
			}
			else 
			{
				memcpy(recvBuf[packet.seqnum].data, packet.payload, sizeof(packet.payload));
				bufStatus[packet.seqnum] = true;
				printf("��������жϣ����ܷ����汨�����%d��base=%d\n", packet.seqnum, base);
			}

			lastAckPkt.acknum = packet.seqnum; //ȷ����ŵ����յ��ı������
			lastAckPkt.checksum = pUtils->calculateCheckSum(lastAckPkt);
			pUtils->printPacket("���շ�����ȷ�ϱ���", lastAckPkt);
			pns->sendToNetworkLayer(SENDER, lastAckPkt);	//����ģ�����绷����sendToNetworkLayer��ͨ������㷢��ȷ�ϱ��ĵ��Է�
		}
	}

}


SRRdtReceiver::~SRRdtReceiver()
{
	delete[] recvBuf;
	delete[] bufStatus;
}
