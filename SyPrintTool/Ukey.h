#pragma once
#include "stdafx.h"
typedef bool (__stdcall *fOpenUkey)(char * passwd); 
typedef bool (__stdcall *fReadS)(char* readData,int num);
typedef bool (__stdcall *fWriteS)(char* writeData,int num);
typedef bool (__stdcall *fCloseUkey)(void); 
typedef bool (__stdcall *fChangePwd)(char* oldPwd,char* newPwd);
typedef bool (__stdcall *fGetSerial)(char * serial); 
class Ukey
{
public:
	Ukey(void);
	~Ukey(void);
	string init(void);
	string read(int num);
	string write(string str);
private:
	fOpenUkey Uopen;
	fReadS Uread;
	fWriteS Uwrite;
	fCloseUkey Uclose;
	fChangePwd Uchange;
	fGetSerial uSerial;
};

