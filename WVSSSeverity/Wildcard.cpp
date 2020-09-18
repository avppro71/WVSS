#include "stdafx.h"
#include "Wildcard.h"
CWildcardException CWildcard::m_Exc;
const char *CWildcard::m_RusUpr = "ÀÁÂÃÄÅ¨ÆÇÈÉÊËÌÍÎÏĞÑÒÓÔÕÖ×ØÙÜÚÛİŞß";
const char *CWildcard::m_RusLwr = "àáâãäå¸æçèéêëìíîïğñòóôõö÷øùüúûışÿ";
BOOL CWildcard::DoMatch(LPCSTR p_Name, LPCSTR Pattern, const BOOL p_bCaseInsensitive)
{
 if(!*Pattern) return(*p_Name == NULL); 
 if(*Pattern == '*') {
   Pattern ++;
   while(*p_Name && *Pattern) {
     if(AreCharsEqual(*Pattern, *p_Name, p_bCaseInsensitive) || *p_Name == '*') { 
       if(Match(p_Name + 1, Pattern + 1)) {
         return TRUE; } }
      p_Name ++; }
   return MatchOkay(Pattern); }
 if(*Pattern == '?') {
   if(*p_Name) { 
     if(Match(p_Name + 1, Pattern + 1))
         return TRUE;
     return(MatchOkay(Pattern)); } }
 while(*p_Name && *Pattern != '*' && *Pattern != '?') {
   if(AreCharsEqual(*Pattern, *p_Name, p_bCaseInsensitive)) {
     Pattern ++;
     p_Name ++; }
   else return FALSE; }
 if(*p_Name)
   return Match(p_Name, Pattern);
 return MatchOkay(Pattern);
}
BOOL CWildcard::MatchOkay(LPCSTR Pattern)
{
 if(*Pattern && *Pattern != '*')
   return FALSE;
 return TRUE; 
}
BOOL CWildcard::Match(LPCSTR p_pString, LPCSTR p_pPattern, BOOL p_bCaseInsensitive)
{
   return(DoMatch(p_pString, p_pPattern, p_bCaseInsensitive));
}
CWildcardException *CWildcardException::SetText(LPCTSTR p_pFormat, ...)
{
 CString Str;
 va_list ArgList;
 ASSERT(AfxIsValidString(p_pFormat));
 va_start(ArgList, p_pFormat);
 Str.FormatV(p_pFormat, ArgList);
 va_end(ArgList); 
 strcpy_s(m_ErrorText, _countof(m_ErrorText), (LPCSTR)Str);
 return(this);
}
BOOL CWildcard::AreCharsEqual(const char p_Char1, const char p_Char2, BOOL p_bCaseInsensitive)
{
 if(!p_bCaseInsensitive) return(p_Char1 == p_Char2);
 if(toupper(p_Char1) == toupper(p_Char2)) return(TRUE);
 const char *pInd1, *pInd2;
 short FirstFlag, Ind1, Ind2;
 pInd1 = strchr(m_RusUpr, p_Char1);
 if(pInd1) { FirstFlag = 0; Ind1 = (short)(pInd1 - m_RusUpr); }
 else {
   pInd1 = strchr(m_RusLwr, p_Char1);
   if(pInd1) { FirstFlag = 1; Ind1 = (short)(pInd1 - m_RusLwr); }
   else return(FALSE); } 
 if(!FirstFlag) { 
   pInd2 = strchr(m_RusLwr, p_Char2);
   if(!pInd2) return(FALSE); 
   Ind2 = (short)(pInd2 - m_RusLwr); }
 else { 
   pInd2 = strchr(m_RusUpr, p_Char2);
   if(!pInd2) return(FALSE); 
   Ind2 = (short)(pInd2 - m_RusUpr); }
 return(Ind1 == Ind2);
}
