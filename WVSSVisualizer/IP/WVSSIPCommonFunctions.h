#ifndef INC_LIDAR_COMMON_FUNCTIONS
#define INC_LIDAR_COMMON_FUNCTIONS

#include "qstring.h"
#include "WVSSIPProtocol.h"

typedef unsigned char byte;

byte CalcCRC(byte *p_pBuf, int p_nLen);
byte CalcSum(byte *p_pBuf, int p_nLen);

void GlobalFillMessageNames();
QString GlobalMessageName(int p_nMsgNum);

unsigned char XORCRC(unsigned char *p_pBytes, unsigned long p_lBytes);

#endif
