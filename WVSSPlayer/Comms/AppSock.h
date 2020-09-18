#ifndef INC_WVSS_APP_SOCKET
#define INC_WVSS_APP_SOCKET
#include "afxsock.h"
#include "WVSSIPProtocol.h"
class CAppSocket : public CAsyncSocket
{
 DECLARE_DYNAMIC(CAppSocket);
private:
 bool m_bConnected;
 int m_nLastIPError;
 int m_nTotalSent;
 CWVSSMsgHeader *m_pSendData;
 bool m_bInSend;
 char m_LastErrStr[1024];
protected:
public:
 void (*m_pfMessage)(const char *);
 void (*m_pfSocketConnected)(bool p_bConnected);
 void (*m_pfMessageSent)(short p_nMsgType);
private:
 void Message(const char *p_pFormat, ...);
 bool CheckError(int p_nErrorCode = -1);
protected:
 virtual void OnConnect(int p_nErrorCode);
 virtual void OnReceive(int p_nErrorCode);
 virtual void OnSend(int p_nErrorCode);
 virtual void OnClose(int p_nErrorCode);
public:
 CAppSocket();
 ~CAppSocket();
 bool DoConnect(const char *p_pAddress, UINT p_nPort); 
 inline BOOL IsConnected() { return(m_bConnected); }
 bool SendMsg(CWVSSMsgHeader *p_pData = NULL);
 bool IsAvailableToSend() { return(!m_bInSend); }
};
#endif
