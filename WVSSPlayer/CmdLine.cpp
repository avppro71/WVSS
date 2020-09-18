#include "stdafx.h"
#include "CmdLine.h"
#pragma warning(disable : 4996)
const char *CSWPCommandLineInfo::m_pOperCmdText[] = {
 "File",
 "Pos",
 "Interpolation",
 "FileType",
 "GenerateOutputFiles"
};
const char *CSWPCommandLineInfo::m_pOperDesc[] = {
 "<file name> - Open file",
 "<number> - Set starting frame, 0-based, max - number of frames in the file with current interpolation",
 "<number> Interpolation value in 1/10s increments. Default is 10 (1 second)",
 "<number> File type: 1 - WVSS CSV, 2 - FDR, 3 - WVSS JSON, 4 - Multiformat (a file name per line), 5 - MITRE, 6 - ASAIC JSON",
 "Generate output files"
};
const bool CSWPCommandLineInfo::m_baDataRequired[] = { true, true, true, true, true };
void CSWPCommandLineInfo::ParseParam(const char *p_pParam, BOOL p_bFlag, BOOL p_bLast)
{
 short i, ArgsCount;
 bool FlagFound;
 char tstr[2 * MAX_PATH];
 DWORD lVal;
 ArgsCount = sizeof(m_pOperCmdText) / sizeof(const char *);
 if(p_bFlag) {
   FlagFound = false;
   for(i = 0; i < ArgsCount; i ++) {
     if(!_stricmp(p_pParam, m_pOperCmdText[i])) {
       m_Operation.m_OperType = (OperType)(i + 1);
       FlagFound = true;
       break; } }
   if(!FlagFound) {
     m_Operation.m_OperType = NONE;
     sprintf(tstr, "Command line argument %s is not supported", p_pParam);
     TRACE(tstr); }
   if(FlagFound) {
     memset(m_Operation.m_Data, 0, sizeof(m_Operation.m_Data));
     if(!m_baDataRequired[i]) {
       m_StartupOperations.push_back(m_Operation);
       m_Operation.m_OperType = NONE;
       return; } } }
 else {
   if(m_Operation.m_OperType != NONE) {
     strncpy(m_Operation.m_Data, p_pParam, sizeof(m_Operation.m_Data) - 1);
     switch(m_Operation.m_OperType) {
       case OPENFILE:
         strcpy(tstr, m_Operation.m_Data);
         lVal = GetFullPathName(tstr, sizeof(tstr), m_Operation.m_Data, NULL);
         if(GetFileAttributes(m_Operation.m_Data) == INVALID_FILE_ATTRIBUTES) {
           sprintf(tstr, "File %s does not exist", m_Operation.m_Data);
           AfxGetMainWnd()->MessageBox(tstr);
           return; }
       case POSITION:
       case INTERPOLATION:
       case MAKEOUTPUTFILES:
       default: break; }
     m_StartupOperations.push_back(m_Operation); } }
}
