//// StopWait.cpp : �������̨Ӧ�ó������ڵ㡣
////
#include "stdafx.h"
#include "Global.h"
#include "RdtSender.h"
#include "RdtReceiver.h"
#include "StopWaitRdtSender.h"
#include "StopWaitRdtReceiver.h"
#include "GBNRdtReceiver.h"
#include "GBNRdtSender.h"
#include "SRRdtReceiver.h"
#include "SRRdtSender.h"
#include "TcpRdtSender.h"
#include <algorithm>

using namespace std;

int main(int argc, char* argv[])
{
	int verbos = 1;
	//GBNRdtSender* ps = new GBNRdtSender();
	//GBNRdtReceiver* pr = new GBNRdtReceiver();
	//SRRdtSender* ps = new SRRdtSender();
	//SRRdtReceiver* pr = new SRRdtReceiver();
	TcpRdtSender* ps = new TcpRdtSender();
	GBNRdtReceiver* pr = new GBNRdtReceiver();


	pns->setRunMode(verbos);  //����ģʽ/VERBOSģʽ
	pns->init();
	pns->setRtdSender(ps);
	pns->setRtdReceiver(pr);
	pns->setInputFile("..\\input.txt");
	pns->setOutputFile("..\\output.txt");

	pns->start();

	delete ps;
	delete pr;
	delete pUtils;									//ָ��Ψһ�Ĺ�����ʵ����ֻ��main��������ǰdelete
	delete pns;										//ָ��Ψһ��ģ�����绷����ʵ����ֻ��main��������ǰdelete

	return 0;


}

