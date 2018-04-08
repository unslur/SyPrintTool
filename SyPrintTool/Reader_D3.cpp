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

//��ѯD3�Ƿ�����
bool CReader_D3::QuestReaderD3InUse()
{
	HINSTANCE gt1 = NULL;
	try{
		gt1=LoadLibrary("dcrf32.dll");

		if(!gt1){
			DWORD er = GetLastError();
			//AfxMessageBox("2");
			throw(exception("���ö�д����̬���ӿ�(dcrf32.dll)ʧ��!"));
		}
		initdc= (lpinit)GetProcAddress(gt1,"dc_init");
		carddc= (lpcard)GetProcAddress(gt1,"dc_card");	
		loadkeydc= (lploadkey)GetProcAddress(gt1,"dc_load_key");	
		exitdc= (lpexit)GetProcAddress(gt1,"dc_exit");
		if (!initdc||!carddc||!loadkeydc||!exitdc)
		{	
			//AfxMessageBox("22");
			throw(exception("����D3����д������ʧ��!"));
		}
	
		unsigned long snr;
		HANDLE icdev;
		icdev = initdc(100,0);//usb��ʼ��
		if ((int)icdev < 0)	{
			
			throw("��ʼ��ʧ��");}
		carddc(icdev,0,&snr);
		#if 0   //��ʾ��id��
 		char buff[32] ;
		itoa(snr,buff,16);
		AfxMessageBox(buff);
		#endif
		//δ��ʼ��
		unsigned char defaultPass[6] = {0xFF,0xFF,0xFF,0xFF,0xFF,0xFF};
		unsigned char oldPassword[6] = {0xD5,0xC5,0xB3,0xCF,0xCE,0xE4};
		loadkeydc(icdev, 0,1,oldPassword);//1��ʾ������1��M1��������0~15
		if( carddc(icdev,0,&snr) == 1 )
		{
			loadkeydc(icdev, 0,1,defaultPass);
			if( carddc(icdev,0,&snr) == 1 )
			{
				exitdc(icdev); 
				//AfxMessageBox("11");
				throw(exception("����뿨"));
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
	//S50��������
	unsigned char defaultPass[6] = {0xFF,0xFF,0xFF,0xFF,0xFF,0xFF};
	unsigned char oldPassword[6] = {0xD5,0xC5,0xB3,0xCF,0xCE,0xE4};
	unsigned char keyB[6] = {0x6E,0x65,0x6F,0x77,0x65,0x72};//����B
	unsigned char controlKey[4] = {0x7F,0x07,0x88,0x69};//�����ֽ�
	unsigned char initBuf[16] = {0xD5,0xC5,0xB3,0xCF,0xCE,0xE4,0x7F,0x07,0x88,0x69,0x6E,0x65,0x6F,0x77,0x65,0x72};
	char buffer[0x40]={0};
	char temp[0x40]={0};
	int st = 0;
	int encryptType=-1;//0Ĭ������ 1 ������ 2˫����
	HINSTANCE gt1 = NULL;
	try{
		gt1=LoadLibrary("dcrf32.dll");

		if(!gt1){
			DWORD er = GetLastError();
			throw(exception("���ö�д����̬���ӿ�(dcrf32.dll)ʧ��!"));
		}
		initdc= (lpinit)GetProcAddress(gt1,"dc_init");
		carddc= (lpcard)GetProcAddress(gt1,"dc_card");	
		loadkeydc= (lploadkey)GetProcAddress(gt1,"dc_load_key");	
		authenticationdc= (lpauthentication)GetProcAddress(gt1,"dc_authentication");	
		readdc= (lpread)GetProcAddress(gt1,"dc_read");	
		writedc= (lpwrite)GetProcAddress(gt1,"dc_write");
		beepdc= (lpbeep)GetProcAddress(gt1,"dc_beep");
		exitdc= (lpexit)GetProcAddress(gt1,"dc_exit");
		changeb3dc = (lpchangb3)GetProcAddress(gt1,"dc_changeb3");//����޸Ŀ�3�ĺ������ﵽ���ܹ���
		if (!initdc||!carddc||!loadkeydc||!authenticationdc||!readdc||!writedc||!beepdc||!exitdc||!changeb3dc)
		//if (!initdc||!carddc||!loadkeydc||!authenticationdc||!readdc||!writedc||!beepdc||!exitdc)
		{
			throw(exception("����D3����д������ʧ��!"));
		}

		unsigned long snr;
		icdev = initdc(100,0);//usb��ʼ��
		if ((int)icdev < 0)	{throw("��ʼ��ʧ��");}

		//test
		char tmpBuf[256]={0};
		
		//δ��ʼ��
		loadkeydc(icdev, 0,1,defaultPass);
		if( carddc(icdev,0,&snr) == 1 ){ exitdc(icdev); throw(exception("����뿨"));}       //MODEΪ1�ɷ���Ѱ��, MODEΪ0�������ÿ��ſ����²���!  
		if(authenticationdc(icdev,0,1) == 0){
			if (0 == readdc(icdev,0x7,(unsigned char*)tmpBuf))//0x7
			{
				encryptType = 0;
				throw(exception("this_card_is_uninit!"));
			}
		}
		//������
		loadkeydc(icdev, 0,1,oldPassword);
		if( carddc(icdev,0,&snr) == 1 ){ exitdc(icdev); throw(exception("����뿨"));}         
		if(authenticationdc(icdev,0,1) == 0){
			if (0 == readdc(icdev,0x7,(unsigned char*)tmpBuf))//0x7
			{
				loadkeydc(icdev, 4,5,defaultPass);
				if( carddc(icdev,0,&snr) == 1 ){ exitdc(icdev); throw(exception("����뿨"));}         
				if(authenticationdc(icdev,4,5) == 0){
					if (0 == readdc(icdev,0x17,(unsigned char*)tmpBuf))//0x17
					{
						encryptType = 1;
						//�ڴ�ȫ������
						for (int j = 1; j<= 15; j++){

							#if 0//ֱ��ʹ�����ӿ⺯�����м���
							if (changeb3dc(icdev,j,oldPassword,0x00,0x00,0x00,0x11,0,keyB)!=0);
							{
								exitdc(icdev);
								throw(exception("д���ܴ���"));
							}
							#endif




							#if 1
							bool re_auth=false;
							loadkeydc(icdev, 4,j,defaultPass);
							if( carddc(icdev,0,&snr) == 1 ){ exitdc(icdev); throw(exception("����뿨"));}         
							if(authenticationdc(icdev,4,j) != 0){//ע��0����
								throw(exception("auth error��"));
							}else{
								re_auth= true;
							}

							
							#if 0
							loadkeydc(icdev,0,j,defaultPass);
							if( carddc(icdev,0,&snr) == 1 ){ exitdc(icdev); throw(exception("����뿨"));}      //   
							if(authenticationdc(icdev,0,j) != 0){//ע��0����
								if( carddc(icdev,0,&snr) == 1 ){ exitdc(icdev); throw(exception("����뿨"));}
								if(authenticationdc(icdev,1,j) == 0){//ע��1���� 
									re_auth= true;
								}
							}else{
								re_auth= true;
							}
							#endif

							if(re_auth){//д������
								if(writedc(icdev,j*4+3,initBuf) != 0){//��0x17 ʮ����23
									if(writedc(icdev,j*4+3,initBuf) != 0){
										exitdc(icdev); 
										throw(exception("д���ݴ���1��"));
									}	
								}
							}else{
								throw(exception("д���ݴ���2��"));
							}	
							#endif	
						}
						//������
					}
				}
			}
		}
		
		//read card
		for( int i = 1; i <= 4; i++ ){
			loadkeydc(icdev, 0,i,oldPassword);
		}
		if( carddc(icdev,0,&snr) == 1 ){ exitdc(icdev); throw(exception("����뿨"));}
		if(authenticationdc(icdev,0,1) == 0){
			for(int k=0;k<overTime;k++){
				if(readdc(icdev,4,(unsigned char*)buffer)==0){
					strncpy(temp,buffer,8);temp[8] = 0;
					outV.push_back(temp); //������(8/8)	
					strncpy(temp,buffer+8,8);temp[8] = 0;
					outV.push_back(temp);//��ҵ����(8/14)
					break;
				}
			}
			for(int k=0;k<overTime;k++){
				if(readdc(icdev,5,(unsigned char*)buffer)==0){
					strncpy(temp,buffer,6);temp[6] = 0;
					outV[outV.size()-1] += temp; //��ҵ����(14/14)
					strncpy(temp,buffer+6,10);temp[10] = 0;
					outV.push_back(temp); //��ҵ����(10/48)
					break;
				}
			}
			for(int k=0;k<overTime;k++){
				if(readdc(icdev,6,(unsigned char*)buffer)==0){
					strncpy(temp,buffer,16);temp[16] = 0;
					outV[outV.size()-1] += temp; //��ҵ����(16/48)
					break;
				}
			}
		}
		if( carddc(icdev,0,&snr) == 1 ){ exitdc(icdev); throw(exception("����뿨"));}
		if(authenticationdc(icdev,0,2) == 0){
			for(int k=0;k<overTime;k++){
				if(readdc(icdev,8,(unsigned char*)buffer)==0){
					strncpy(temp,buffer,16);temp[16] = 0;
					outV[outV.size()-1] += temp; //��ҵ����(16/48)
					break;
				}
			}
			for(int k=0;k<overTime;k++){
				if(readdc(icdev,9,(unsigned char*)buffer)==0){
					strncpy(temp,buffer,6);temp[6] = 0;
					outV[outV.size()-1] += temp;; //��ҵ����(48/48)
					strncpy(temp,buffer+6,10);temp[10] = 0;
					outV.push_back(temp);  //��ϵ�绰(10/30)
					break;
				}
			}
			for(int k=0;k<overTime;k++){
				if(readdc(icdev,10,(unsigned char*)buffer)==0){
					strncpy(temp,buffer,16);temp[16] = 0;
					outV[outV.size()-1] += temp;  //��ϵ�绰(16/30)
					break;
				}
			}
		}
		if( carddc(icdev,0,&snr) == 1 ){ exitdc(icdev); throw(exception("����뿨"));}
		if(authenticationdc(icdev,0,3) == 0){
			for(int k=0;k<overTime;k++){
				if(readdc(icdev,12,(unsigned char*)buffer)==0){
					strncpy(temp,buffer,4);temp[4] = 0;
					outV[outV.size()-1] += temp; //��ϵ�绰(30/30)
					strncpy(temp,buffer+4,12);temp[12] = 0;
					outV.push_back(temp); //��Դ���豸����(12/32)
					break;
				}
			}
			for(int k=0;k<overTime;k++){
				if(readdc(icdev,13,(unsigned char*)buffer)==0){
					strncpy(temp,buffer,16);temp[16] = 0;
					outV[outV.size()-1] += temp; //��Դ���豸����(16/32)
					break;
				}
			}
			for(int k=0;k<overTime;k++){
				if(readdc(icdev,14,(unsigned char*)buffer)==0){
					strncpy(temp,buffer,4);temp[4] = 0;
					outV[outV.size()-1] += temp; //��Դ���豸����(32/32)
					strncpy(temp,buffer+4,12);temp[12] = 0;
					outV.push_back(temp); //�г�����(12/48)
					break;
				}
			}
		}
		if( carddc(icdev,0,&snr) == 1 ){ exitdc(icdev); throw(exception("����뿨"));}
		if(authenticationdc(icdev,0,4) == 0){
			for(int k=0;k<overTime;k++){
				if(readdc(icdev,16,(unsigned char*)buffer)==0){
					strncpy(temp,buffer,16);temp[16] = 0;
					outV[outV.size()-1] += temp; //�г����� (16 28/48)
					break;
				}
			}
			for(int k=0;k<overTime;k++){
				if(readdc(icdev,17,(unsigned char*)buffer)==0){
					strncpy(temp,buffer,16);temp[16] = 0;
					outV[outV.size()-1] += temp; //�г����� (16 44/48)
					break;
				}
			}
			for(int k=0;k<overTime;k++){
				if(readdc(icdev,18,(unsigned char*)buffer)==0){
					strncpy(temp,buffer,4);temp[4] = 0;
					outV[outV.size()-1] += temp; //�г����� (4 48/48)
					strncpy(temp,buffer+4,12);temp[12] = 0;
					outV.push_back(temp); 	//�ڲ���ţ�1�ֽڣ�
					break;
				}
			}
		}
		
		//����100����
		beepdc(icdev,overTime);
		exitdc(icdev);
	}catch(const exception& e){
		//cout << e.what() << endl;
		strResult = e.what();
	}
	catch(...){
		strResult = "�����쳣2";
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
			throw(exception("������������"));
		if( V[0].size() != 8 )
			throw(exception("������ λ������"));
		if (V[1].size() != 14)
			throw(exception("��ҵ���� λ������"));
		if (V[2].size() > 48)
			throw(exception("��ҵ���� ����"));
		if(V[3].size() > 30)
			throw(exception("��ϵ�绰����"));
		if(V[4].size() != 32)
			throw(exception("��Դ���豸���� λ������"));
		if(V[5].size() > 48)
			throw(exception("�г����� ����"));
		if(V[6].size()!=1)
			throw(exception("�ڲ���� λ������"));

		gt1=LoadLibrary("dcrf32.dll");
		if(!gt1){
			DWORD er = GetLastError();
			throw(exception("���ö�д����̬���ӿ�(dcrf32.dll)ʧ��!"));
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
			throw(exception("����D3����д������ʧ��!"));
		}

		char* p = buffer;

		strcpy(p,V[0].c_str()); //������(8/8)
		strcpy(p+=8,V[1].c_str()); //��ҵ����part1(8/14)
		strcpy(p+=14,V[2].substr(0,26).c_str()); //��ҵ����(26/48)
		p += 26;
		*p++ = 0xD5;*p++ = 0xC5;*p++ = 0xB3;*p++ = 0xCF;*p++ = 0xCE;*p++ = 0xE4;*p++ = 0x7F;*p++ = 0x07;*p++ = 0x88;*p++ = 0x69;*p++ = 0x6E;*p++ = 0x65;*p++ = 0x6F;*p++ = 0x77;*p++ = 0x65;*p++ = 0x72;
		if(V[2].length()>26)                     //��ҵ����(22/48)
			strcpy(p,V[2].substr(26).c_str());
		strcpy(p+=22,V[3].substr(0,26).c_str()); //��ϵ�绰(26/30)
		p += 26;
		*p++ = 0xD5;*p++ = 0xC5;*p++ = 0xB3;*p++ = 0xCF;*p++ = 0xCE;*p++ = 0xE4;*p++ = 0x7F;*p++ = 0x07;*p++ = 0x88;*p++ = 0x69;*p++ = 0x6E;*p++ = 0x65;*p++ = 0x6F;*p++ = 0x77;*p++ = 0x65;*p++ = 0x72;
		if (V[3].length()>26)                    //��ϵ�绰(4/30)
			strcpy(p,V[3].substr(26).c_str());
		strcpy(p+=4,V[4].c_str());              //�豸����(32/32)
		strcpy(p+=32,V[5].substr(0,12).c_str()); //�г�����(12/48)
		p += 12;
		*p++ = 0xD5;*p++ = 0xC5;*p++ = 0xB3;*p++ = 0xCF;*p++ = 0xCE;*p++ = 0xE4;*p++ = 0x7F;*p++ = 0x07;*p++ = 0x88;*p++ = 0x69;*p++ = 0x6E;*p++ = 0x65;*p++ = 0x6F;*p++ = 0x77;*p++ = 0x65;*p++ = 0x72;
		if (V[5].length()>12)                   //�г�����(36/48)
			strcpy(p,V[5].substr(12).c_str());
		strcpy(p+=36,V[6].c_str());             //�ڲ����(1/1)
		p += 12;
		*p++ = 0xD5;*p++ = 0xC5;*p++ = 0xB3;*p++ = 0xCF;*p++ = 0xCE;*p++ = 0xE4;*p++ = 0x7F;*p++ = 0x07;*p++ = 0x88;*p++ = 0x69;*p++ = 0x6E;*p++ = 0x65;*p++ = 0x6F;*p++ = 0x77;*p++ = 0x65;*p++ = 0x72;

		icdev = initdc(100,0);//usb��ʼ��
		if ((int)icdev < 0){throw("��ʼ��ʧ��");}
		//read card
		unsigned long snr;
		unsigned char tpassword[6];
		unsigned char defaultPassword[6] = {0xFF,0xFF,0xFF,0xFF,0xFF,0xFF};
		unsigned char oldPassword[6] = {0xD5,0xC5,0xB3,0xCF,0xCE,0xE4};
		unsigned char initBuf[16] = {0xD5,0xC5,0xB3,0xCF,0xCE,0xE4,0x7F,0x07,0x88,0x69,0x6E,0x65,0x6F,0x77,0x65,0x72};
		int encryptType=-1;//0δ��ʼ�� 1������ 2˫���� ������ָֻ�޸���KeyA
		unsigned char tmpBuf[0x100];
		//δ��ʼ��
		loadkeydc(icdev, 0,1,defaultPassword);
		if( carddc(icdev,0,&snr) == 1 ){ exitdc(icdev); throw(exception("����뿨"));}         
		if(authenticationdc(icdev,0,1) == 0){
			
				encryptType = 0; 
				//AfxMessageBox("0");
			
		}else{	
			loadkeydc(icdev, 4,5,defaultPassword);//ע��4���� 0 1 2  4 5 6 ��ʾ��������ÿ�����������������룬�ֱ�Ϊ��0��Akey����1��Akey����2��Akey����0��Bkey����1��Bkey����2��Bkey
			if( carddc(icdev,0,&snr) == 1 ){ exitdc(icdev); throw(exception("����뿨"));}         
			if(authenticationdc(icdev,4,5) == 0){						
				encryptType = 1;//������ 	
				//AfxMessageBox("1");
			}
			else{
				encryptType = 2; 	//˫����  ����Ϳ��Բ������ж���  ������������������� �����������				
			}
		}
	
		if (-1 == encryptType)
		{
			strResult = "����뿨";
		}
		//test
		if (0 == encryptType || 1 == encryptType)
		{
			//���ܿհ�����
			for (int j = 1; j<= 15; j++){
				bool re_auth=true;
				#if 1
				loadkeydc(icdev, 4,j,defaultPassword);
				if( carddc(icdev,0,&snr) == 1 ){ exitdc(icdev); throw(exception("����뿨"));}         
				if(authenticationdc(icdev,4,j) != 0){//ע��0����
					throw(exception("auth error��"));
				}else{
					re_auth= true;
				}
				#endif

				if(re_auth){//д������
					
					if(writedc(icdev,j*4+3,initBuf) != 0){//��0x17 ʮ����23
						if(writedc(icdev,j*4+3,initBuf) != 0){
							exitdc(icdev); 
							throw(exception("д���ݴ���1��"));
						}	
					}
					//AfxMessageBox("1");
				}else{
					throw(exception("д���ݴ���2��"));
				}		
			}
			//�������
		}
		for( int i = 1; i <= 4; i++ ){			
			//if( 1 == mode){
				if( 0){
				memcpy((unsigned char*)tpassword,(unsigned char*)defaultPassword,6);//��ʼ��
				loadkeydc(icdev, 0,i,tpassword);
			}else{
				memcpy((unsigned char*)tpassword,(unsigned char*)oldPassword,6);//����
				loadkeydc(icdev, 0,i,tpassword);
			}
		}

		for( int i = 1; i <= 4; i++ ){			
			bool re_auth=false;
			if( carddc(icdev,0,&snr) == 1 ){ exitdc(icdev); throw(exception("����뿨"));}         
			if(authenticationdc(icdev,0,i) != 0){
				if( carddc(icdev,0,&snr) == 1 ){ exitdc(icdev); throw(exception("����뿨"));}
				if(authenticationdc(icdev,1,i) == 0){ 
					re_auth= true;
				}
			}else{
				re_auth= true;
			}
			if(re_auth){
				int num=0;
				if ( 2 == encryptType ) num = 2;//�Ѿ���˫�����  ֻд���� ��д������
				else num = 3;//δ��ʼ�����͵����뿨 ȫ��д
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
							throw(exception("д���ݴ���"));
						}
					}
					//}
				}
				//AfxMessageBox("3");
			}else{
				exitdc(icdev); 
				throw(exception("��֤����"));
			}
		}

		//����100����
		beepdc(icdev,10);
		
		exitdc(icdev);
	}catch(const exception& e){
		strResult = e.what();
	}
	catch(...){
		//throw(exception("loadkeydc error"));
		strResult = "д���쳣";
	}
	FreeLibrary(gt1);
	gt1 = NULL;
	return strResult/*.AllocSysString()*/;
}