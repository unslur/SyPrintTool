#pragma once
#include "Funcs.h"

class CReader_Cmrf801u
{
public:
	CReader_Cmrf801u(void);
	~CReader_Cmrf801u(void);

	bool QuestReader801InUse();
	string ReadRFID_CMRF801U(LONG overTime);
	string WriteRFID_CMRF801U(LPCTSTR codeStr, LONG mode);
};
