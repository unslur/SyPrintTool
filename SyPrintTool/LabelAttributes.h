#pragma once
struct Location
{
	bool enable;
	double x;
	double y;
	double size;
};
#include "stdafx.h"
class LabelAttributes
{
public:
	LabelAttributes(void);
	~LabelAttributes(void);
	bool LoadConfiguration();
	bool TSCLoadConfiguration();
	double spacebetween;
	double originpoint[2];
	bool Rotation;//true ÕýÏò
	Location listLocation[13];
	int labelWidth;
	int labelHeight;
	int splitenumber;
	int concentration;
	double codeheight;
	int number;
};

