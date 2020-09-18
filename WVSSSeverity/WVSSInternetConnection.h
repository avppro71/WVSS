#pragma once
#include <afxinet.h>
namespace WVSS
{
class CWVSSInternetSession : public CInternetSession
{
private:
protected:
public:
private:
protected:
public:
 CWVSSInternetSession(LPCTSTR pstrAgent = NULL, DWORD_PTR dwContext = 1, DWORD dwAccessType = PRE_CONFIG_INTERNET_ACCESS, LPCTSTR pstrProxyName = NULL, LPCTSTR pstrProxyBypass = NULL, DWORD dwFlags = 0);
 virtual void OnStatusCallback(DWORD_PTR dwContext, DWORD dwInternetStatus, LPVOID lpvStatusInformation, DWORD dwStatusInformationLength);
};
} 
