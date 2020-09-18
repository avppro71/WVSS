#include "WVSSIPCommonFunctions.h"
#include <map>

byte CalcCRC(byte *p_pBuf, int p_nLen)
{
 byte sum = CalcSum(p_pBuf, p_nLen);
 byte crc = (sum ^ 0xFF) + 0x01;
 return crc;
}

byte CalcSum(byte *p_pBuf, int p_nLen)
{
 byte sum = 0;
 int i;

 for(i = 0; i < p_nLen; i++) sum += p_pBuf[i];
 return sum;
}

std::map<int, QString>g_CommonMsgNames;

void GlobalFillMessageNames()
{
 g_CommonMsgNames[WVSS_msg_data] = "WVSS_msg_data";
 g_CommonMsgNames[WVSS_msg_area] = "WVSS_msg_area";
 g_CommonMsgNames[WVSS_msg_planepos] = "WVSS_msg_planepos";
 g_CommonMsgNames[WVSS_msg_loadplane] = "WVSS_msg_loadplane";

}

QString GlobalMessageName(int p_nMsgNum)
{
 std::map<int, QString>::iterator It;

 It = g_CommonMsgNames.find(p_nMsgNum);
 if(It == g_CommonMsgNames.end()) return("Unknown message");
 return(It->second);
}
