#include "StdAfx.h"
#include ".\tscprinter.h"
#include "LabelAttributes.h"
HINSTANCE gt1Tsc;
fTSC_Openport TSC_Openport=NULL;
fTSC_Closeport TSC_Closeport=NULL;
fTSC_Sendcommand TSC_Sendcommand=NULL;
fTSC_Setup TSC_Setup=NULL;
fTSC_Clearbuffer TSC_Clearbuffer=NULL;
fTSC_Printlabel TSC_Printlabel=NULL;
fTSC_Windowsfont TSC_Windowsfont=NULL;
extern LabelAttributes label;
CTscPrinter::CTscPrinter(void)
{
	wineFlag = false;
}

CTscPrinter::~CTscPrinter(void)
{
}


int CTscPrinter::Print2(char * printerName, const string & printStr, LONG nPrintType)
{
	int rtn = 0;
	bool bIsPrint = false;
	char buff[0x100] = { 0 };
	string printStr1, printStr2;
	splitStrTwo(printStr, printStr1, printStr2);
	try {
		int ret = TSC_Setup(to_string(label.labelWidth).c_str(), to_string(label.labelHeight).c_str(), "3", "13", "0", "2", "0");
		ret = TSC_Clearbuffer();
		if (ret != 1) {
			sprintf(buff, "PTK_ClearBuffer:%d", ret);
			throw(buff);
		}
		TSC_Sendcommand("SET CUTTER OFF");
		TSC_Sendcommand("DIRECTION 0");
		char command1[200] = { 0 };
		//int X = 25,Y = 10;
		ret = Print_two(printStr1, 0);
		if (ret != 1) {
			sprintf(buff, "TSC_Printlabel:%d", ret);
			throw(buff);
		}
		if (printStr2.length() > 10) {
			ret = Print_two(printStr2, label.spacebetween);
			if (ret != 1) {
				sprintf(buff, "TSC_Printlabel:%d", ret);
				throw(buff);
			}
		
		}
		
		
		//==================================

		ret = TSC_Printlabel("1", "1");
		if (ret != 1) {
			sprintf(buff, "TSC_Printlabel:%d", ret);
			throw(buff);
		}

		//TSC_Closeport();
		rtn = 1;
	}
	catch (const exception& e) {
		MessageBox(NULL, e.what(), NULL, MB_OK);
		rtn = 0;
	}
	catch (...) {
		//sprintf(buff,"打印机错误:%d",GetLastError());
		MessageBox(NULL, buff, NULL, MB_OK);
		rtn = 0;
	}
	return rtn;
}

bool CTscPrinter::printN3(const string& printStr,int X, int Y,fTSC_Sendcommand TSC_DrawQR,fTSC_Windowsfont TSC_DrawText)
{
	char buff[0x100];
	list<string> printv = splitPrintStr(printStr);
	int y = Y-72,z = 0,ret = 0;
	char command1[200]={0};
	//MessageBox(NULL, printStr.c_str(), NULL, MB_OK);
	for ( list<string>::iterator iter = printv.begin();iter != printv.end(); ++iter ){	
		if (iter->length()<1)
		{continue;
		}
		if (z==0)
		{
						
						/*sprintf(xbuf,"%d",X-8);
						sprintf(ybuf,"%d",Y+8);
						qr_param = "QRCODE ";
						qr_param += xbuf;
						qr_param += ",";
						qr_param += ybuf;						
						qr_param += ",L,3,A,180,M2,S2,\"";											
						qr_param += (char *)iter->c_str();
						qr_param += "\"";
						ret = TSC_DrawQR((char *)qr_param.c_str());*/
				
				sprintf(command1,"QRCODE %d,%d,L,3,A,180,M2,S2,\"%s\"",X,Y,iter->c_str());
				ret = TSC_DrawQR(command1);
				if ( ret != 1 ){
					sprintf(buff,"TSC_DrawQR:%d",ret);
					throw(buff);
				}
				//memset(command1,0,100);
		}
		else
		{
			list<string> printCodes = splitPrintv(string(*iter),30);
			for ( list<string>::const_iterator citer = printCodes.begin(); citer != printCodes.end(); ++citer ){
				ret = TSC_DrawText(X,y,14,180,2,0,"宋体",(char *)citer->c_str());	
				if ( ret != 1 ){
					sprintf(buff,"TSC_DrawText:%d",ret);
					throw(buff);
				}
				y -= 30;
			}	
		}
		/*list<string> printCodes = splitPrintv(string(*iter),30);
		for ( list<string>::const_iterator citer = printCodes.begin(); citer != printCodes.end(); ++citer ){
			ret = TSC_DrawText(X,y,14,180,2,0,"宋体",(char *)citer->c_str());	
			if ( ret != 1 ){
				sprintf(buff,"TSC_DrawText:%d",ret);
				throw(buff);
			}
			y -= 30;
		}	*/
			
		
		
		z++;
	}
		//memset(command1,0,100);
	return true;
}

bool CTscPrinter::TscInit(char* printerName)
{
	gt1Tsc = LoadLibrary("TSCLIB.dll");
	if(!gt1Tsc){
		DWORD er = GetLastError();
		return false;
	}
	TSC_Openport = (fTSC_Openport)GetProcAddress(gt1Tsc,"openport");
	TSC_Closeport = (fTSC_Closeport)GetProcAddress(gt1Tsc,"closeport");
	TSC_Sendcommand = (fTSC_Sendcommand)GetProcAddress(gt1Tsc,"sendcommand");
	TSC_Setup = (fTSC_Setup)GetProcAddress(gt1Tsc,"setup");
	TSC_Clearbuffer = (fTSC_Clearbuffer)GetProcAddress(gt1Tsc,"clearbuffer");
	TSC_Printlabel = (fTSC_Printlabel)GetProcAddress(gt1Tsc,"printlabel");
	TSC_Windowsfont = (fTSC_Windowsfont)GetProcAddress(gt1Tsc,"windowsfont");
	if (!TSC_Openport||!TSC_Closeport||!TSC_Sendcommand||!TSC_Setup||!TSC_Clearbuffer||!TSC_Printlabel||!TSC_Windowsfont)
	{
		
		return false;
	}

	int ret = TSC_Openport(printerName);	
	if ( ret != 1 ){
		
		return false;
	}
	return true;
}
bool CTscPrinter::TscUnInit()
{
	TSC_Closeport();
    FreeLibrary(gt1Tsc);
	
	return true;
}


int CTscPrinter::Print_one(char* printerName,const string& printStr,LONG nPrintType)
{
	int rtn;
	bool bIsPrint = false;
	char buff[0x100]={0};
	
	vector<string> printv = splitPrintStrVector(printStr);
	
    try{
		int ret = TSC_Setup("40","30","3","13","0","2","2");
		ret = TSC_Clearbuffer();
		if ( ret != 1 ){
			sprintf(buff,"PTK_ClearBuffer:%d",ret);
			throw(buff);
		}
		TSC_Sendcommand("SET CUTTER OFF");
		TSC_Sendcommand("DIRECTION 0");
		char command1[200] = { 0 };
		//int X = 25,Y = 10;
		int X = 860,Y = 160;
		if (printv.size() < 1){
			sprintf(buff, "Print info is empty\n");
			throw(buff);
		}
		sprintf(command1, "QRCODE %d,%d,L,3,A,180,M2,S2,\"%s\"", int(label.listLocation[0].x), int(label.listLocation[0].y),printv[0].c_str() );
		ret = TSC_Sendcommand(command1);
		if (ret != 1) {
			sprintf(buff, "TSC_DrawQR:%d", ret);
			throw(buff);
		}
		for (size_t i = 1; i < printv.size(); i++)
		{


			
			ret = TSC_Windowsfont(int(label.listLocation[i].x), int(label.listLocation[i].y), int(label.listLocation[i].size), 180, 2, 0, "宋体", "龙泉驿山泉镇桃源村");
			if (ret != 1) {
				sprintf(buff, "TSC_DrawText: rank %d %d", i,ret);
				throw(buff);
			}
		}
		//==================================
		
		ret = TSC_Printlabel("1", "1");	
		if ( ret != 1 ){
			sprintf(buff,"TSC_Printlabel:%d",ret);
			throw(buff);
		}
		
		//TSC_Closeport();
		rtn = 1;
	}
	catch (const exception& e) {
		MessageBox(NULL, e.what(), NULL, MB_OK);
		rtn = 0;
	}
	catch (...) {
		//sprintf(buff,"打印机错误:%d",GetLastError());
		MessageBox(NULL, buff, NULL, MB_OK);
		rtn = 0;
	}
	//FreeLibrary(gt1);
	return rtn;
}
int CTscPrinter::Print_one_shit(char* printerName, const string& printStr, LONG nPrintType)
{
	int rtn;
	bool bIsPrint = false;
	char buff[0x100] = { 0 };

	vector<string> printv = splitPrintStrVector(printStr);

	try {
		int ret = TSC_Setup("60", "40", "3", "13", "0", "2", "2");
		ret = TSC_Clearbuffer();
		if (ret != 1) {
			sprintf(buff, "PTK_ClearBuffer:%d", ret);
			throw(buff);
		}
		TSC_Sendcommand("SET CUTTER OFF");
		TSC_Sendcommand("DIRECTION 0");
		char command1[200] = { 0 };
		//int X = 25,Y = 10;
		int X = 860, Y = 160;

		sprintf(command1, "QRCODE %d,%d,L,3,A,180,M2,S2,\"%s\"", int(label.listLocation[0].x) + int(label.originpoint[0]), int(label.listLocation[0].y) + int(label.originpoint[1]), printv[0].c_str());
		ret = TSC_Sendcommand(command1);
		if (ret != 1) {
			sprintf(buff, "TSC_DrawQR:%d", ret);
			throw(buff);
		}
		for (size_t i = 1; i < printv.size(); i++){
			ret = TSC_Windowsfont(int(label.listLocation[i].x)+int(label.originpoint[0]), int(label.listLocation[i].y)+int(label.originpoint[1]), int(label.listLocation[i].size), 180, 2, 0, "宋体", printv[i].c_str());
			if (ret != 1) {
				sprintf(buff, "TSC_DrawText: rank %d %d", i, ret);
				throw(buff);
			}
		}
		/*ret = TSC_Windowsfont(X, Y+10, 14, 180, 2, 0, "宋体", "桃类fdfdf质量");
		if (ret != 1) {
		sprintf(buff, "TSC_DrawText:%d", ret);
		throw(buff);
		}*/
		/*ret = TSC_Windowsfont(X, Y, 14, 180, 2, 0, "宋体", "追溯体系");
		if (ret != 1) {
		sprintf(buff, "TSC_DrawText:%d", ret);
		throw(buff);
		}*/
		//==================================

		ret = TSC_Printlabel("1", "1");
		if (ret != 1) {
			sprintf(buff, "TSC_Printlabel:%d", ret);
			throw(buff);
		}

		//TSC_Closeport();
		rtn = 1;
	}
	catch (const exception& e) {
		MessageBox(NULL, e.what(), NULL, MB_OK);
		rtn = 0;
	}
	catch (...) {
		//sprintf(buff,"打印机错误:%d",GetLastError());
		MessageBox(NULL, buff, NULL, MB_OK);
		rtn = 0;
	}
	//FreeLibrary(gt1);
	return rtn;
}



int CTscPrinter::Print_two(const string & printStr, int spacebetween)
{
	int ret = 0;
	vector<string> printv = splitPrintStrVector(printStr);
	char command1[200] = { 0 };
	char buff[0x100] = { 0 };
	if (printv.size() < 1) {
		sprintf(buff, "Print info is empty\n");
		throw(buff);
	}
	sprintf(command1, "QRCODE %d,%d,L,3,A,180,M2,S2,\"%s\"", int(label.listLocation[0].x)-spacebetween+int(label.originpoint[0]), int(label.listLocation[0].y)+ int(label.originpoint[1]), printv[0].c_str());
	ret = TSC_Sendcommand(command1);
	if (ret != 1) {
		sprintf(buff, "TSC_DrawQR:%d", ret);
		throw(buff);
	}
	
	
	for (size_t i = 1; i < printv.size(); i++)
	{
		if (i == 1) {
			printv[1] = printv[1].substr((printv[1].find("溯源码:") != -1 ? printv[1].find("溯源码:") + 7 : 0), printv[1].length());
		}
		ret = TSC_Windowsfont(int(label.listLocation[i].x)- spacebetween+ int(label.originpoint[0]), int(label.listLocation[i].y)+ int(label.originpoint[1]), int(label.listLocation[i].size), 180, 2, 0, "宋体", printv[i].c_str());
		if (ret != 1) {
			sprintf(buff, "TSC_DrawText: rank %d %d", i, ret);
			throw(buff);
		}
	}
	return ret;
}
