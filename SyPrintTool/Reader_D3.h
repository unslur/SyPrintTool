#pragma once
#include "Funcs.h"

class CReader_D3
{
public:
	CReader_D3(void);
	~CReader_D3(void);

	bool QuestReaderD3InUse();
	string ReadRFID_D3(LONG overTime);
	string WriteRFID_D3(LPCTSTR codeStr, LONG mode);

};
