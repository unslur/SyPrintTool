#pragma once
#include "stdafx.h"
class method
{
public:
	method();
	~method();
	long printNew(string pp, LPCTSTR printerName, LONG nPrintType);
	long printNew_1(string pp, LPCTSTR printerName, LONG nPrintType);
	long printNew3(string pp, LPCTSTR printerName, LONG nPrintType);
	LONG print2(LPCTSTR pp);
	string ReadQR(LPCTSTR com, LONG baudRate, LONG overTime);
	string ReadQRNew();
	string DownloadData(LPCTSTR inVar);
	string GetNumber(LPCTSTR inVar);
	string ReadRFID2(LONG overTime);
	string WriteRFID2(LPCTSTR codeStr, LONG mode);
	bool JudgeDefaultType(void);
	bool JudgeOldType(void);
	bool ModifyPass(void);
	string EncryptNoUseBlock();
	BSTR ULogin(void);
	BOOL initUsb();
	string UkeyGetDownData(IN const char* sendCommand, IN const char* ip, IN int port, OUT unsigned char** buffer, OUT size_t& bufferLen) const;
	string SentUsbIdToServer(string& taskIP, int taskPort, char* usbId);
	//LONG printWine3(LPCTSTR http, LPCTSTR code, LPCTSTR pp, LONG nPrintType);
	long printtag2(LPCTSTR pp, LPCTSTR printerName, LONG nPrintType);
	string readUkey(int num);
	string writeUkey(string data);
	long print3(string pp, LPCTSTR printerName, LONG nPrintType);
	long printone(string pp, LONG nPrintType);
	long printtwo(string pp, LONG nPrintType);
	long printone_shit(string pp, LONG nPrintType);
};

