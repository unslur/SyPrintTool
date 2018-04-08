#pragma once
#ifndef TSC_PRINTER_H
#define TSC_PRINTER_H
#include "Funcs.h"

typedef int (__stdcall *fTSC_Openport)(char*);
typedef int (__stdcall *fTSC_Closeport)();
typedef int (__stdcall *fTSC_Sendcommand)(char*);
typedef int (__stdcall *fTSC_Setup)(const char *, const char *, const char *, const char *, const char *, const char *, const char *);
typedef int (__stdcall *fTSC_Clearbuffer)();
typedef int (__stdcall *fTSC_Printlabel)(char *, char *);
typedef int (__stdcall *fTSC_Windowsfont)(int x, int y, int fontheight, int rotation, int fontstyle, int fontunderline, char *szFaceName, const char *content);


class CTscPrinter
{
public:
	CTscPrinter(void);
	~CTscPrinter(void);
	bool wineFlag;
    bool TscInit(char* printerName);
	bool TscUnInit();
	
	int Print_one(char* printerName,const string& printStr,LONG nPrintType);
	int Print_one_shit(char * printerName, const string & printStr, LONG nPrintType);
	int Print_two(const string& printStr, int spacebetween);
	int Print2(char* printerName,const string& printStr,LONG nPrintType);
	bool printN3(const string& printStr,int X, int Y,fTSC_Sendcommand TSC_DrawQR,fTSC_Windowsfont TSC_DrawText);
};

#endif
