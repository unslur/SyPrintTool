#include "LabelAttributes.h"

#define configFilePath "./config.ini"
LabelAttributes::LabelAttributes(void)
{
}


LabelAttributes::~LabelAttributes(void)
{
}
bool LabelAttributes:: LoadConfiguration(){
	FILE *fp=fopen("config.ini","r");
	if (fp==NULL)
	{
		return false;
	}else
	{
		fclose(fp);
	}
	spacebetween=GetPrivateProfileInt("Attributes","pagewidth",508,configFilePath)*0.0394;
	Rotation=GetPrivateProfileInt("Attributes","Rotation",0,configFilePath);
	concentration=GetPrivateProfileInt("Attributes","concentration",0,configFilePath);
	originpoint[0]=GetPrivateProfileInt("Attributes","x",0,configFilePath)*0.0394;
	originpoint[1]=GetPrivateProfileInt("Attributes","y",0,configFilePath)*0.0394;
	labelWidth=GetPrivateProfileInt("Attributes","width",0,configFilePath)*0.0394;
	splitenumber=GetPrivateProfileInt("Attributes","code",0,configFilePath);
	codeheight=GetPrivateProfileInt("Attributes","codeheight",0,configFilePath)*0.0394;
	char num[4]={0};
	for (int i=0;i<13;i++){
	Location one;
	sprintf(num,"%d",i);
	one.enable= GetPrivateProfileInt(num,"enable",0,configFilePath);
	one.x=GetPrivateProfileInt(num,"x",0,configFilePath)*0.0394;
	one.y=GetPrivateProfileInt(num,"y",0,configFilePath)*0.0394;
	one.size=GetPrivateProfileInt(num,"size",0,configFilePath);
	listLocation[i]=one;
	}
	return true;
}
bool LabelAttributes::TSCLoadConfiguration() {
	/*FILE *fp = fopen("D:\projectCy\SyTool_new_Ë®ÃÛÌÒ\SyPrintTool\Debug\config.ini", "r");
	if (fp == NULL)
	{
		return false;
	}
	else
	{
		fclose(fp);
	}*/
	spacebetween = GetPrivateProfileInt("Attributes", "pagewidth", 508, configFilePath);
	Rotation = GetPrivateProfileInt("Attributes", "Rotation", 0, configFilePath);
	concentration = GetPrivateProfileInt("Attributes", "concentration", 0, configFilePath);
	originpoint[0] = GetPrivateProfileInt("Attributes", "x", 0, configFilePath);
	originpoint[1] = GetPrivateProfileInt("Attributes", "y", 0, configFilePath);
	labelWidth = GetPrivateProfileInt("Attributes", "width", 60, configFilePath);
	labelHeight = GetPrivateProfileInt("Attributes", "height", 40, configFilePath);
	splitenumber = GetPrivateProfileInt("Attributes", "code", 0, configFilePath);
	codeheight = GetPrivateProfileInt("Attributes", "codeheight", 0, configFilePath);
	number = GetPrivateProfileInt("Attributes", "number", 0, configFilePath);
	char num[4] = { 0 };
	//char info[10] = { 0 };
	for (int i = 0; i<number; i++) {
		Location one;
		sprintf(num, "%d", i);
		one.enable = GetPrivateProfileInt(num, "enable", 1, configFilePath);

		
		one.x = GetPrivateProfileInt(num, "x", 860, configFilePath);
		if (one.x< 1){
			return false; }
		/*sprintf(info, "%.2f", one.x);
		MessageBox(NULL,info, NULL, MB_OK);*/
		one.y = GetPrivateProfileInt(num, "y", 160, configFilePath);
		one.size = GetPrivateProfileInt(num, "size", 14, configFilePath);
		listLocation[i] = one;
	}
	return true;
}
