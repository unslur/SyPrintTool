

#pragma comment(lib,"comsupp.lib") 


#include "stdafx.h"
#include <string>
#include <iostream>
#include <windows.h>
#include "LabelAttributes.h"
using namespace std;

//#define __MYDEBUG__




LabelAttributes label;
string function(vector<string> RecvStr);
bool sendMessage(string message);
string recvMessaage();
bool vaildIn=false;
std::string msg ="";//���ܵ��ַ���
DWORD WINAPI ThreadFun(LPVOID pM)  
{ 
	std::string msg_pre=msg;
	while (true)
	{
		Sleep(1000*60);
		if (!msg_pre.compare(msg))
		{
			exit(0);
		}
		msg_pre=msg;
	} 
	
	return 0;  
} 
int main()
{
#ifdef _DEBUG
	if (!label.TSCLoadConfiguration()) {
		return -1;
	};
#endif _DEBUG
	
	HANDLE handle = CreateThread(NULL, 0, ThreadFun, NULL, 0, NULL); 
	/*std::ifstream fin("file.txt", std::ios::in);
	string readline;
	while (getline(fin,readline))
	{
		msg = "peach#printone#" + readline + "#1";*/
	//}
	while (1)
	{
		//MessageBox(NULL, "s", NULL, MB_OK);
		
#ifndef _DEBUG
		//MessageBox(NULL, "ssss", NULL, MB_OK);
		msg = recvMessaage();  
#endif // !__MYDEBUG__
		vaildIn=true;
		
#ifdef _DEBUG
		//msg="#Listen#";
		msg = "shit#printone#http://localhost\\n���ƣ�����\\n������10kg\\n���һ��һ��#4#";
		//msg = "peach#printtwo#http://localhost\\n���ƣ�����\\n������10kg\\n���һ��һ��|#4#";
		//msg = "peach#printone#http://www.zhongline.com:8080/med/receive/search?code=20015063014356699905353129#3";
		//msg="#peach#print#http://www.zhongline.com:8080/med/receive/search?code=20015063014356699905353129\\n5234567114848\\nstring\\n�ַ���\\n|http://www.zhongline.com:8080/med/receive/search?code=20015063014356699905353129\\n56699905353129\\n|http://www.zhongline.com:8080/med/receive/search?code=20015063014356699905353129\\n56699905353129\\n#4#";
		//msg = "#DownloadData#ea1965c52c6bbd1b885ac666da8e8e68 153169228 1459308128428604 192.168.9.227 15554#";  
		//msg="#printWhite#Ʒ��:�����õĺǺǺǺ�ʦ������ѧ�翴���ؿ�����\\n�ջ���:���ĵ�Ϊ���Ŷ�Ҿ�㵵�Ŷ��λŶ\\n����:100\\n����:2016-1-26\\n|Ʒ��:����\\n�ջ���:����\\n����:100\\n����:2016-1-26\\n|Ʒ��:����\\n�ջ���:����\\n����:100\\n����:2016-1-26\\n#4#";
		//msg = "#printWine#http://182.148.107.188:18082/jiusy/time/index.html\nƷ��:ʱ���ζ��\n���:2016��\n����:����\n����:Ũ����\n�ȼ�:�ż�\n��Դ��:25417852145632145214\n|http://182.148.107.188:18082/jiusy/time/index.html\nƷ��:ʱ���ζ��\n���:2016��\n����:����\n����:Ũ����\n�ȼ�:�ż�\n��Դ��:25417852145632145214\n#3#";  
		//msg="#GetNumber#1165 2081457406245301128 100#";
		//msg="#printNew2#http://cdcspt.zyczs.gov.cn/zycqt/receive/search?code=51012730061805200001915090100000047\\n��Դ��:82345678908234567890123456789011\\nƷ ��:����\\n��    ��:20g\\n�� ��:������ɽ��\\n��������:2012.09.18\\n�� ��:887966530187G\\n����:���������ҩ�����޺���ҩ�����޺���ҩ������\\n|*CPC*ZYCZS12345678901234567890123456789022\\n��Դ��:82345678908234567890123456789022\\nƷ ��:����\\n��    ��:20g\\n�� ��:������ɽ��\\n��������:2012.09.18\\n�� ��:887966530187G\\n�� ��:���������ҩ�����޹�˾\\n#3#";
#endif // __MYDEBUG__

		
		//MessageBox(NULL, msg.c_str(), NULL, MB_OK);
		std::string message = "{\"text\":\"297\"}";
#ifndef _DEBUG
		msg = U2G(msg.c_str());
#endif _DEBUG
		
		//MessageBox(NULL, msg.c_str(), NULL, MB_OK);
		if (msg.find("#stop#") != string::npos)
		{
			break;
		}
		if (msg.find("Listen") != string::npos)
		{
			/*try {
				label.LoadConfiguration();
			}
			catch (...) {

			}*/
			if (!label.TSCLoadConfiguration()) {
				return -1;
			};
			sendMessage( "{\"text\":\"296\"}");
			continue;
		}
		
		//MessageBox(NULL, msg.c_str(), NULL, MB_OK);
		//////�����Ϊ!��̾��
		vector<string> receiveStr = VsplitPrintStr2(msg);
		message = function(receiveStr);
		
		if (!sendMessage(message))
		{
			MessageBox(NULL, message.c_str(), NULL, MB_OK);
		}
			
#ifdef _DEBUG
		break;
#endif _DEBUG

		
	}
    return 0;
}

string function(vector<string> RecvStr) {
	try
	{
	int rtn = 0;
	method fun;
	//MessageBox(NULL, RecvStr[0].c_str(), NULL, MB_OK);
	if (RecvStr[0].find("peach") != string::npos)//��Ŀ���� ��Ȫˮ����
	{
		if (RecvStr[1]=="printone")
		{
			rtn = fun.printone(RecvStr[2].c_str(), atoi(RecvStr[3].c_str()));
			if (rtn < 1)
			{
				return  "{\"text\":\"317\"}";//��ӡʧ��
			}
		}
		else if (RecvStr[1] == "printtwo")
		{
			rtn = fun.printtwo(RecvStr[2], atoi(RecvStr[3].c_str()));
			if (rtn < 1)
			{
				return  "{\"text\":\"317\"}";//��ӡʧ��
			}
		}
		
	}
	
	else if (RecvStr[0].find("shit") != string::npos)//��Ŀ���� ��Ƶ�����Դ
	{
		//MessageBox(NULL, RecvStr[1].c_str(), NULL, MB_OK);
		if (RecvStr[1] == "printone")
		{
			rtn = fun.printone_shit(RecvStr[2], atoi(RecvStr[3].c_str()));
			if (rtn < 1)
			{
				return  "{\"text\":\"317\"}";//��ӡʧ��
			}
		}

	}
	else
	{

		return  "{\"text\":\"300\"}";
	}

	}
	catch (const std::exception&e)
	{
		MessageBox(NULL, e.what(), NULL, MB_OK);
		return "{\"text\":\"315\"}";//�쳣
	}
	return "{\"text\":\"297\"}";//��ӡ�ɹ�

}



bool sendMessage(string message) {
	
	message = Gb2312ToUtf8(message.c_str());
	bool flag = false;
	UINT len = message.length();
	std::cout << char(len >> 0)
		<< char(len >> 8)
		<< char(len >> 16)
		<< char(len >> 24);
	std::cout << message << std::flush;
#ifdef __MYDEBUG__
	getchar();  
#endif // __MYDEBUG__

	 flag=true;
	 return flag;
}
string recvMessaage() {
	std::string msg = "";//���ܵ��ַ���
	unsigned int length = 0;
	for (int i = 0; i < 4; i++) {//ǰ�ĸ��ֽ�Ϊ֧�������ȿ���
		unsigned int read_char = getchar();
		length = length | (read_char << i * 8);
	}

	for (int i = 0; i < length; i++) {
		msg += getchar();
	}
	return msg;
}
//////////////////////////






