#include "Ukey.h"


Ukey::Ukey(void)
{
	Uread=nullptr;
	Uopen=nullptr;
	Uclose=nullptr;
	Uwrite=nullptr;
	Uchange=nullptr;
	uSerial=nullptr;
	
}


Ukey::~Ukey(void)
{
}
string Ukey::init(void)
{
	HINSTANCE gt1 = NULL;
	try{
		gt1=LoadLibrary("xjkKeyMana.dll");

		if(!gt1){
			DWORD er = GetLastError();
			//AfxMessageBox("2");
			throw(exception("调用读写器动态连接库(xjkKeyMana.dll)失败!"));
		}
		Uopen=(fOpenUkey)GetProcAddress(gt1,"xjkOpenKeyEx");
		Uclose=(fCloseUkey)GetProcAddress(gt1,"xjkCloseKey");
		Uread=(fReadS)GetProcAddress(gt1,"xjkReadSector");
		Uwrite=(fWriteS)GetProcAddress(gt1,"xjkWriteSector");
		Uchange=(fChangePwd)GetProcAddress(gt1,"xjkChangePwd");
		uSerial=(fGetSerial)GetProcAddress(gt1,"xjkGetSerial");
		if(!Uopen&&!Uclose&&!Uread&&!Uwrite&&!Uchange&&!uSerial){
			throw(exception("查找函数失败!"));
		}
		if(!Uopen("888888")){
			if(!Uopen("helloworld")){
				return "open error";
			}
		}

	}
	catch(const exception& e){
		return e.what();	
	}
	//MessageBox(NULL,"2",nullptr,MB_OK);
	return "OK";
};
string Ukey::read(int num)
{
	
	char  result[512]={0};
	//MessageBox(NULL,"2",nullptr,MB_OK);
	
	if(!Uread(result,num)){
		return "read error";
	}
	//MessageBox(NULL,"3",nullptr,MB_OK);
	Uclose();
	return result;

};
string Ukey::write(string str)
{
	
	char  result[64]={0};
	strcpy_s(result,str.c_str());
	//MessageBox(nullptr,result,nullptr,MB_OK);
	if(!Uwrite(result,10)){
		return "write error";
	}
	Uclose();
	return "OK";
};