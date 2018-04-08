#include "StdAfx.h"
#include ".\reader_cmrf801u.h"

CReader_Cmrf801u::CReader_Cmrf801u(void)
{
}

CReader_Cmrf801u::~CReader_Cmrf801u(void)
{
}

string EncryptNoUseBlock(int type)
{
	string strResult;
	HINSTANCE hInstMaster = NULL;
	try{

		hInstMaster = LoadLibrary("function.dll");
		if(!hInstMaster){
			DWORD er = GetLastError();
			throw(exception("调用读写器动态连接库(function.dll)失败!"));
		}
		unsigned char tpassword[6]={0};
		unsigned char defaultPass[6] = {0xFF,0xFF,0xFF,0xFF,0xFF,0xFF};
		unsigned char oldPassword[6] = {0xD5,0xC5,0xB3,0xCF,0xCE,0xE4};
		typedef int (WINAPI MF_Write_T)(int mode, int blk_add, int num_blk, BYTE *buf, BYTE *buffer);
		MF_Write_T* MF_Write = (MF_Write_T*)GetProcAddress(hInstMaster, "MF_Write");
		if (!MF_Write)
		{
			throw(exception("查找写卡函数失败!"));
		}

		unsigned char initBuf[16] = {0xD5,0xC5,0xB3,0xCF,0xCE,0xE4,0x7F,0x07,0x88,0x69,0x6E,0x65,0x6F,0x77,0x65,0x72};
		for( int i = 5; i <= 15; i++ ){	
			unsigned char tmpBuf[16]={0};
			memcpy(tmpBuf,initBuf,16);
			if ( 0 == type)
			{
				memcpy((unsigned char*)tpassword,(unsigned char*)defaultPass,6);
			}
			else if(1 == type)
			{
				memcpy((unsigned char*)tpassword,(unsigned char*)oldPassword,6);
			}	
			if( MF_Write(0, i*4+3, 1, tpassword, tmpBuf) != 0 ) 
			{
				continue;
			}
		}				
	}catch(const exception& e){
		strResult = e.what();
	}

	FreeLibrary(hInstMaster);
	return strResult;
}

//查询D3是否在用
bool CReader_Cmrf801u::QuestReader801InUse()
{
	HINSTANCE hInstMaster = NULL;
	HINSTANCE gt1 = NULL;
	try{
		unsigned char tbuf[16];
		unsigned char defaultPass[6] = {0xFF,0xFF,0xFF,0xFF,0xFF,0xFF};
		unsigned char oldPassword[6] = {0xD5,0xC5,0xB3,0xCF,0xCE,0xE4};
		
		hInstMaster = LoadLibrary("function.dll");
		if(!hInstMaster) return false;

		typedef int (WINAPI MF_Read_T)(int mode, int blk_add, int num_blk, BYTE *buf, BYTE *buffer);
		MF_Read_T* MF_Read = (MF_Read_T*)GetProcAddress(hInstMaster, "MF_Read");
		if (NULL == MF_Read)
		{
			throw(exception("查找读卡函数失败!"));
		}

		char buffer[0x400],temp[0x40];
		BYTE tmpBuf[0x100];
		unsigned char tpassword[6];
		int encryptFlag = -1;
		//判断是否是未初始化的卡 没有任何数据  
		int ret=0;
		memcpy(tpassword,oldPassword,6);//
		ret = MF_Read(0, 7, 1, tpassword, (unsigned char *)buffer);//读第二区的第一块
		if (0 != ret)
		{
			memcpy(tpassword,defaultPass,6);//
			ret = MF_Read(0, 7, 1, tpassword, (unsigned char *)buffer);//读第二区的第一块
			if (0 != ret)
			{
				FreeLibrary(hInstMaster);
				hInstMaster = NULL;
				return false;
			}
		}
	}
	catch(const exception& e)
	{
		//cout << e.what() << endl;
		FreeLibrary(hInstMaster);
		hInstMaster = NULL;
		return false;
	}
	catch(...){
		FreeLibrary(hInstMaster);
		hInstMaster = NULL;
		return false;
	}
	FreeLibrary(hInstMaster);
	hInstMaster = NULL;
	return true;
}


string CReader_Cmrf801u::ReadRFID_CMRF801U(LONG overTime)
{
	string strResult;
	bool retFlag=false;
	vector<string> outV;
	int encryptFlag = -1;
	unsigned char tbuf[16];
	unsigned char defaultPass[6] = {0xFF,0xFF,0xFF,0xFF,0xFF,0xFF};
	unsigned char oldPassword[6] = {0xD5,0xC5,0xB3,0xCF,0xCE,0xE4};
	unsigned char initBuf[16] = {0xD5,0xC5,0xB3,0xCF,0xCE,0xE4,0x7F,0x07,0x88,0x69,0x6E,0x65,0x6F,0x77,0x65,0x72};
	HINSTANCE hInstMaster = NULL;

	try{
		hInstMaster = LoadLibrary("function.dll");
		if(!hInstMaster) return false;

		typedef int (WINAPI MF_Read_T)(int mode, int blk_add, int num_blk, BYTE *buf, BYTE *buffer);
		MF_Read_T* MF_Read = (MF_Read_T*)GetProcAddress(hInstMaster, "MF_Read");
		typedef int (WINAPI MF_Write_T)(int mode, int blk_add, int num_blk, BYTE *buf, BYTE *buffer);
		MF_Write_T* MF_Write = (MF_Write_T*)GetProcAddress(hInstMaster, "MF_Write");
		if (NULL == MF_Read || NULL == MF_Write)
		{
			throw(exception("查找读写卡函数失败!"));
		}

		char buffer[0x400],temp[0x40];
		BYTE tmpBuf[0x100];
		unsigned char tpassword[6];
		//判断是否是未初始化的卡 没有任何数据  
		int ret=0;
		memcpy(tpassword,defaultPass,6);//
		ret = MF_Read(0, 7, 1, tpassword, (unsigned char *)buffer);//读第二区的第一块
		if (0 == ret) {
			throw(exception("此卡为新卡,请先初始化!"));
		}

		if (-1 == encryptFlag)
		{
			//双密码卡
			//ret=0;
			for (int i=1;i<=2;i++){//如果读取失败 多读几次
				memcpy(tpassword,oldPassword,6);
				if (MF_Read(0, 0x17, 1, tpassword, tmpBuf) != 0) {
					Sleep(100);
					continue;
				}
				encryptFlag = 2;//双密码卡
				break;
			}
		}
		//判断是否为旧的写卡方式  单密码
		if (-1 == encryptFlag)
		{
			//判断是否为旧的写卡方式  单密码
			memcpy(tpassword,oldPassword,6);
			if (0 == MF_Read(0, 7, 1, tpassword, tmpBuf)) {
				memcpy(tpassword,defaultPass,6);
				if (0 == MF_Read(0, 0x17, 1, tpassword, tmpBuf)) {
					encryptFlag = 1;//单密码卡
				}
			}
		}
		if (-1 == encryptFlag)
		{
			strResult = "请放入卡！";
		}
		if(1 == encryptFlag)
		{//是旧的写卡方式 因为旧的从14之后都是用的ff ff的密码
			//如果是旧写卡密码 就修改为双密码
			for(int i=1;i<=2;i++)//如果出错 重写3次
			{
				//用单密码修改有数据的前面4个区
				memcpy(tpassword,oldPassword,6);
				memcpy(tbuf,initBuf,16);
				if( MF_Write(0, 7, 1, tpassword, tbuf) != 0 ) continue;
				memcpy(tpassword,oldPassword,6);
				memcpy(tbuf,initBuf,16);
				if( MF_Write(0, 0x0B, 1, tpassword, tbuf) != 0 ) continue;
				memcpy(tpassword,oldPassword,6);
				memcpy(tbuf,initBuf,16);
				if( MF_Write(0, 0x0F, 1, tpassword, tbuf) != 0 ) continue;
				memcpy(tpassword,oldPassword,6);
				memcpy(tbuf,initBuf,16);
				if( MF_Write(0, 0x13, 1, tpassword, tbuf) != 0 ) continue;
				break;
			}
			//再修改后面的7个没有数据的区域
			EncryptNoUseBlock(encryptFlag);//加密没有使用的扇区
		}
		if (2 == encryptFlag)
		{
			//如果是新的双密码 或者已经将老的单密码处理为双密码之后 开始读数据
			retFlag = false;
			for (int i = 0; i < overTime; i++){
				memcpy((unsigned char*)tpassword,(unsigned char*)oldPassword,6);//之前只是修改了加密方式为双密码 单任然是用旧密码读
				if( MF_Read(0, 4, 4, tpassword, (unsigned char*)buffer) != 0 ){
					Sleep(100);
					continue;
				}		
				strncpy(temp,buffer,8);temp[8] = 0;
				outV.push_back(temp); //卡类型(8)
				strncpy(temp,buffer+8,14);temp[14] = 0;
				outV.push_back(temp); //企业代码(14)
				strncpy(temp,buffer+22,26);temp[26] = 0;
				outV.push_back(temp); //企业名称(26/48)		
				retFlag = true;
				break;
			}
			if (!retFlag) throw(exception("读卡错误！"));

			retFlag = false;
			for (int i = 0; i < overTime; i++){
				memcpy((unsigned char*)tpassword,(unsigned char*)oldPassword,6);//之前只是修改了加密方式为双密码 单任然是用旧密码读
				if( MF_Read(0, 8, 4, (unsigned char*)tpassword, (unsigned char*)buffer)  != 0 ){
					Sleep(100);
					continue;
				}
				//memcpy((unsigned char*)tpassword,(unsigned char*)defaultPass,6);//之前只是修改了加密方式为双密码 单任然是用旧密码读
				#if 0
				char buffer1[0x40]={0};
				if (MF_Read(0,4,4,(unsigned char *)tpassword,(unsigned char *)buffer1)!=0)
				{
					Sleep(100);
					continue;;
				}
				buffer1[48]='\0';
				string a=buffer1;
				//sprintf(,"%s",buffer1);
				AfxMessageBox(a.c_str());
				#endif
				strncpy(temp,buffer,22);temp[22] = 0;
				outV[outV.size()-1] += temp; //企业名称(22/48)
				strncpy(temp,buffer+22,26);temp[26] = 0;
				outV.push_back(temp);  //联系电话(26/30)
				retFlag = true;
				break;
			}
			if (!retFlag) throw(exception("读卡错误！"));

			retFlag = false;
			for (int i = 0; i < overTime; i++){
				memcpy((unsigned char*)tpassword,(unsigned char*)oldPassword,6);//之前只是修改了加密方式为双密码 单任然是用旧密码读
				if( MF_Read(0, 0x0C, 4, (unsigned char*)tpassword, (unsigned char*)buffer)  != 0 ){
					Sleep(100);
					continue;
				}
				strncpy(temp,buffer,4);temp[4] = 0;
				outV[outV.size()-1] += temp; //联系电话(4/30)
				strncpy(temp,buffer+4,32);temp[32] = 0;
				outV.push_back(temp); //溯源称设备代码(32/32)
				strncpy(temp,buffer+36,12);temp[12] = 0;
				outV.push_back(temp); //市场名称(12/48)
				retFlag = true;
				break;
			}
			if (!retFlag) throw(exception("读卡错误！"));

			retFlag = false;
			for (int i = 0; i < overTime; i++){
				memcpy((unsigned char*)tpassword,(unsigned char*)oldPassword,6);
				if( MF_Read(0, 0x10, 4, (unsigned char*)tpassword, (unsigned char*)buffer)  != 0 ){
					Sleep(100);
					continue;
				}
				strncpy(temp,buffer,36);temp[36] = 0;
				outV[outV.size()-1] += temp; //市场名称 (36 48/48)
				strncpy(temp,buffer+36,1);temp[1] = 0;
				outV.push_back(temp);  //内部序号（1字节）
				retFlag = true;
				break;
			}
			if (!retFlag) throw(exception("读卡错误！"));
		}
		//char a[10];
		//sprintf(a,"%d",encryptFlag);
		//AfxMessageBox(a);
	}catch(const exception& e){
		//cout << e.what() << endl;
		strResult = e.what();
	}catch(...){
		strResult = "读卡错误！";
	}

	FreeLibrary(hInstMaster);
	hInstMaster = NULL;
	for(vector<string>::const_iterator iter = outV.begin(); iter != outV.end(); ++iter){
		strResult += iter->c_str();
		strResult += "|";
	}
	return strResult;
}

// 0 1 2 3   4 5 6 7    8 9 0a 0b     0c 0d 0e 0f     10 11 12 13      14 15 16 17      18
string CReader_Cmrf801u::WriteRFID_CMRF801U(LPCTSTR codeStr, LONG mode)
{
	//AFX_MANAGE_STATE(AfxGetStaticModuleState());
	string strResult;
	int encryptFlag = -1;// 0代表新卡初始化 1代表老卡重置 2代表双密码卡重置
	unsigned char tpassword[6];
	unsigned char defaultPassword[6] = {0xFF,0xFF,0xFF,0xFF,0xFF,0xFF};
	unsigned char oldPassword[6] = {0xD5,0xC5,0xB3,0xCF,0xCE,0xE4};
	vector<string> V;
	string writeStr = (char*)codeStr;
	while (true){
		if (writeStr.length() > 0){
			V.push_back(writeStr.substr(0,writeStr.find("|")));
			writeStr = writeStr.substr(writeStr.find("|")+1);
			continue;
		}
		break;
	}
	HINSTANCE hInstMaster = NULL;
	try{
		if( V.size() != 7)
			throw(exception("参数个数不够"));
		if( V[0].size() != 8 )
			throw(exception("卡类型 位数不对"));
		if (V[1].size() != 14)
			throw(exception("企业代码 位数不对"));
		if (V[2].size() > 48)
			throw(exception("企业名称 超长"));
		if(V[3].size() > 30)
			throw(exception("联系电话超长"));
		if(V[4].size() != 32)
			throw(exception("溯源称设备代码 位数不对"));
		if(V[5].size() > 48)
			throw(exception("市场名称 超长"));
		if(V[6].size()!=1)
			throw(exception("内部序号 位数不对"));

		hInstMaster = LoadLibrary("function.dll");
		if(!hInstMaster){
			DWORD er = GetLastError();
			throw(exception("调用读写器动态连接库(function.dll)失败!"));
		}

		unsigned char tmpBuf[0x100]={0};
		char buffer[0x100]={0};
		char* p = buffer;
		strcpy(p,V[0].c_str()); //卡类型(8/8)
		strcpy(p+=8,V[1].c_str()); //企业代码part1(8/14)
		strcpy(p+=14,V[2].substr(0,26).c_str()); //企业名称(26/48)
		p += 26;
		*p++ = 0xD5;*p++ = 0xC5;*p++ = 0xB3;*p++ = 0xCF;*p++ = 0xCE;*p++ = 0xE4;*p++ = 0x7F;*p++ = 0x07;*p++ = 0x88;*p++ = 0x69;*p++ = 0x6E;*p++ = 0x65;*p++ = 0x6F;*p++ = 0x77;*p++ = 0x65;*p++ = 0x72;
		if(V[2].length()>26)                     //企业名称(22/48)
			strcpy(p,V[2].substr(26).c_str());
		strcpy(p+=22,V[3].substr(0,26).c_str()); //联系电话(26/30)
		p += 26;
		*p++ = 0xD5;*p++ = 0xC5;*p++ = 0xB3;*p++ = 0xCF;*p++ = 0xCE;*p++ = 0xE4;*p++ = 0x7F;*p++ = 0x07;*p++ = 0x88;*p++ = 0x69;*p++ = 0x6E;*p++ = 0x65;*p++ = 0x6F;*p++ = 0x77;*p++ = 0x65;*p++ = 0x72;
		if (V[3].length()>26)                    //联系电话(4/30)
			strcpy(p,V[3].substr(26).c_str());
		strcpy(p+=4,V[4].c_str());              //设备代码(32/32)
		strcpy(p+=32,V[5].substr(0,12).c_str()); //市场名称(12/48)
		p += 12;
		*p++ = 0xD5;*p++ = 0xC5;*p++ = 0xB3;*p++ = 0xCF;*p++ = 0xCE;*p++ = 0xE4;*p++ = 0x7F;*p++ = 0x07;*p++ = 0x88;*p++ = 0x69;*p++ = 0x6E;*p++ = 0x65;*p++ = 0x6F;*p++ = 0x77;*p++ = 0x65;*p++ = 0x72;
		if (V[5].length()>12)                   //市场名称(36/48)
			strcpy(p,V[5].substr(12).c_str());
		strcpy(p+=36,V[6].c_str());             //内部序号(1/1)
		p += 12;
		*p++ = 0xD5;*p++ = 0xC5;*p++ = 0xB3;*p++ = 0xCF;*p++ = 0xCE;*p++ = 0xE4;*p++ = 0x7F;*p++ = 0x07;*p++ = 0x88;*p++ = 0x69;*p++ = 0x6E;*p++ = 0x65;*p++ = 0x6F;*p++ = 0x77;*p++ = 0x65;*p++ = 0x72;

		typedef int (WINAPI MF_Write_T)(int mode, int blk_add, int num_blk, BYTE *buf, BYTE *buffer);
		MF_Write_T* MF_Write = (MF_Write_T*)GetProcAddress(hInstMaster, "MF_Write");
		typedef int (WINAPI MF_Read_T)(int mode, int blk_add, int num_blk, BYTE *buf, BYTE *buffer);
		MF_Read_T* MF_Read = (MF_Read_T*)GetProcAddress(hInstMaster, "MF_Read");
		if (NULL == MF_Read || NULL == MF_Write)
		{
			throw(exception("查找读、写卡函数失败!"));
		}

		//判断是否是未初始化的卡 没有任何数据  
		int ret=0;
		for (int i=1;i<=1;i++){//如果读取失败 多读几次
			memcpy(tpassword,defaultPassword,6);//
			ret = MF_Read(0, 7, 1, tpassword, tmpBuf);//读第二区的第一块
			if (ret != 0) {
				Sleep(100);
				continue;
			}
			encryptFlag = 0;//未初始化卡
			break;
		}
		if (-1 == encryptFlag)
		{
			//双密码卡
			ret=0;
			for (int i=1;i<=2;i++){//如果读取失败 多读几次
				memcpy(tpassword,oldPassword,6);
				ret = MF_Read(0, 0x17, 1, tpassword, tmpBuf);
				if (ret != 0) {
					Sleep(100);
					continue;
				}
				encryptFlag = 2;//双密码卡
				break;
			}
		}
		if (-1 == encryptFlag)
		{
			//判断是否为旧的写卡方式  单密码
			ret=0;
			memcpy(tpassword,oldPassword,6);
			ret = MF_Read(0, 7, 1, tpassword, tmpBuf);
			if (0 == ret) {
				memcpy(tpassword,defaultPassword,6);
				ret = MF_Read(0, 0x17, 1, tpassword, tmpBuf);
				if (0 == ret) {
					encryptFlag = 1;//单密码卡
					//读取失败  代表是旧的写卡方式 因为旧的从14之后都是用的ff ff的密码
					//如果是旧写卡密码 就修改为双密码
				}
			}
		}
		if (-1 == encryptFlag)
		{
			strResult = "请放入卡！";
		}
		//1添加  全部加密   在初始化的时候还是重置的时候也必须写  2循环写3次 
		if (0 == encryptFlag)
		{
			for( int i = 1; i <= 4; i++ )
			{	
				for(int j = 1;j<=3;j++)
				{
					//unsigned char tmpBuf[0x100];
					memcpy(tmpBuf,buffer+(i-1)*4*16,0x100);
					memcpy((unsigned char*)tpassword,(unsigned char*)defaultPassword,6);
					if( MF_Write(0, i*4, 4, tpassword, tmpBuf) != 0 )//默认密码写						
					{
						Sleep(100);
						continue;
					}
					else strResult = "";
					break;	
				}				
			}
			EncryptNoUseBlock(encryptFlag);//加密没有使用的扇区
		}
		if (1 == encryptFlag || 2 == encryptFlag)
		{
			for( int i = 1; i <= 4; i++ )
			{	
				for(int j = 1;j<=3;j++)
				{
					//unsigned char tmpBuf[0x100];
					memcpy(tmpBuf,buffer+(i-1)*4*16,0x100);
					memcpy((unsigned char*)tpassword,(unsigned char*)oldPassword,6);
					//注意 4 还是 3
					ret = 0;
					if (1 == encryptFlag)//单密码就 全部修改
					{
						ret = MF_Write(0, i*4, 4, tpassword, tmpBuf);
					}
					if (2 == encryptFlag)//如果用了新的双卡密码  只修改数据就可以了 所以只写每个区的3块  用4也不能修改成功 只能用new密码才可以修改
					{
						ret = MF_Write(0, i*4, 3, tpassword, tmpBuf);
					}

					if(ret != 0 )
					{
						Sleep(100);
						continue;
					}
					else strResult = "";
					break;	
				}				
			}
			if (1 == encryptFlag)
			{
				EncryptNoUseBlock(encryptFlag);//加密没有使用的扇区
			}
		}
	}catch(const exception& e){
		strResult = e.what();
    }
	catch(...){
		strResult = "写卡异常";
	}

	FreeLibrary(hInstMaster);
	return strResult;
}