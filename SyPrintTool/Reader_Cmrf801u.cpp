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
			throw(exception("���ö�д����̬���ӿ�(function.dll)ʧ��!"));
		}
		unsigned char tpassword[6]={0};
		unsigned char defaultPass[6] = {0xFF,0xFF,0xFF,0xFF,0xFF,0xFF};
		unsigned char oldPassword[6] = {0xD5,0xC5,0xB3,0xCF,0xCE,0xE4};
		typedef int (WINAPI MF_Write_T)(int mode, int blk_add, int num_blk, BYTE *buf, BYTE *buffer);
		MF_Write_T* MF_Write = (MF_Write_T*)GetProcAddress(hInstMaster, "MF_Write");
		if (!MF_Write)
		{
			throw(exception("����д������ʧ��!"));
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

//��ѯD3�Ƿ�����
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
			throw(exception("���Ҷ�������ʧ��!"));
		}

		char buffer[0x400],temp[0x40];
		BYTE tmpBuf[0x100];
		unsigned char tpassword[6];
		int encryptFlag = -1;
		//�ж��Ƿ���δ��ʼ���Ŀ� û���κ�����  
		int ret=0;
		memcpy(tpassword,oldPassword,6);//
		ret = MF_Read(0, 7, 1, tpassword, (unsigned char *)buffer);//���ڶ����ĵ�һ��
		if (0 != ret)
		{
			memcpy(tpassword,defaultPass,6);//
			ret = MF_Read(0, 7, 1, tpassword, (unsigned char *)buffer);//���ڶ����ĵ�һ��
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
			throw(exception("���Ҷ�д������ʧ��!"));
		}

		char buffer[0x400],temp[0x40];
		BYTE tmpBuf[0x100];
		unsigned char tpassword[6];
		//�ж��Ƿ���δ��ʼ���Ŀ� û���κ�����  
		int ret=0;
		memcpy(tpassword,defaultPass,6);//
		ret = MF_Read(0, 7, 1, tpassword, (unsigned char *)buffer);//���ڶ����ĵ�һ��
		if (0 == ret) {
			throw(exception("�˿�Ϊ�¿�,���ȳ�ʼ��!"));
		}

		if (-1 == encryptFlag)
		{
			//˫���뿨
			//ret=0;
			for (int i=1;i<=2;i++){//�����ȡʧ�� �������
				memcpy(tpassword,oldPassword,6);
				if (MF_Read(0, 0x17, 1, tpassword, tmpBuf) != 0) {
					Sleep(100);
					continue;
				}
				encryptFlag = 2;//˫���뿨
				break;
			}
		}
		//�ж��Ƿ�Ϊ�ɵ�д����ʽ  ������
		if (-1 == encryptFlag)
		{
			//�ж��Ƿ�Ϊ�ɵ�д����ʽ  ������
			memcpy(tpassword,oldPassword,6);
			if (0 == MF_Read(0, 7, 1, tpassword, tmpBuf)) {
				memcpy(tpassword,defaultPass,6);
				if (0 == MF_Read(0, 0x17, 1, tpassword, tmpBuf)) {
					encryptFlag = 1;//�����뿨
				}
			}
		}
		if (-1 == encryptFlag)
		{
			strResult = "����뿨��";
		}
		if(1 == encryptFlag)
		{//�Ǿɵ�д����ʽ ��Ϊ�ɵĴ�14֮�����õ�ff ff������
			//����Ǿ�д������ ���޸�Ϊ˫����
			for(int i=1;i<=2;i++)//������� ��д3��
			{
				//�õ������޸������ݵ�ǰ��4����
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
			//���޸ĺ����7��û�����ݵ�����
			EncryptNoUseBlock(encryptFlag);//����û��ʹ�õ�����
		}
		if (2 == encryptFlag)
		{
			//������µ�˫���� �����Ѿ����ϵĵ����봦��Ϊ˫����֮�� ��ʼ������
			retFlag = false;
			for (int i = 0; i < overTime; i++){
				memcpy((unsigned char*)tpassword,(unsigned char*)oldPassword,6);//֮ǰֻ���޸��˼��ܷ�ʽΪ˫���� ����Ȼ���þ������
				if( MF_Read(0, 4, 4, tpassword, (unsigned char*)buffer) != 0 ){
					Sleep(100);
					continue;
				}		
				strncpy(temp,buffer,8);temp[8] = 0;
				outV.push_back(temp); //������(8)
				strncpy(temp,buffer+8,14);temp[14] = 0;
				outV.push_back(temp); //��ҵ����(14)
				strncpy(temp,buffer+22,26);temp[26] = 0;
				outV.push_back(temp); //��ҵ����(26/48)		
				retFlag = true;
				break;
			}
			if (!retFlag) throw(exception("��������"));

			retFlag = false;
			for (int i = 0; i < overTime; i++){
				memcpy((unsigned char*)tpassword,(unsigned char*)oldPassword,6);//֮ǰֻ���޸��˼��ܷ�ʽΪ˫���� ����Ȼ���þ������
				if( MF_Read(0, 8, 4, (unsigned char*)tpassword, (unsigned char*)buffer)  != 0 ){
					Sleep(100);
					continue;
				}
				//memcpy((unsigned char*)tpassword,(unsigned char*)defaultPass,6);//֮ǰֻ���޸��˼��ܷ�ʽΪ˫���� ����Ȼ���þ������
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
				outV[outV.size()-1] += temp; //��ҵ����(22/48)
				strncpy(temp,buffer+22,26);temp[26] = 0;
				outV.push_back(temp);  //��ϵ�绰(26/30)
				retFlag = true;
				break;
			}
			if (!retFlag) throw(exception("��������"));

			retFlag = false;
			for (int i = 0; i < overTime; i++){
				memcpy((unsigned char*)tpassword,(unsigned char*)oldPassword,6);//֮ǰֻ���޸��˼��ܷ�ʽΪ˫���� ����Ȼ���þ������
				if( MF_Read(0, 0x0C, 4, (unsigned char*)tpassword, (unsigned char*)buffer)  != 0 ){
					Sleep(100);
					continue;
				}
				strncpy(temp,buffer,4);temp[4] = 0;
				outV[outV.size()-1] += temp; //��ϵ�绰(4/30)
				strncpy(temp,buffer+4,32);temp[32] = 0;
				outV.push_back(temp); //��Դ���豸����(32/32)
				strncpy(temp,buffer+36,12);temp[12] = 0;
				outV.push_back(temp); //�г�����(12/48)
				retFlag = true;
				break;
			}
			if (!retFlag) throw(exception("��������"));

			retFlag = false;
			for (int i = 0; i < overTime; i++){
				memcpy((unsigned char*)tpassword,(unsigned char*)oldPassword,6);
				if( MF_Read(0, 0x10, 4, (unsigned char*)tpassword, (unsigned char*)buffer)  != 0 ){
					Sleep(100);
					continue;
				}
				strncpy(temp,buffer,36);temp[36] = 0;
				outV[outV.size()-1] += temp; //�г����� (36 48/48)
				strncpy(temp,buffer+36,1);temp[1] = 0;
				outV.push_back(temp);  //�ڲ���ţ�1�ֽڣ�
				retFlag = true;
				break;
			}
			if (!retFlag) throw(exception("��������"));
		}
		//char a[10];
		//sprintf(a,"%d",encryptFlag);
		//AfxMessageBox(a);
	}catch(const exception& e){
		//cout << e.what() << endl;
		strResult = e.what();
	}catch(...){
		strResult = "��������";
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
	int encryptFlag = -1;// 0�����¿���ʼ�� 1�����Ͽ����� 2����˫���뿨����
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

		hInstMaster = LoadLibrary("function.dll");
		if(!hInstMaster){
			DWORD er = GetLastError();
			throw(exception("���ö�д����̬���ӿ�(function.dll)ʧ��!"));
		}

		unsigned char tmpBuf[0x100]={0};
		char buffer[0x100]={0};
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

		typedef int (WINAPI MF_Write_T)(int mode, int blk_add, int num_blk, BYTE *buf, BYTE *buffer);
		MF_Write_T* MF_Write = (MF_Write_T*)GetProcAddress(hInstMaster, "MF_Write");
		typedef int (WINAPI MF_Read_T)(int mode, int blk_add, int num_blk, BYTE *buf, BYTE *buffer);
		MF_Read_T* MF_Read = (MF_Read_T*)GetProcAddress(hInstMaster, "MF_Read");
		if (NULL == MF_Read || NULL == MF_Write)
		{
			throw(exception("���Ҷ���д������ʧ��!"));
		}

		//�ж��Ƿ���δ��ʼ���Ŀ� û���κ�����  
		int ret=0;
		for (int i=1;i<=1;i++){//�����ȡʧ�� �������
			memcpy(tpassword,defaultPassword,6);//
			ret = MF_Read(0, 7, 1, tpassword, tmpBuf);//���ڶ����ĵ�һ��
			if (ret != 0) {
				Sleep(100);
				continue;
			}
			encryptFlag = 0;//δ��ʼ����
			break;
		}
		if (-1 == encryptFlag)
		{
			//˫���뿨
			ret=0;
			for (int i=1;i<=2;i++){//�����ȡʧ�� �������
				memcpy(tpassword,oldPassword,6);
				ret = MF_Read(0, 0x17, 1, tpassword, tmpBuf);
				if (ret != 0) {
					Sleep(100);
					continue;
				}
				encryptFlag = 2;//˫���뿨
				break;
			}
		}
		if (-1 == encryptFlag)
		{
			//�ж��Ƿ�Ϊ�ɵ�д����ʽ  ������
			ret=0;
			memcpy(tpassword,oldPassword,6);
			ret = MF_Read(0, 7, 1, tpassword, tmpBuf);
			if (0 == ret) {
				memcpy(tpassword,defaultPassword,6);
				ret = MF_Read(0, 0x17, 1, tpassword, tmpBuf);
				if (0 == ret) {
					encryptFlag = 1;//�����뿨
					//��ȡʧ��  �����Ǿɵ�д����ʽ ��Ϊ�ɵĴ�14֮�����õ�ff ff������
					//����Ǿ�д������ ���޸�Ϊ˫����
				}
			}
		}
		if (-1 == encryptFlag)
		{
			strResult = "����뿨��";
		}
		//1���  ȫ������   �ڳ�ʼ����ʱ�������õ�ʱ��Ҳ����д  2ѭ��д3�� 
		if (0 == encryptFlag)
		{
			for( int i = 1; i <= 4; i++ )
			{	
				for(int j = 1;j<=3;j++)
				{
					//unsigned char tmpBuf[0x100];
					memcpy(tmpBuf,buffer+(i-1)*4*16,0x100);
					memcpy((unsigned char*)tpassword,(unsigned char*)defaultPassword,6);
					if( MF_Write(0, i*4, 4, tpassword, tmpBuf) != 0 )//Ĭ������д						
					{
						Sleep(100);
						continue;
					}
					else strResult = "";
					break;	
				}				
			}
			EncryptNoUseBlock(encryptFlag);//����û��ʹ�õ�����
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
					//ע�� 4 ���� 3
					ret = 0;
					if (1 == encryptFlag)//������� ȫ���޸�
					{
						ret = MF_Write(0, i*4, 4, tpassword, tmpBuf);
					}
					if (2 == encryptFlag)//��������µ�˫������  ֻ�޸����ݾͿ����� ����ֻдÿ������3��  ��4Ҳ�����޸ĳɹ� ֻ����new����ſ����޸�
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
				EncryptNoUseBlock(encryptFlag);//����û��ʹ�õ�����
			}
		}
	}catch(const exception& e){
		strResult = e.what();
    }
	catch(...){
		strResult = "д���쳣";
	}

	FreeLibrary(hInstMaster);
	return strResult;
}