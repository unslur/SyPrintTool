#include "method.h"
//#include "zlib.h"


method::method()
{
}


method::~method()
{
}

const  string DefaultPrinter()
{
	TCHAR szBuffer[1024] = { 0 };
	DWORD length = 1024;
	int ret = ::GetDefaultPrinter(szBuffer, &length);
	
	return szBuffer;
}
//long method::print3(string pp, LPCTSTR printerNameT, LONG nPrintType){
//	string printStr = pp;
//	int rtn = 0;
//
//	char printerName[0x64] = { 0 };
//	strcpy(printerName, DefaultPrinter().c_str());
//
//	if (strstr(printerName, "POSTEK") != NULL)
//	{
//		CPskPrinter printer;
//		printer.PskInit(printerName);
//		rtn = printer.Print3(NULL, printStr, nPrintType);
//		printer.PskUnInit();
//	}
//	else if (strstr(printerName, "Printer LP") != NULL
//			|| strstr(printerName, "Printer B") != NULL||strstr(printerName,"TSC")!=NULL)
//		{
//			CTscPrinter printer;
//			printer.TscInit(printerName);
//			rtn = printer.Print3(NULL, printStr, nPrintType);
//			printer.TscUnInit();
//		}
//	else if (strstr(printerName, "ZDesigner") != NULL || strstr(printerName, "Microsoft") != NULL)
//	{
//		CZbraPrinter printer;
//		rtn = printer.PrintZbra3(printerName, printStr, nPrintType);
//	}
//	else
//	{
//		MessageBox(NULL, "请检查是否正确设置默认打印机！", NULL, MB_OK);
//	}
//	return rtn;
//}
long method::printone(string pp, LONG nPrintType) {
	string printStr = pp;
	int rtn = 0;

	char printerName[0x64] = { 0 };
	strcpy(printerName, DefaultPrinter().c_str());

	if (strstr(printerName, "POSTEK") != NULL)
	{
		CPskPrinter printer;
		printer.PskInit(printerName);
		rtn = printer.Print3(NULL, printStr, nPrintType);
		printer.PskUnInit();
	}
	else if (strstr(printerName, "Printer LP") != NULL
		|| strstr(printerName, "Printer B") != NULL || strstr(printerName, "TSC") != NULL)
	{
		CTscPrinter printer;
		printer.TscInit(printerName);
		rtn = printer.Print_one(NULL, printStr, nPrintType);
		printer.TscUnInit();
	}
	else if (strstr(printerName, "ZDesigner") != NULL || strstr(printerName, "Microsoft") != NULL)
	{
		CZbraPrinter printer;
		rtn = printer.PrintZbra3(printerName, printStr, nPrintType);
	}
	else
	{
		MessageBox(NULL, "请检查是否正确设置默认打印机！", NULL, MB_OK);
	}
	return rtn;
}
long method::printtwo(string pp, LONG nPrintType) {
	string printStr = pp;
	int rtn = 0;

	char printerName[0x64] = { 0 };
	strcpy(printerName, DefaultPrinter().c_str());

	if (strstr(printerName, "Printer LP") != NULL
		|| strstr(printerName, "Printer B") != NULL || strstr(printerName, "TSC") != NULL)
	{
		CTscPrinter printer;
		printer.TscInit(printerName);
		rtn = printer.Print2(NULL, printStr, nPrintType);
		printer.TscUnInit();
	}
	else
	{
		MessageBox(NULL, "请检查是否正确设置默认打印机！", NULL, MB_OK);
	}
	return rtn;
}
long method::printone_shit(string pp, LONG nPrintType) {
	string printStr = pp;
	int rtn = 0;

	char printerName[0x64] = { 0 };
	strcpy(printerName, DefaultPrinter().c_str());

	if (strstr(printerName, "Printer LP") != NULL
		|| strstr(printerName, "Printer B") != NULL || strstr(printerName, "TSC") != NULL)
	{
		CTscPrinter printer;
		printer.TscInit(printerName);
		rtn = printer.Print_one_shit(NULL, printStr, nPrintType);
		printer.TscUnInit();
	}
	else
	{
		MessageBox(NULL, "请检查是否正确设置默认打印机！", NULL, MB_OK);
	}
	return rtn;
}
