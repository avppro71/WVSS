#pragma once
#define AVP_SWP_CMDLINE_ARG_TYPE_COUNT 5
#include <vector>
class CSWPCommandLineInfo : public CCommandLineInfo
{
public:
 enum OperType { NONE, OPENFILE, POSITION, INTERPOLATION, FILETYPE, MAKEOUTPUTFILES };
 struct rOperation { OperType m_OperType; char m_Data[MAX_PATH]; };
 static const char *m_pOperCmdText[AVP_SWP_CMDLINE_ARG_TYPE_COUNT];
 static const char *m_pOperDesc[AVP_SWP_CMDLINE_ARG_TYPE_COUNT];
 static const bool m_baDataRequired[AVP_SWP_CMDLINE_ARG_TYPE_COUNT];
private:
 rOperation m_Operation;
protected:
public:
 std::vector<rOperation> m_StartupOperations;
private:
protected:
public:
 CSWPCommandLineInfo() { }
 ~CSWPCommandLineInfo() { }
 virtual void ParseParam(const char* pszParam, BOOL bFlag, BOOL bLast);
};
