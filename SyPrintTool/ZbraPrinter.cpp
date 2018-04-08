#include "StdAfx.h"
#include ".\zbraprinter.h"
#include <winspool.h>
using namespace std;
CZbraPrinter::CZbraPrinter(void)
{
}

CZbraPrinter::~CZbraPrinter(void)
{
}
char* printNew2_Zbra(const string& printStr,int X, int Y,char* p){
	char buff[0x100];
	list<string> printv = splitPrintStr(printStr);
	int z = 0,y = 0;

	for ( list<string>::iterator iter = printv.begin();iter != printv.end(); ++iter ){
		if( 0 == z ){ //��ά��
			if (strstr(iter->c_str(),"http") != NULL)
			{
				p += sprintf(p,"^FT%d,%d^BQN,2,4^FDMA,%s^FS",X+124,Y+138,(char*)iter->c_str()); // 2�����ά�� 1������
			}
			else if (iter->length()<5)
			{
				break;
			}
			else
			{
				p += sprintf(p,"^FT%d,%d^BQN,2,5^FDMA,%s^FS",X+124,Y+149,(char*)iter->c_str()); // 2�����ά�� 3������				
			}
			
			
			y = Y + 162;
		}
		else if( 1 == z ){//������
			
			int x = X+6;
			list<string> printCodes = splitPrintv(string(*iter),28);
			for ( list<string>::const_iterator citer = printCodes.begin(); citer != printCodes.end(); ++citer ){
				p += sprintf(p,"^FT%d,%d^A2N,17,16^FH\^FD%s^FS",x,y,(char*)citer->c_str()); // 16,16 �����С A2N��17,16���ֱ��ʾ��ת�ȣ��߶ȣ���ȣ�����a��N��ʾѡ��0�ȣ�
				x += 50;
				y += 18;
			}
			y += 4;
			Y = y;
			*iter=iter->substr((iter->find( "��Դ��:")!=-1?iter->find("��Դ��:")+7:0),iter->length());
			//DelRecordByCode(iter->c_str(),DBPathName);
		}
		else if( 2 == z || 4 == z || 6 == z || 7 == z|| 3 == z || 5 == z ){ //Ʒ�� ���� ���� ����
			if (3 == z)
			{
				delchar((char*)iter->c_str(),' ');
			}
			if (iter->length()==0)
			{
				z++;
				continue;
			}	
			
			p += sprintf(p,"^FT%d,%d^A2N,17,16^FH\^FD%s^FS",X+6,y,(char*)iter->c_str());
			y += 22;
		}
		
		z++;
	}
	return p;
}
int CZbraPrinter::PrintZbra2(char* printerName,const string& printStr,LONG nPrintType){
	string printStr1, printStr2;
	bool bIsPrint = false;
	int id_name = 0,rtn = 0;
	char buff[0x400*10];
	char* p = buff,*printBuff = NULL;
	splitStrTwo(printStr, printStr1, printStr2);

	//AFX_MANAGE_STATE(AfxGetStaticModuleState());

	try{
		HANDLE     hPrinter;//��ӡ�����
		DWORD      dwBytesWritten;//�����ӡ��д�˶���
		//AfxMessageBox(printerName);
		if(!OpenPrinter( printerName, &hPrinter, NULL ) )
			throw("�򿪴�ӡ��ʧ��");

		DOC_INFO_1 Info;
		memset(&Info,0,sizeof(Info));
		Info.pDocName = "zyczs"; //��ӡ����������
		Info.pOutputFile = NULL;
		Info.pDatatype = "RAW";

		if(!StartDocPrinter( hPrinter, 1,(LPBYTE)&Info ))
			throw("���ĵ�ʧ��");
		if(!StartPagePrinter( hPrinter ))
			throw("��ҳʧ��");
		/*
		LLy   y��ʾ��ǩ����mm
		LHx,y   x,y��ʾ��ǩ��ʼλ��
		JUS �趨�����޸���ϣ���洢���趨
		MMT ��ӡģʽ ˺��
		MN ֽ����
		MTT ��תӡֽ
		LT ��ǩƫ�ƶ��� ���64������λ��
		CW ���弰����趨���˴�Ϊ 2 ��
		PMN ����ӡ����
		JMA 8��ÿ����
		JSN ����˳�򣬵��ڴ�ӡ������ģʽ
		*/

		strcpy(p,"CT~~CD,~CC^~CT~^XA~TA-020~JSN^LT0^MNW^MTT^PON^PMN^LH0,0^JMA^PR2,2~SD16^JUS^LRN^CI0^XZ^XA^POI^MMT^PW799^LL0360^LH0,32^LS0^CW2,E:MSYHBD.TTF^CI28");//
		p += strlen(p);

		int X = 80,Y = 23;
		if (printStr1.length() > 10 && (nPrintType == 1 || nPrintType == 3)){//��ӡ���
			p = printNew2_Zbra(printStr1,X,Y,p);			
		}
		if (printStr2.length() > 10 && (nPrintType == 2 || nPrintType == 3)){//��ӡ�м�
			p = printNew2_Zbra(printStr2,X+=363,Y,p);
		}
		strcpy(p,"^PQ1,0,1,Y^XZ");
		printBuff = Gb2312ToUtf8(buff);
		if (!WritePrinter( hPrinter, printBuff, strlen(printBuff), &dwBytesWritten ))
			throw("��ӡʧ��");
		if(!EndPagePrinter( hPrinter ))
			throw("����ҳʧ��");
		if(!EndDocPrinter( hPrinter ))
			throw("�����ĵ�ʧ��");
		if(!ClosePrinter( hPrinter ))
			throw("�رմ�ӡ��ʧ��");
		rtn = 1;
	}
	catch (const exception& e) {
		MessageBox(NULL, e.what(), NULL, MB_OK);
		rtn = 0;
	}
	catch (...) {
		sprintf(buff,"��ӡ������:%d",GetLastError());
		MessageBox(NULL, buff, NULL, MB_OK);
		rtn = 0;
	}
	delete [] printBuff;
	return rtn;
}

//char* printNew2_Zbra_1(const string& printStr,int X, int Y,char* p)
//{
//	char buff[0x100];
//	list<string> printv = splitPrintStr(printStr);
//	int z = 0,y = 0;
//	int numText=70+Y;
//	for ( list<string>::iterator iter = printv.begin();iter != printv.end(); ++iter ){
//		if( 0 == z ){ //��ά��
//			if (strstr(iter->c_str(),"http") != NULL)
//			{
//				p += sprintf(p,"^FT%d,%d^BQN,2,3^FDMA,%s^FS",X+170,Y+156,(char*)iter->c_str()); // 2�����ά�� 3������
//			}
//			else if (iter->length()<5)
//			{
//				break;
//			}
//			else
//			{
//				p += sprintf(p,"^FT%d,%d^BQN,2,4^FDMA,%s^FS",X+170,Y+176,(char*)iter->c_str()); // 2�����ά�� 4������				
//			}
//			
//		}
//		else if( 1 == z ){//������
//			int x = X+160;
//			int numY=160+Y;
//			
//			list<string> printCodes = splitPrintv(iter->substr(7,50),18);//18����ÿ18���ַ�Ϊһ��
//			for ( list<string>::const_iterator citer = printCodes.begin(); citer != printCodes.end(); ++citer ){
//				p += sprintf(p,"^FT%d,%d^A2N,14,12^FH\^FD%s^FS",x,numY,(char*)citer->c_str()); // 14,12 ����߶�����
//				//x += 75;
//				numY+=17;
//			}
//			
//			
//			DelRecordByCode(iter->substr(7).c_str(),DBPathName);
//		}
//		else  //Ʒ�� ���� ���� ����
//		{
//			if (iter->length()==0)
//			{
//				z++;
//				continue;
//			}
//			if(7 == z&&(iter->length()>23))
//			{	
//				//MessageBox(nullptr,iter->c_str(),nullptr,MB_OK);
//				delchar((char *)iter->c_str(),' ');
//				*iter=iter->substr((iter->find( "����:")!=-1?iter->find("����:")+5:0),iter->length());
//				list<string> printcomp = splitPrintv(*iter,22);
//				for ( list<string>::const_iterator citer = printcomp.begin(); citer != printcomp.end(); ++citer ){
//					p += sprintf(p,"^FT%d,%d^A2N,15,13^FH\^FD%s^FS",X+16,numText,(char*)citer->c_str());
//					
//					numText += 20;
//
//				}
//			}
//			else
//			{
//				p += sprintf(p,"^FT%d,%d^A2N,15,13^FH\^FD%s^FS",X+16,numText,(char*)iter->c_str());
//				numText += 20;
//			}
//			
//			
//			
//		}
//		z++;
//	}
//	return p;
//}
//
//
//int CZbraPrinter::PrintZbra2_1(char* printerName,const string& printStr,LONG nPrintType)
//
//{
//	
//	string printStr1, printStr2;
//	bool bIsPrint = false;
//	int id_name = 0,rtn = 0;
//	char buff[0x400*10];
//	char* p = buff,*printBuff = NULL;
//	splitStrTwo(printStr, printStr1, printStr2);
//
//	//AFX_MANAGE_STATE(AfxGetStaticModuleState());
//
//	try{
//		HANDLE     hPrinter;//��ӡ�����
//		DWORD      dwBytesWritten;//�����ӡ��д�˶���
//		//AfxMessageBox(printerName);
//		if(!OpenPrinter( printerName, &hPrinter, NULL ) )
//			throw("�򿪴�ӡ��ʧ��");
//
//		DOC_INFO_1 Info;
//		memset(&Info,0,sizeof(Info));
//		Info.pDocName = "zyczs"; //��ӡ����������
//		Info.pOutputFile = NULL;
//		Info.pDatatype = "RAW";
//
//		if(!StartDocPrinter( hPrinter, 1,(LPBYTE)&Info ))
//			throw("���ĵ�ʧ��");
//		if(!StartPagePrinter( hPrinter ))
//			throw("��ҳʧ��");
//
//		strcpy(p,"CT~~CD,~CC^~CT~^XA~TA-020~JSN^LT0^MNW^MTT^PON^PMN^LH0,0^JMA^PR2,2~SD16^JUS^LRN^CI0^XZ^XA^POI^MMT^PW799^LL0360^LH0,32^LS0^CW2,E:MSYHBD.TTF^CI28");
//		p += strlen(p);
//
//		int X = 80,Y = 23;
//		if (printStr1.length() > 10 && (nPrintType == 1 || nPrintType == 3)){//��ӡ���
//			p = printNew2_Zbra_1(printStr1,X,Y,p);			
//		}
//		if (printStr2.length() > 10 && (nPrintType == 2 || nPrintType == 3)){//��ӡ�м�
//			p = printNew2_Zbra_1(printStr2,X+=363,Y,p);
//		}
//		strcpy(p,"^PQ1,0,1,Y^XZ");
//		printBuff = Gb2312ToUtf8(buff);
//		if (!WritePrinter( hPrinter, printBuff, strlen(printBuff), &dwBytesWritten ))
//			throw("��ӡʧ��");
//		if(!EndPagePrinter( hPrinter ))
//			throw("����ҳʧ��");
//		if(!EndDocPrinter( hPrinter ))
//			throw("�����ĵ�ʧ��");
//		if(!ClosePrinter( hPrinter ))
//			throw("�رմ�ӡ��ʧ��");
//		rtn = 1;
//	}
//	catch (const exception& e) {
//		MessageBox(NULL, e.what(), NULL, MB_OK);
//		rtn = 0;
//	}
//	catch (...) {
//		sprintf(buff,"��ӡ������:%d",GetLastError());
//		MessageBox(NULL, buff, NULL, MB_OK);
//		rtn = 0;
//	}
//	delete [] printBuff;
//	return rtn;
//}
//
//
//
char* printNew3_Zbra(const string& printStr,int X, int Y,char* p)
{
	char buff[0x100];
	list<string> printv = splitPrintStr(printStr);
	int z = 0,y = 0;
	bool flag = false;
	int division=12;
	int offset_X=0,offset_Y=0;
	for ( list<string>::iterator iter = printv.begin();iter != printv.end(); ++iter ){
		if( 0 == z ){ //��ά��	
			/*//��˾��ҩ����Ŀ������
			p += sprintf(p,"^FT%d,%d^BQN,2,3^FDMA,%s^FS",X+130,Y+90,(char*)iter->c_str()); // 2�����ά�� 3������
			*/
			p += sprintf(p,"^FT%d,%d^BQN,2,3^FDMA,%s^FS",X,Y+90,(char*)iter->c_str()); // 2�����ά�� 1������
			y = Y + 100;
		}
		//else if( 1 == z ){//������
		//	*iter=iter->substr((iter->find( "��Դ��:")!=-1?iter->find("��Դ��:")+7:0),iter->length());
		//	
		//	if (iter->c_str()[0]=='8')
		//	{
		//		division=20;
		//		offset_X=-100;offset_Y=32;
		//	}
		//	
		//	list<string> printCodes = splitPrintv(string(*iter),division);
		//	for ( list<string>::const_iterator citer = printCodes.begin(); citer != printCodes.end(); ++citer ){
		//		p += sprintf(p,"^FT%d,%d^A2N,14,13^FH\^FD%s^FS",X+130+offset_X,y,(char*)citer->c_str()); // 16,16 �����С
		//		y += 16;
		//	}
		//	DelRecordByCode(iter->c_str(),DBPathName);
		//	if (iter->c_str()[0]=='6')
		//	{
		//		offset_Y=-16;

		//	}
		//	y = Y + 84+offset_Y;
		//}
		else{//��������
			///*if (2 == z)
			//{
			//	delchar((char*)iter->c_str(),' ');				
			//}*/
			//if (iter->length()==0)
			//{
			//	z++;
			//	continue;
			//}
			
				p += sprintf(p,"^FT%d,%d^A2N,15,15^FH\^FD%s^FS",X,y,(char*)iter->c_str());
			y += 16;
		}
		z++;
	}
	return p;
}

int CZbraPrinter::PrintZbra3(char* printerName,const string& printStr,LONG nPrintType)
{
	string printStr1 = "", printStr2 = "", printStr3 = "";

	char buff[0x400*10]={0};
	char* p = buff,*printBuff = NULL;
	int rtn = 0;
	splitStrThree(printStr, printStr1, printStr2, printStr3);
    

	//AFX_MANAGE_STATE(AfxGetStaticModuleState());

	try{
		//--------------------------------------------------
		HANDLE     hPrinter;//��ӡ�����
		DWORD      dwBytesWritten;//�����ӡ��д�˶���
		//AfxMessageBox(printerName);
		if(!OpenPrinter( printerName, &hPrinter, NULL ) )
			throw("�򿪴�ӡ��ʧ��");


		DOC_INFO_1 Info;
		memset(&Info,0,sizeof(Info));
		Info.pDocName = "zyczs"; //��ӡ����������
		Info.pOutputFile = NULL;
		Info.pDatatype = "RAW";
		
		if(!StartDocPrinter( hPrinter, 1,(LPBYTE)&Info ))
			throw("���ĵ�ʧ��");
		
		if(!StartPagePrinter( hPrinter ))
			throw("��ҳʧ��");
		/*//��˾��ҩ����Ŀ������
		strcpy(p,"CT~~CD,~CC^~CT~^XA~TA000~JSN^LT0^MNW^MTT^PON^PMN^LH0,0^JMA^PR4,4~SD20^JUS^LRN^CI0^XZ^XA^POI^MMT^PW799^LL0360^LH0,32^LS0^CW2,E:MSYHBD.TTF^CI28");
		*/
		/*
		isale����
		
		*/
		strcpy(p,"CT~~CD,~CC^~CT~^XA~TA000~JSN^LT0^MNW^MTT^PON^PMN^LH0,0^JMA^PR4,4~SD20^JUS^LRN^CI0^XZ^XA^MMT^PW799^LL0360^LH0,32^LS0^CW2,E:MSYHBD.TTF^CI28");
		p += strlen(p);

		int X = 22,Y = 18;
		if (printStr1.length() > 10 && (nPrintType == 1 || nPrintType == 4)){//��ӡ���
			p = printNew3_Zbra(printStr1,X,Y,p);			
		}

		if (printStr2.length() > 10 && (nPrintType == 2 || nPrintType == 4)){//��ӡ�м�
			p = printNew3_Zbra(printStr2,X+=260,Y,p);
		}
		if (printStr3.length() > 10 && (nPrintType == 3 || nPrintType == 4)){//��ӡ�ұ�
			p = printNew3_Zbra(printStr3,X+=260,Y,p);
		}
		strcpy(p,"^PQ1,0,1,Y^XZ");
		printBuff = Gb2312ToUtf8(buff);
		if (!WritePrinter( hPrinter, printBuff, strlen(printBuff), &dwBytesWritten ))
			throw("��ӡʧ��");
		
		if(!EndPagePrinter( hPrinter ))
			throw("����ҳʧ��");
		if(!EndDocPrinter( hPrinter ))
			throw("�����ĵ�ʧ��");
		if(!ClosePrinter( hPrinter ))
			throw("�رմ�ӡ��ʧ��");
		rtn = 1;
	}
	catch (const exception& e) {
		MessageBox(NULL, e.what(), NULL, MB_OK);
		rtn = 0;
	}
	catch (...) {
		sprintf(buff,"��ӡ������:%d",GetLastError());
		MessageBox(NULL, buff, NULL, MB_OK);
		
		rtn = 0;
	}
	//delete [] printBuff;
	return rtn;
}//*/