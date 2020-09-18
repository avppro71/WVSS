#include "stdafx.h"
#include "WVSSInternetConnection.h"
using namespace WVSS;
CWVSSInternetSession::CWVSSInternetSession(LPCTSTR pstrAgent, DWORD_PTR dwContext, DWORD dwAccessType, LPCTSTR pstrProxyName, LPCTSTR pstrProxyBypass, DWORD dwFlags) 
                                           : CInternetSession(pstrAgent, dwContext, dwAccessType, pstrProxyName, pstrProxyBypass, dwFlags)
{
}
void CWVSSInternetSession::OnStatusCallback(DWORD_PTR p_dwContext, DWORD p_dwInternetStatus, LPVOID , DWORD )
{
 AFX_MANAGE_STATE(AfxGetAppModuleState());
 p_dwInternetStatus = p_dwInternetStatus;
 if(p_dwContext == GetContext() && p_dwInternetStatus == INTERNET_STATUS_RESPONSE_RECEIVED) {
   }
}
