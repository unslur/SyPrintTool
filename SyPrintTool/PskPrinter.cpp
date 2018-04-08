#include "StdAfx.h"
#include ".\pskprinter.h"

/////////////////////////////////
typedef int (__stdcall *fOpenPort)(char* PortFlag);
typedef int (__stdcall *fPTK_ClearBuffer)();
typedef int (__stdcall *fPTK_SetLabelHeight)
(unsigned int lheight, unsigned int gapH,int gapOffset,BOOL bBlag);
typedef int (__stdcall *fPTK_SetLabelWidth)(unsigned int lwidth);
typedef int (__stdcall *fPTK_SetDarkness)(unsigned  int id);
typedef int (__stdcall *fPTK_SetPrintSpeed)(unsigned int px);
typedef int (__stdcall *fPTK_DrawBar2D_QR)(unsigned int x, unsigned int y,
										   unsigned int w, unsigned int v,
										   unsigned int o, unsigned int r,
										   unsigned int m, unsigned int g,
										   unsigned int s, char* pstr);
typedef int (__stdcall *fPTK_DrawBar2D_QREx) ( unsigned int x, unsigned int y, unsigned int o,unsigned int r, unsigned int g,unsigned int v, unsigned int s,LPTSTR id_name,LPTSTR pstr );

typedef int (__stdcall *fPTK_DrawText)(unsigned int px, unsigned int py,
									   unsigned int pdirec, unsigned int pFont,
									   unsigned int pHorizontal, unsigned int pVertical,
									   char ptext, char* pstr);


typedef int (__stdcall *fPTK_DrawTextTrueTypeW)
		                            (int x,int y,int FHeight,
									int FWidth,LPCTSTR FType,
                                    int Fspin,int FWeight,
									BOOL FItalic,BOOL FUnline,
                                    BOOL FStrikeOut,
									LPCTSTR id_name,
									LPCTSTR data);

typedef int (__stdcall *fClosePort)(void);
typedef int (__stdcall *fPTK_PrintLabel)(unsigned int number,
										 unsigned int cpnumber);
typedef int (__stdcall *fPTK_setDirection)(TCHAR direct);
typedef int (__stdcall *fPTK_setCoordinateOrigin)(int x,int y);

////////////////////////////////////////////////////
HINSTANCE gt1Psk;
fOpenPort OpenPort = NULL;
fPTK_ClearBuffer PTK_ClearBuffer = NULL;
fPTK_SetLabelHeight PTK_SetLabelHeight = NULL;
fPTK_SetLabelWidth PTK_SetLabelWidth = NULL;
fPTK_SetDarkness PTK_SetDarkness = NULL;
fPTK_SetPrintSpeed PTK_SetPrintSpeed = NULL;
fPTK_DrawBar2D_QR PTK_DrawBar2D_QR = NULL;
fPTK_DrawText PTK_DrawText = NULL;
fPTK_DrawTextTrueTypeW PTK_DrawTextTrueTypeW = NULL;
fClosePort ClosePort = NULL;
fPTK_PrintLabel PTK_PrintLabel = NULL;
fPTK_DrawBar2D_QREx PTK_DrawBar2D_QREx=NULL;
fPTK_setDirection PTK_setDirection=NULL; 
fPTK_setCoordinateOrigin PTK_setCoordinateOrigin=NULL;
OSVERSIONINFO os;
 int sizeOffset3=0;
 int sizeOffset=0;
 int sizeOffset_1=0;
///////////////////////////////

CPskPrinter::CPskPrinter(void)
{
}

CPskPrinter::~CPskPrinter(void)
{
}


bool printN2(const string& printStr,int X, int Y,int& id_name,fPTK_DrawBar2D_QR PTK_DrawBar2D_QR,fPTK_DrawTextTrueTypeW PTK_DrawTextTrueTypeW)
{
	char buff[0x100];
	list<string> printv = splitPrintStr(printStr);
	int y = Y,ret = 0,z=0;
	char numBuff[10]={"dda"};
	static int num=1;
	for ( list<string>::iterator iter = printv.begin();iter != printv.end(); ++iter ){
		if (iter->length()==0)
			{
				continue;
			}
		if (z==0)
		{
			sprintf(numBuff,"484%d",num++);

			
				ret = PTK_DrawBar2D_QREx(X+37,Y+70,0,3,0,0,3,(char *)numBuff,(char*)iter->c_str());//字符串长度限制导致无法正确扫描二维码，故选取该API，
				num=num%39;
		
				
				

			if ( ret != 0 ){
				sprintf(buff,"PTK_DrawBar2D_QR:%d",ret);
				throw(buff);
			}

			
		}else{
			list<string> printCodes = splitPrintv(string(*iter),30);
			for ( list<string>::const_iterator citer = printCodes.begin(); citer != printCodes.end(); ++citer ){
				/*char buf[10]={0};
				itoa(y,buf,10);
				MessageBox(NULL, buf, NULL, MB_OK);*/
				ret = PTK_DrawTextTrueTypeW(X+120, y+200, 12,0,"宋体", 1, 600, 0, 0, 0, itoa(id_name++,buff,10), (char*)citer->c_str()); //x-100
				if ( ret != 0 ){
					sprintf(buff,"PTK_DrawTextTrueTypeW:%d",ret);
					throw(buff);
				}
				//y += 25;
			}	
		}

		
		z++;
	}
	return true;
}
bool CPskPrinter::PskInit(char* printerName)
{
	gt1Psk = LoadLibrary("WINPSK.dll");
	OpenPort = (fOpenPort)GetProcAddress(gt1Psk,"OpenPort");
	ClosePort = (fClosePort)GetProcAddress(gt1Psk,"ClosePort");
	PTK_DrawBar2D_QR = (fPTK_DrawBar2D_QR)GetProcAddress(gt1Psk,"PTK_DrawBar2D_QR");
	PTK_SetLabelHeight = (fPTK_SetLabelHeight)GetProcAddress(gt1Psk,"PTK_SetLabelHeight");
	PTK_PrintLabel = (fPTK_PrintLabel)GetProcAddress(gt1Psk,"PTK_PrintLabel");
	PTK_SetDarkness = (fPTK_SetDarkness)GetProcAddress(gt1Psk,"PTK_SetDarkness");
	PTK_SetPrintSpeed = (fPTK_SetPrintSpeed)GetProcAddress(gt1Psk,"PTK_SetPrintSpeed");
	PTK_SetLabelWidth = (fPTK_SetLabelWidth)GetProcAddress(gt1Psk,"PTK_SetLabelWidth");
	PTK_ClearBuffer = (fPTK_ClearBuffer)GetProcAddress(gt1Psk,"PTK_ClearBuffer");
	PTK_DrawText = (fPTK_DrawText)GetProcAddress(gt1Psk,"PTK_DrawText");
	PTK_DrawTextTrueTypeW = (fPTK_DrawTextTrueTypeW)GetProcAddress(gt1Psk,"PTK_DrawTextTrueTypeW");
	PTK_DrawBar2D_QREx=(fPTK_DrawBar2D_QREx)GetProcAddress(gt1Psk,"PTK_DrawBar2D_QREx");
	PTK_setDirection=(fPTK_setDirection)GetProcAddress(gt1Psk,"PTK_SetDirection");
	PTK_setCoordinateOrigin=(fPTK_setCoordinateOrigin)GetProcAddress(gt1Psk,"PTK_SetCoordinateOrigin");
		int ret = OpenPort(printerName);	
		if ( ret != 0 ){
			
			char buff[64]={0};
			sprintf(buff,"OpenPort:%d",ret);
			//throw(buff);
			//MessageBox(NULL, buff, NULL, MB_OK);
			return false;
		}
	
		return true;
}
bool CPskPrinter::PskUnInit()
{
	try
	{
		ClosePort();

		FreeLibrary(gt1Psk);
	}
	catch (const std::exception&)
	{
		MessageBox(NULL, "2333", NULL, MB_OK);
	}
	
	
	
	
	return true;
}
bool printNewWhite_psk(const string& printStr,int X, int Y,int& id_name,fPTK_DrawBar2D_QR PTK_DrawBar2D_QR,fPTK_DrawTextTrueTypeW PTK_DrawTextTrueTypeW)
{
	char buff[0x100];
	list<string> printv = splitPrintStr(printStr);
	int y = Y,ret = 0;
	int z=0,num=0;
	char numBuff[20]={0};
	for ( list<string>::iterator iter = printv.begin();iter != printv.end(); ++iter ){
		if (z==0)
		{
			sprintf(numBuff,"484%d",num++);
			ret = PTK_DrawBar2D_QREx(X+40,Y,0,3,0,0,3,(char *)numBuff,(char*)iter->c_str());//字符串长度限制导致无法正确扫描二维码，故选取该API，
			num=num%39;
			if ( ret != 0 ){
				sprintf(buff,"PTK_DrawBar2D_QR:%d",ret);
				throw(buff);
			}
		}else
		{
			ret = PTK_DrawTextTrueTypeW(X+140, y, 17,0,"宋体", 1, 600, 0, 0, 0, itoa(id_name++,buff,10), (char*)iter->c_str()); //x-100
			if ( ret != 0 ){
				sprintf(buff,"PTK_DrawTextTrueTypeW:%d",ret);
				throw(buff);
			}y += 22;
		}
		z++;
				 
	}
	return true;
}
int CPskPrinter::Print3(char* printerName,const string& printStr,LONG nPrintType)
{
	int rtn,id_name = 0;
	bool bIsPrint = false;
	char buff[0x100]={0};
	
	string printStr1, printStr2,printStr3;
	splitStrThree(printStr, printStr1, printStr2, printStr3);

	//AFX_MANAGE_STATE(AfxGetStaticModuleState());

	
	try{
		int ret = PTK_ClearBuffer();
		if ( ret != 0 ){
			sprintf(buff,"PTK_ClearBuffer:%d",ret);
			throw(buff);
		}
		ret = PTK_SetLabelHeight(200, 24,0,false);
		if ( ret != 0 ){
			sprintf(buff,"PTK_SetLabelHeight:%d",ret);
			throw(buff);
		}
		ret = PTK_SetLabelWidth(832);
		if ( ret != 0 ){
			sprintf(buff,"PTK_SetLabelWidth:%d",ret);
			throw(buff);
		}
		ret = PTK_SetDarkness(13);
		if ( ret != 0 ){
			sprintf(buff,"PTK_SetDarkness:%d",ret);
			throw(buff);
		}
		ret = PTK_SetPrintSpeed(5);
		if ( ret != 0 ){
			sprintf(buff,"PTK_SetPrintSpeed:%d",ret);
			throw(buff);
		}
		ret=PTK_setDirection('B');
		if ( ret != 0 ){
			sprintf(buff,"PTK_setDirection:%d",ret);
			throw(buff);
		}
		//int X = 42,Y = 10;//old
		int X = 20,Y = 35;//new
		//sprintf(numBuff,"%d",num++)
		//打印左边
		
		if (printStr1.length() > 0 && (nPrintType == 1 || nPrintType == 4)){
			bIsPrint |= printNewWhite_psk(printStr1,X,Y,id_name,PTK_DrawBar2D_QR,PTK_DrawTextTrueTypeW);
		}

		//打印中间
		if (printStr2.length() > 0 && (nPrintType == 2 || nPrintType == 4)){
			bIsPrint |= printNewWhite_psk(printStr2,X+=265,Y,id_name,PTK_DrawBar2D_QR,PTK_DrawTextTrueTypeW);
		}

		//打印右边
		if (printStr3.length() > 0 && (nPrintType == 3 || nPrintType == 4)){
			bIsPrint |= printNewWhite_psk(printStr3,X+=265,Y,id_name,PTK_DrawBar2D_QR,PTK_DrawTextTrueTypeW);
		}
		//==================================
		if (bIsPrint){
			ret = PTK_PrintLabel(1, 1);	
			if ( ret != 0 ){
				sprintf(buff,"PTK_PrintLabel:%d",ret);
				throw(buff);
			}
		}
	
		rtn = 1;
	}
	catch (const exception& e) {
		//AfxMessageBox(e.what());
		rtn = 0;
	}
	catch (...) {
		sprintf(buff,"打印机错误:%d",GetLastError());
		//AfxMessageBox(buff);
		rtn = 0;
	}
	
	return rtn;
}
int CPskPrinter::Print2(char* printerName,const string& printStr,LONG nPrintType)
{
	int rtn,id_name = 0;
	bool bIsPrint = false;
	char buff[0x100]={0};
	
	string printStr1, printStr2,printStr3;
	splitStrThree(printStr, printStr1, printStr2, printStr3);

	//AFX_MANAGE_STATE(AfxGetStaticModuleState());

	
	try{
		int ret = PTK_ClearBuffer();
		if ( ret != 0 ){
			sprintf(buff,"PTK_ClearBuffer:%d",ret);
			throw(buff);
		}
		ret = PTK_SetLabelHeight(240, 16,0,false);
		if ( ret != 0 ){
			sprintf(buff,"PTK_SetLabelHeight:%d",ret);
			throw(buff);
		}
		ret = PTK_SetLabelWidth(832);
		if ( ret != 0 ){
			sprintf(buff,"PTK_SetLabelWidth:%d",ret);
			throw(buff);
		}
		ret = PTK_SetDarkness(13);
		if ( ret != 0 ){
			sprintf(buff,"PTK_SetDarkness:%d",ret);
			throw(buff);
		}
		ret = PTK_SetPrintSpeed(5);
		if ( ret != 0 ){
			sprintf(buff,"PTK_SetPrintSpeed:%d",ret);
			throw(buff);
		}

		//int X = 42,Y = 10;//old
		int X = 68,Y = 10;//new37
		
		
		if (printStr1.length() > 0 && (nPrintType == 1 || nPrintType == 3)){
			bIsPrint |= printN2(printStr1,X,Y,id_name,PTK_DrawBar2D_QR,PTK_DrawTextTrueTypeW);
		}

		
		if (printStr2.length() > 0 && (nPrintType == 2 || nPrintType == 3)){
			bIsPrint |= printN2(printStr2,X+=340,Y,id_name,PTK_DrawBar2D_QR,PTK_DrawTextTrueTypeW);
		}

		
		//==================================
		if (bIsPrint){
			ret = PTK_PrintLabel(1, 1);	
			if ( ret != 0 ){
				sprintf(buff,"PTK_PrintLabel:%d",ret);
				throw(buff);
			}
		}
	
		rtn = 1;
	}
	catch (const exception& e) {
		//AfxMessageBox(e.what());
		rtn = 0;
	}
	catch (...) {
		sprintf(buff,"打印机错误:%d",GetLastError());
		//AfxMessageBox(buff);
		rtn = 0;
	}
	
	return rtn;
}
