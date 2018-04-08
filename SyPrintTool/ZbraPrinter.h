#pragma once
#ifndef ZBRA_PRINTER_H
#define ZBRA_PRINTER_H
#include "Funcs.h"
class CZbraPrinter
{
public:
	CZbraPrinter(void);
	~CZbraPrinter(void);
	
	int PrintZbra2(char* printerName,const string& printStr,LONG nPrintType);
	int PrintZbra3(char* printerName,const string& printStr,LONG nPrintType);
};

#endif
