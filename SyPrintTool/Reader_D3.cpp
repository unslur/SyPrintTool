#include "StdAfx.h"
#include ".\reader_d3.h"

/////////////////////////////////
typedef HANDLE(__stdcall *lpinit)(int port,long baud);
typedef int(__stdcall *lpcard)(HANDLE icdev,unsigned char _Mode,unsigned long *_Snr);
typedef int(__stdcall *lploadkey)(HANDLE icdev,unsigned char _Mode,unsigned char _SecNr,unsigned char *_NKey);
typedef int(__stdcall *lpauthentication)(HANDLE icdev,unsigned char _Mode,unsigned char _SecNr);
typedef int(__stdcall *lpread)(HANDLE icdev,unsigned char _Adr,unsigned char *_Data);
typedef int(__stdcall *lpwrite)(HANDLE icdev,unsigned char _Adr,unsigned char *_Data);
typedef int(__stdcall *lpbeep)(HANDLE icdev,unsigned int _Msec);
typedef int(__stdcall *lpexit)(HANDLE icdev); 
typedef int(__stdcall *lpreset)(HANDLE icdev,unsigned __int16 _Msec);
typedef int(__stdcall *lpchangb3)(HANDLE icdev,unsigned char _SecNr,unsigned char *_KeyA,unsigned char _B0,unsigned char _B1,unsigned char _B2,unsigned char _B3,unsigned char _Bk,unsigned char * _KeyB);
lpinit initdc = NULL;
lpcard carddc = NULL;
lploadkey loadkeydc = NULL;
lpauthentication authenticationdc = NULL;
lpread readdc= NULL;
lpwrite writedc = NULL;
lpbeep beepdc = NULL;	
lpexit exitdc = NULL;
lpreset resetdc = NULL;
lpchangb3 changeb3dc=NULL;
///////////////////////////////

CReader_D3::CReader_D3(void)
{
}

CReader_D3::~CReader_D3(void)
{
}

//查询D3是否在用
bool CReader_D3::QuestReaderD3InUse()
{
	HINSTANCE gt1 = NULL;
	try{
		gt1=LoadLibrary("dcrf32.dll");

		if(!gt1){
			DWORD er = GetLastError();
			//AfxMessageBox("2");
			throw(exception("调用读写器动态连接库(dcrf32.dll)失败!"));
		}
		initdc= (lpinit)GetProcAddress(gt1,"dc_init");
		carddc= (lpcard)GetProcAddress(gt1,"dc_card");	
		loadkeydc= (lploadkey)GetProcAddress(gt1,"dc_load_key");	
		exitdc= (lpexit)GetProcAddress(gt1,"dc_exit");
		if (!initdc||!carddc||!loadkeydc||!exitdc)
		{	
			//AfxMessageBox("22");
			throw(exception("查找D3读、写卡函数失败!"));
		}
	
		unsigned long snr;
		HANDLE icdev;
		icdev = initdc(100,0);//usb初始化
		if ((int)icdev < 0)	{
			
			throw("初始化失败");}
		carddc(icdev,0,&snr);
		#if 0   //显示卡id号
 		char buff[32] ;
		itoa(snr,buff,16);
		AfxMessageBox(buff);
		#endif
		//未初始化
		unsigned char defaultPass[6] = {0xFF,0xFF,0xFF,0xFF,0xFF,0xFF};
		unsigned char oldPassword[6] = {0xD5,0xC5,0xB3,0xCF,0xCE,0xE4};
		loadkeydc(icdev, 0,1,oldPassword);//1表示扇区号1，M1卡扇区号0~15
		if( carddc(icdev,0,&snr) == 1 )
		{
			loadkeydc(icdev, 0,1,defaultPass);
			if( carddc(icdev,0,&snr) == 1 )
			{
				exitdc(icdev); 
				//AfxMessageBox("11");
				throw(exception("请放入卡"));
			} 
		}    
		exitdc(icdev);
	}
	catch(const exception& e)
	{
		//cout << e.what() << endl;
		return false;
	}
	catch(...){
		return false;
	}
	return true;
}

string CReader_D3::ReadRFID_D3(LONG overTime)
{
	string strResult;
	vector<string> outV;
	HANDLE icdev;
	//S50卡新密码
	unsigned char defaultPass[6] = {0xFF,0xFF,0xFF,0xFF,0xFF,0xFF};
	unsigned char oldPassword[6] = {0xD5,0xC5,0xB3,0xCF,0xCE,0xE4};
	unsigned char keyB[6] = {0x6E,0x65,0x6F,0x77,0x65,0x72};//密码B
	unsigned char controlKey[4] = {0x7F,0x07,0x88,0x69};//控制字节
	unsigned char initBuf[16] = {0xD5,0xC5,0xB3,0xCF,0xCE,0xE4,0x7F,0x07,0x88,0x69,0x6E,0x65,0x6F,0x77,0x65,0x72};
	char buffer[0x40]={0};
	char temp[0x40]={0};
	int st = 0;
	int encryptType=-1;//0默认密码 1 单密码 2双密码
	HINSTANCE gt1 = NULL;
	try{
		gt1=LoadLibrary("dcrf32.dll");

		if(!gt1){
			DWORD er = GetLastError();
			throw(exception("调用读写器动态连接库(dcrf32.dll)失败!"));
		}
		initdc= (lpinit)GetProcAddress(gt1,"dc_init");
		carddc= (lpcard)GetProcAddress(gt1,"dc_card");	
		loadkeydc= (lploadkey)GetProcAddress(gt1,"dc_load_key");	
		authenticationdc= (lpauthentication)GetProcAddress(gt1,"dc_authentication");	
		readdc= (lpread)GetProcAddress(gt1,"dc_read");	
		writedc= (lpwrite)GetProcAddress(gt1,"dc_write");
		beepdc= (lpbeep)GetProcAddress(gt1,"dc_beep");
		exitdc= (lpexit)GetProcAddress(gt1,"dc_exit");
		changeb3dc = (lpchangb3)GetProcAddress(gt1,"dc_changeb3");//添加修改块3的函数，达到加密功能
		if (!initdc||!carddc||!loadkeydc||!authenticationdc||!readdc||!writedc||!beepdc||!exitdc||!changeb3dc)
		//if (!initdc||!carddc||!loadkeydc||!authenticationdc||!readdc||!writedc||!beepdc||!exitdc)
		{
			throw(exception("查找D3读、写卡函数失败!"));
		}

		unsigned long snr;
		icdev = initdc(100,0);//usb初始化
		if ((int)icdev < 0)	{throw("初始化失败");}

		//test
		char tmpBuf[256]={0};
		
		//未初始化
		loadkeydc(icdev, 0,1,defaultPass);
		if( carddc(icdev,0,&snr) == 1 ){ exitdc(icdev); throw(exception("请放入卡"));}       //MODE为1可反复寻卡, MODE为0卡必须拿开才可重新操作!  
		if(authenticationdc(icdev,0,1) == 0){
			if (0 == readdc(icdev,0x7,(unsigned char*)tmpBuf))//0x7
			{
				encryptType = 0;
				throw(exception("this_card_is_uninit!"));
			}
		}
		//单密码
		loadkeydc(icdev, 0,1,oldPassword);
		if( carddc(icdev,0,&snr) == 1 ){ exitdc(icdev); throw(exception("请放入卡"));}         
		if(authenticationdc(icdev,0,1) == 0){
			if (0 == readdc(icdev,0x7,(unsigned char*)tmpBuf))//0x7
			{
				loadkeydc(icdev, 4,5,defaultPass);
				if( carddc(icdev,0,&snr) == 1 ){ exitdc(icdev); throw(exception("请放入卡"));}         
				if(authenticationdc(icdev,4,5) == 0){
					if (0 == readdc(icdev,0x17,(unsigned char*)tmpBuf))//0x17
					{
						encryptType = 1;
						//在此全部加密
						for (int j = 1; j<= 15; j++){

							#if 0//直接使用链接库函数进行加密
							if (changeb3dc(icdev,j,oldPassword,0x00,0x00,0x00,0x11,0,keyB)!=0);
							{
								exitdc(icdev);
								throw(exception("写加密错误"));
							}
							#endif




							#if 1
							bool re_auth=false;
							loadkeydc(icdev, 4,j,defaultPass);
							if( carddc(icdev,0,&snr) == 1 ){ exitdc(icdev); throw(exception("请放入卡"));}         
							if(authenticationdc(icdev,4,j) != 0){//注意0区别
								throw(exception("auth error！"));
							}else{
								re_auth= true;
							}

							
							#if 0
							loadkeydc(icdev,0,j,defaultPass);
							if( carddc(icdev,0,&snr) == 1 ){ exitdc(icdev); throw(exception("请放入卡"));}      //   
							if(authenticationdc(icdev,0,j) != 0){//注意0区别
								if( carddc(icdev,0,&snr) == 1 ){ exitdc(icdev); throw(exception("请放入卡"));}
								if(authenticationdc(icdev,1,j) == 0){//注意1区别 
									re_auth= true;
								}
							}else{
								re_auth= true;
							}
							#endif

							if(re_auth){//写了两次
								if(writedc(icdev,j*4+3,initBuf) != 0){//从0x17 十进制23
									if(writedc(icdev,j*4+3,initBuf) != 0){
										exitdc(icdev); 
										throw(exception("写数据错误1！"));
									}	
								}
							}else{
								throw(exception("写数据错误2！"));
							}	
							#endif	
						}
						//加密完
					}
				}
			}
		}
		
		//read card
		for( int i = 1; i <= 4; i++ ){
			loadkeydc(icdev, 0,i,oldPassword);
		}
		if( carddc(icdev,0,&snr) == 1 ){ exitdc(icdev); throw(exception("请放入卡"));}
		if(authenticationdc(icdev,0,1) == 0){
			for(int k=0;k<overTime;k++){
				if(readdc(icdev,4,(unsigned char*)buffer)==0){
					strncpy(temp,buffer,8);temp[8] = 0;
					outV.push_back(temp); //卡类型(8/8)	
					strncpy(temp,buffer+8,8);temp[8] = 0;
					outV.push_back(temp);//企业代码(8/14)
					break;
				}
			}
			for(int k=0;k<overTime;k++){
				if(readdc(icdev,5,(unsigned char*)buffer)==0){
					strncpy(temp,buffer,6);temp[6] = 0;
					outV[outV.size()-1] += temp; //企业代码(14/14)
					strncpy(temp,buffer+6,10);temp[10] = 0;
					outV.push_back(temp); //企业名称(10/48)
					break;
				}
			}
			for(int k=0;k<overTime;k++){
				if(readdc(icdev,6,(unsigned char*)buffer)==0){
					strncpy(temp,buffer,16);temp[16] = 0;
					outV[outV.size()-1] += temp; //企业名称(16/48)
					break;
				}
			}
		}
		if( carddc(icdev,0,&snr) == 1 ){ exitdc(icdev); throw(exception("请放入卡"));}
		if(authenticationdc(icdev,0,2) == 0){
			for(int k=0;k<overTime;k++){
				if(readdc(icdev,8,(unsigned char*)buffer)==0){
					strncpy(temp,buffer,16);temp[16] = 0;
					outV[outV.size()-1] += temp; //企业名称(16/48)
					break;
				}
			}
			for(int k=0;k<overTime;k++){
				if(readdc(icdev,9,(unsigned char*)buffer)==0){
					strncpy(temp,buffer,6);temp[6] = 0;
					outV[outV.size()-1] += temp;; //企业名称(48/48)
					strncpy(temp,buffer+6,10);temp[10] = 0;
					outV.push_back(temp);  //联系电话(10/30)
					break;
				}
			}
			for(int k=0;k<overTime;k++){
				if(readdc(icdev,10,(unsigned char*)buffer)==0){
					strncpy(temp,buffer,16);temp[16] = 0;
					outV[outV.size()-1] += temp;  //联系电话(16/30)
					break;
				}
			}
		}
		if( carddc(icdev,0,&snr) == 1 ){ exitdc(icdev); throw(exception("请放入卡"));}
		if(authenticationdc(icdev,0,3) == 0){
			for(int k=0;k<overTime;k++){
				if(readdc(icdev,12,(unsigned char*)buffer)==0){
					strncpy(temp,buffer,4);temp[4] = 0;
					outV[outV.size()-1] += temp; //联系电话(30/30)
					strncpy(temp,buffer+4,12);temp[12] = 0;
					outV.push_back(temp); //溯源称设备代码(12/32)
					break;
				}
			}
			for(int k=0;k<overTime;k++){
				if(readdc(icdev,13,(unsigned char*)buffer)==0){
					strncpy(temp,buffer,16);temp[16] = 0;
					outV[outV.size()-1] += temp; //溯源称设备代码(16/32)
					break;
				}
			}
			for(int k=0;k<overTime;k++){
				if(readdc(icdev,14,(unsigned char*)buffer)==0){
					strncpy(temp,buffer,4);temp[4] = 0;
					outV[outV.size()-1] += temp; //溯源称设备代码(32/32)
					strncpy(temp,buffer+4,12);temp[12] = 0;
					outV.push_back(temp); //市场名称(12/48)
					break;
				}
			}
		}
		if( carddc(icdev,0,&snr) == 1 ){ exitdc(icdev); throw(exception("请放入卡"));}
		if(authenticationdc(icdev,0,4) == 0){
			for(int k=0;k<overTime;k++){
				if(readdc(icdev,16,(unsigned char*)buffer)==0){
					strncpy(temp,buffer,16);temp[16] = 0;
					outV[outV.size()-1] += temp; //市场名称 (16 28/48)
					break;
				}
			}
			for(int k=0;k<overTime;k++){
				if(readdc(icdev,17,(unsigned char*)buffer)==0){
					strncpy(temp,buffer,16);temp[16] = 0;
					outV[outV.size()-1] += temp; //市场名称 (16 44/48)
					break;
				}
			}
			for(int k=0;k<overTime;k++){
				if(readdc(icdev,18,(unsigned char*)buffer)==0){
					strncpy(temp,buffer,4);temp[4] = 0;
					outV[outV.size()-1] += temp; //市场名称 (4 48/48)
					strncpy(temp,buffer+4,12);temp[12] = 0;
					outV.push_back(temp); 	//内部序号（1字节）
					break;
				}
			}
		}
		
		//鸣叫100毫秒
		beepdc(icdev,overTime);
		exitdc(icdev);
	}catch(const exception& e){
		//cout << e.what() << endl;
		strResult = e.what();
	}
	catch(...){
		strResult = "读卡异常2";
	}
	FreeLibrary(gt1);
	gt1 = NULL;
	for(vector<string>::const_iterator iter = outV.begin(); iter != outV.end(); ++iter){
		strResult += iter->c_str();
		strResult += "|";
	}
	
	return strResult;
}


string CReader_D3::WriteRFID_D3(LPCTSTR codeStr, LONG mode)
{
	string strResult="";
	HANDLE icdev;

	char buffer[0x400]={0};
	vector<string> V;
	string writeStr = (char*)codeStr;

	HINSTANCE gt1 = NULL;
	lpinit initdc = NULL;
	lpcard carddc = NULL;
	lploadkey loadkeydc = NULL;
	lpauthentication authenticationdc = NULL;
	lpread readdc= NULL;
	lpwrite writedc = NULL;
	lpbeep beepdc = NULL;	
	lpexit exitdc = NULL;
	lpreset resetdc = NULL;
	lpchangb3 changeb3dc =NULL;

	while (true){
		if (writeStr.length() > 0){
			V.push_back(writeStr.substr(0,writeStr.find("|")));
			writeStr = writeStr.substr(writeStr.find("|")+1);
			continue;
		}
		break;
	}

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

		gt1=LoadLibrary("dcrf32.dll");
		if(!gt1){
			DWORD er = GetLastError();
			throw(exception("调用读写器动态连接库(dcrf32.dll)失败!"));
		}

		initdc= (lpinit)GetProcAddress(gt1,"dc_init");
		loadkeydc = (lploadkey)GetProcAddress(gt1,"dc_load_key");
		carddc= (lpcard)GetProcAddress(gt1,"dc_card");	
		authenticationdc= (lpauthentication)GetProcAddress(gt1,"dc_authentication");	
		readdc= (lpread)GetProcAddress(gt1,"dc_read");
		writedc= (lpwrite)GetProcAddress(gt1,"dc_write");
		beepdc= (lpbeep)GetProcAddress(gt1,"dc_beep");
		exitdc= (lpexit)GetProcAddress(gt1,"dc_exit");
		changeb3dc= (lpchangb3)GetProcAddress(gt1,"dc_changeb3");
		//if (!initdc||!carddc||!loadkeydc||!authenticationdc||!readdc||!writedc||!beepdc||!exitdc)
		if (!initdc||!carddc||!loadkeydc||!authenticationdc||!readdc||!writedc||!beepdc||!exitdc||!changeb3dc)
		{
			throw(exception("查找D3读、写卡函数失败!"));
		}

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

		icdev = initdc(100,0);//usb初始化
		if ((int)icdev < 0){throw("初始化失败");}
		//read card
		unsigned long snr;
		unsigned char tpassword[6];
		unsigned char defaultPassword[6] = {0xFF,0xFF,0xFF,0xFF,0xFF,0xFF};
		unsigned char oldPassword[6] = {0xD5,0xC5,0xB3,0xCF,0xCE,0xE4};
		unsigned char initBuf[16] = {0xD5,0xC5,0xB3,0xCF,0xCE,0xE4,0x7F,0x07,0x88,0x69,0x6E,0x65,0x6F,0x77,0x65,0x72};
		int encryptType=-1;//0未初始化 1单密码 2双密码 单密码指只修改了KeyA
		unsigned char tmpBuf[0x100];
		//未初始化
		loadkeydc(icdev, 0,1,defaultPassword);
		if( carddc(icdev,0,&snr) == 1 ){ exitdc(icdev); throw(exception("请放入卡"));}         
		if(authenticationdc(icdev,0,1) == 0){
			
				encryptType = 0; 
				//AfxMessageBox("0");
			
		}else{	
			loadkeydc(icdev, 4,5,defaultPassword);//注意4区别 0 1 2  4 5 6 表示读卡器中每个区块存放有三套密码，分别为套0的Akey，套1的Akey，套2的Akey，套0的Bkey，套1的Bkey，套2的Bkey
			if( carddc(icdev,0,&snr) == 1 ){ exitdc(icdev); throw(exception("请放入卡"));}         
			if(authenticationdc(icdev,4,5) == 0){						
				encryptType = 1;//单密码 	
				//AfxMessageBox("1");
			}
			else{
				encryptType = 2; 	//双密码  这个就可以不用了判断了  如果不是上述两种类型 就是这个类型				
			}
		}
	
		if (-1 == encryptType)
		{
			strResult = "请放入卡";
		}
		//test
		if (0 == encryptType || 1 == encryptType)
		{
			//加密空白区域
			for (int j = 1; j<= 15; j++){
				bool re_auth=true;
				#if 1
				loadkeydc(icdev, 4,j,defaultPassword);
				if( carddc(icdev,0,&snr) == 1 ){ exitdc(icdev); throw(exception("请放入卡"));}         
				if(authenticationdc(icdev,4,j) != 0){//注意0区别
					throw(exception("auth error！"));
				}else{
					re_auth= true;
				}
				#endif

				if(re_auth){//写了两次
					
					if(writedc(icdev,j*4+3,initBuf) != 0){//从0x17 十进制23
						if(writedc(icdev,j*4+3,initBuf) != 0){
							exitdc(icdev); 
							throw(exception("写数据错误1！"));
						}	
					}
					//AfxMessageBox("1");
				}else{
					throw(exception("写数据错误2！"));
				}		
			}
			//加密完成
		}
		for( int i = 1; i <= 4; i++ ){			
			//if( 1 == mode){
				if( 0){
				memcpy((unsigned char*)tpassword,(unsigned char*)defaultPassword,6);//初始化
				loadkeydc(icdev, 0,i,tpassword);
			}else{
				memcpy((unsigned char*)tpassword,(unsigned char*)oldPassword,6);//重置
				loadkeydc(icdev, 0,i,tpassword);
			}
		}

		for( int i = 1; i <= 4; i++ ){			
			bool re_auth=false;
			if( carddc(icdev,0,&snr) == 1 ){ exitdc(icdev); throw(exception("请放入卡"));}         
			if(authenticationdc(icdev,0,i) != 0){
				if( carddc(icdev,0,&snr) == 1 ){ exitdc(icdev); throw(exception("请放入卡"));}
				if(authenticationdc(icdev,1,i) == 0){ 
					re_auth= true;
				}
			}else{
				re_auth= true;
			}
			if(re_auth){
				int num=0;
				if ( 2 == encryptType ) num = 2;//已经是双密码的  只写数据 不写控制区
				else num = 3;//未初始化卡和单密码卡 全部写
				for (int j = 0; j <= 2 ; j++)
				//for (int j = 0; j <= num ; j++)
				{
					//if (j < 3)
					//{
					//AfxMessageBox("2");
					if(writedc(icdev,i*4+j,(unsigned char*)(buffer+((i-1)*4+j)*16)) != 0)
					{
						if(writedc(icdev,i*4+j,(unsigned char*)(buffer+((i-1)*4+j)*16)) != 0)
						{
							exitdc(icdev); 
							throw(exception("写数据错误！"));
						}
					}
					//}
				}
				//AfxMessageBox("3");
			}else{
				exitdc(icdev); 
				throw(exception("认证错误！"));
			}
		}

		//鸣叫100毫秒
		beepdc(icdev,10);
		
		exitdc(icdev);
	}catch(const exception& e){
		strResult = e.what();
	}
	catch(...){
		//throw(exception("loadkeydc error"));
		strResult = "写卡异常";
	}
	FreeLibrary(gt1);
	gt1 = NULL;
	return strResult/*.AllocSysString()*/;
}