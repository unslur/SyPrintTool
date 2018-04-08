#include "stdafx.h"
#include "Funcs.h"



extern char  DBPathName[40]={"d:\\zyczs\\tcm.db" };
//string FindAndCreatDB( const char* dbPathName ){//文件不存在就创建空文件
//	bool creatF = false;
//#if 1
//	fstream   file;
//	//AfxMessageBox(dbPathName);
//	file.open( dbPathName,ios::in);
//	
//	if(file.is_open()){
//		file.seekg(0,std::ios::end );
//		if(file.tellg() > 0 )
//			creatF= true;
//	}else{//文件不存在创建文件
//		file.open(dbPathName,ios::out);//只能创建已存在的目录下的文件
//		if(!file.is_open()) //创建失败
//			return "310";
//		
//	}
//	file.close();
//#endif	
//#if 0
//
//FILE *fp=fopen(dbPathName,"a+");
//if(fp==NULL){
//	return strerror(errno);
//}
//fseek(fp,0,SEEK_END);
//if(ftell(fp)){
//creatF=true;
//}
//fclose(fp);
//#endif
//	if (!creatF){
//		char buff[0x400];
//		sqlite3 *db;
//		char *zErrMsg = 0;
//		int rc;
//		rc = sqlite3_open(dbPathName, &db);
//		if( rc ){
//			sqlite3_close(db);
//			sprintf(buff,"311",sqlite3_errmsg(db));//打开本地sqlite数据库文件失败
//			return buff;
//		}
//		rc = sqlite3_exec(db, "create table number(box_code varchar(500),box_sourcecode varchar(500),printtask_code varchar(500),flag int)", NULL, 0, &zErrMsg);
//		if( rc!=SQLITE_OK ){
//			sprintf(buff, "312", zErrMsg);//创建本地数据库表失败
//			sqlite3_free(zErrMsg);
//			sqlite3_close(db);
//			return buff;
//		}
//		sqlite3_close(db);
//	}	
//	return "OK"; 
//}
//void filePath(){//设置数据库目录文件
//	
//	if(GetDriveType("d:") == DRIVE_FIXED){
//		strcpy(DBPathName,"d:\\zyczs\\tcm.db");
//	
//	}	
//	else if(GetDriveType("c:") == DRIVE_FIXED){
//		strcpy(DBPathName,"c:\\zyczs\\tcm.db");
//	
//	}
//
//	else
//	{
//		GetSystemDirectory(DBPathName,strlen(DBPathName)-1);
//		strcpy(DBPathName +1,":\\zyczs\\tcm.db");
//	}
//
//}

//string DelRecordByCode(const char* code,const char* dbPathName){
//	char buff[0x400];
//	//const char *dbPathName =dBPathName.c_str();
//	sqlite3 *db;
//	char *zErrMsg = 0;
//	int rc;
//	rc = sqlite3_open(dbPathName, &db);
//	if( rc ){
//		sqlite3_close(db);
//		sprintf(buff,"Can't open database: %s",sqlite3_errmsg(db));
//		return buff;
//	}
//	
//	sprintf(buff,"delete from number where box_sourcecode='%s'",code);
//	rc = sqlite3_exec(db, buff, NULL, 0, &zErrMsg);
//	if( rc!=SQLITE_OK ){
//		sprintf(buff, "SQL error: %s", zErrMsg);
//		sqlite3_free(zErrMsg);
//		sqlite3_close(db);
//		return buff;
//	}
//	sqlite3_close(db);
//	return "OK";
//}//*/

void splitStrTwo( string printStr, string &Str1, string &Str2 )
{

	int b = printStr.find("|");
	if( b != -1 ){
		Str1 = printStr.substr(0,b);
		Str2 = printStr.substr(b+1,printStr.length());
	}
	else{
		if(printStr.length()!=0){
			Str1 = printStr;
		}
	}

	return;
}

list<string> splitPrintStr( string printStr ){
	list<string> vstr;
	while (true){
		int b = printStr.find("\\n");
		if( b != -1 ){
			vstr.push_back(printStr.substr(0,b));
			printStr = printStr.substr(b+2,printStr.length());
		}
		else{
			if(printStr.length()!=0){
				vstr.push_back(printStr);
			}
			break;
		}
	}
	return vstr;
}
vector<string> splitPrintStrVector(string printStr) {
	vector<string> vstr;
	while (true) {
		int b = printStr.find("\\n");
		if (b != -1) {
			vstr.push_back(printStr.substr(0, b));
			printStr = printStr.substr(b + 2, printStr.length());
		}
		else {
			if (printStr.length() != 0) {
				vstr.push_back(printStr);
			}
			break;
		}
	}
	return vstr;
}
list<string> splitPrintStr2( string printStr,char * Cnum ){
	list<string> vstr;
	while (true){
		int b = printStr.find(Cnum[0]);
		if( b != -1 ){
			vstr.push_back(printStr.substr(0,b));
			printStr = printStr.substr(b+1,printStr.length());
		}
		else{
			if(printStr.length()!=0){
				vstr.push_back(printStr);
			}
			break;
		}
	}
	return vstr;
}
vector<string> VsplitPrintStr(string printStr) {
	vector<string> vstr;
	while (true) {
		int b = printStr.find("#");
		if (b != -1) {
			vstr.push_back(printStr.substr(0, b));
			printStr = printStr.substr(b + 1, printStr.length());
		}
		else {
			if (printStr.length() != 0) {
				vstr.push_back(printStr);
			}
			break;
		}
	}
	return vstr;
}
vector<string> VsplitPrintStr2(string printStr) {
	vector<string> vstr;
	while (true) {
		int b = printStr.find("!");
		if (b != -1) {
			vstr.push_back(printStr.substr(0, b));
			printStr = printStr.substr(b + 1, printStr.length());
		}
		else {
			if (printStr.length() != 0) {
				vstr.push_back(printStr);
			}
			break;
		}
	}
	return vstr;
}
void splitStrThree( string printStr, string &Str1, string &Str2, string&Str3 ){

	string strTemp;

	int b = printStr.find("|");
	if( b != -1 ){
		Str1 = printStr.substr(0,b);
		strTemp = printStr.substr(b+1,printStr.length());
		b = 0;
		b = strTemp.find("|");

		if (b != -1) {
			Str2 = strTemp.substr(0, b);
			Str3 = strTemp.substr(b+1,strTemp.length());
		}
		else {
			if(printStr.length()!=0){
				Str2 = strTemp;
			}
		}
	}
	else{
		if(printStr.length()!=0){
			Str1 = printStr;
		}
	}

	return;
}

//#define DYJC 34

list<string> splitPrintv( list<string>& printStrv, int DYJC ){
	list<string> vstr;
	for(list<string>::const_iterator iter = printStrv.begin(); iter != printStrv.end();++iter){
		string tstr = *iter; //GBK 前字81-FE 之间，尾字节在 40-FE
		while(true){
			if(tstr.length()<=DYJC){
				vstr.push_back(tstr);
				break;
			}
			bool f = false;
			for( int i = 0; i < DYJC;){
				char m = tstr.at(i);
				if( unsigned(m) < 0x80 ){
					f = true;
					i++;
				}
				else{
					i += 2;
					if(i%2==0){
						f = true;
					}else{
						f = false;
					}
				}
			}
			if(f){
				vstr.push_back(tstr.substr(0,DYJC));
				tstr = tstr.substr(DYJC,tstr.length());
			}
			else{
				vstr.push_back(tstr.substr(0,DYJC-1));
				tstr = tstr.substr(DYJC-1,tstr.length());
			}
		}
	}
	return vstr;
}


list<string> splitPrintv( string& tstr, int DYJC ){
	list<string> vstr;
	while(true){
		if(tstr.length()<=DYJC){
			vstr.push_back(tstr);
			break;
		}
		bool f = false;
		for( int i = 0; i < DYJC;){
			char m = tstr.at(i);
			if( unsigned(m) < 0x80 ){//汉字第一个字节在）0x81-0xFE之间 ，
				f = true;
				i++;
			}
			else{
				i += 2;
				if(i%2==0){
					f = true;
				}else{
					f = false;
				}
			}
		}
		if(f){
			vstr.push_back(tstr.substr(0,DYJC));
			tstr = tstr.substr(DYJC,tstr.length());
		}
		else{
			vstr.push_back(tstr.substr(0,DYJC-1));
			tstr = tstr.substr(DYJC-1,tstr.length());
		}
	}
	return vstr;
}

char* Gb2312ToUtf8(const char *pcGb2312) /*const*/
{
	int nUnicodeLen = MultiByteToWideChar(CP_ACP, 0, pcGb2312, -1, NULL, 0);

	wchar_t * pcUnicode = new wchar_t[nUnicodeLen+1]; 
	memset(pcUnicode, 0, nUnicodeLen * 2 + 2); 

	MultiByteToWideChar(CP_ACP, 0, pcGb2312, -1, pcUnicode, nUnicodeLen);

	int nUtf8Len = WideCharToMultiByte(CP_UTF8, 0, pcUnicode, -1, NULL, 0, NULL, NULL);

	char *pcUtf8=new char[nUtf8Len + 1]; 
	memset(pcUtf8, 0, nUtf8Len + 1);   

	WideCharToMultiByte(CP_UTF8, 0, pcUnicode, -1, pcUtf8, nUtf8Len, NULL, NULL);

	delete[] pcUnicode; 
	return pcUtf8;
}
char* U2G(const char* utf8)
{
	int len = MultiByteToWideChar(CP_UTF8, 0, utf8, -1, NULL, 0);
	wchar_t* wstr = new wchar_t[len + 1];
	memset(wstr, 0, len + 1);
	MultiByteToWideChar(CP_UTF8, 0, utf8, -1, wstr, len);
	len = WideCharToMultiByte(CP_ACP, 0, wstr, -1, NULL, 0, NULL, NULL);
	char* str = new char[len + 1];
	memset(str, 0, len + 1);
	WideCharToMultiByte(CP_ACP, 0, wstr, -1, str, len, NULL, NULL);
	if (wstr) delete[] wstr;
	return str;
}
void delchar(char * str,char CharNum){
	char *d = str;
	while (*str != '\0') {
			
		if (*str != CharNum) {
			*d++ = *str;
		}
			
		++str;
		
	}
	*d = '\0';
	
}