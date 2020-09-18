#include "stdafx.h"
#include "AppSock.h"
#define DLSOCK_DATASIZE     4 * 1024
IMPLEMENT_DYNAMIC(CAppSocket, CSocket)
CAppSocket::CAppSocket()
{
 m_bConnected = false;
 m_nTotalSent = 0;
 m_pSendData = NULL;
 m_bInSend = false;
 m_pfMessage = NULL;
 m_pfSocketConnected = NULL;
 m_pfMessageSent = NULL;
 *m_LastErrStr = '\0';
}
CAppSocket::~CAppSocket()
{
 if(m_pSendData) delete [] m_pSendData;
 m_bConnected = false;
 m_bInSend = false;
}
bool CAppSocket::DoConnect(const char *p_pAddress, UINT p_nPort)
{
 bool bVal;
 bVal = (m_hSocket == INVALID_SOCKET ? !!Create() : true);
 if(!CheckError()) {
   Message("Socket create, immediate error %d: %s", m_nLastIPError, m_LastErrStr);
   return(false); }
 bVal = !!Connect(p_pAddress, p_nPort);
 if(!CheckError()) {
   Message("Socket connect, immediate error %d: %s", m_nLastIPError, m_LastErrStr);
   return(false); }
 return(true);
}
void CAppSocket::OnConnect(int p_nErrorCode)
{
 CheckError(p_nErrorCode);
 m_bConnected = (p_nErrorCode == 0);
 if(!p_nErrorCode)
   Message("Socket connected");
 else
   Message("Socket connect, error %d: %s", p_nErrorCode, m_LastErrStr);
 if(m_pfSocketConnected)
   m_pfSocketConnected(p_nErrorCode == 0);
 CAsyncSocket::OnConnect(p_nErrorCode);
}
void CAppSocket::OnReceive(int p_nErrorCode)
{
 Message("Socket OnReceive (?)");
 CAsyncSocket::OnReceive(p_nErrorCode);
}
void CAppSocket::OnSend(int p_nErrorCode)
{
 for(;;) {
   if(!m_bInSend) break;
   SendMsg(); 
   break; }
 CAsyncSocket::OnSend(p_nErrorCode);
}
void CAppSocket::OnClose(int p_nErrorCode)
{
 Message("Socket OnClose");
 m_bConnected = false;
 m_nTotalSent = 0;
 m_pSendData = NULL;
 m_bInSend = false;
 if(m_pfSocketConnected)
   m_pfSocketConnected(false);
 CAsyncSocket::OnClose(p_nErrorCode);
 Close();
}
void CAppSocket::Message(const char *p_pFormat, ...)
{
 char tstr[1024];
 va_list ArgList;
 if(!m_pfMessage) return;
 if(!p_pFormat) return;
 va_start(ArgList, p_pFormat);
 vsprintf_s(tstr, sizeof(tstr), p_pFormat, ArgList);
 va_end(ArgList);
 m_pfMessage(tstr);
}
bool CAppSocket::CheckError(int p_nErrorCode)
{
 *m_LastErrStr = '\0';
 m_nLastIPError = p_nErrorCode;
 if(m_nLastIPError == -1) m_nLastIPError = WSAGetLastError();
 if(!m_nLastIPError || m_nLastIPError == WSAEWOULDBLOCK) return(true); 
 if(!LoadString(AfxGetInstanceHandle(), m_nLastIPError + 40000, m_LastErrStr, sizeof(m_LastErrStr)))
   sprintf_s(m_LastErrStr, sizeof(m_LastErrStr), "Unknown Winsock Error %ld", (long)m_nLastIPError);
 return(false);
}
bool CAppSocket::SendMsg(CWVSSMsgHeader *p_pData)
{
 int Sent;
 if(p_pData) {
   if(m_bInSend || !IsConnected()) { 
     delete [] (char *)p_pData; 
     return(false); } 
   m_pSendData = p_pData;
   m_pSendData->BeforeSend(); } 
 else
   p_pData = p_pData;
 m_bInSend = true;
 Sent = Send((char *)m_pSendData + m_nTotalSent, m_pSendData->msg_size - m_nTotalSent);
 if(Sent == SOCKET_ERROR) {
   CheckError(); 
   if(m_nLastIPError == WSAEWOULDBLOCK) 
     return(true);
   else {
     delete [] (char *)m_pSendData; 
     m_pSendData = NULL;
     m_nTotalSent = 0;
     m_bInSend = false;
     Message("Send failed with %d: %s", m_nLastIPError, m_LastErrStr);
     Close();
     return(false); } }
 m_nTotalSent += Sent;
 if(m_nTotalSent != m_pSendData->msg_size)
   m_nTotalSent = m_nTotalSent;
 if(m_nTotalSent == m_pSendData->msg_size) {
   if(m_pfMessageSent) m_pfMessageSent(m_pSendData->msg_type);
   delete [] (char *)m_pSendData; 
   m_pSendData = NULL;
   m_nTotalSent = 0;
   m_bInSend = false; }
 return(TRUE);
}
