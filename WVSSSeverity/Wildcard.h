#ifndef INC_WILDCARD
#define INC_WILDCARD
class CWildcardException : public CException
{
friend class CWildcard;
private:
 char m_ErrorText[256];
protected:
public:
private:
 CWildcardException *SetText(LPCTSTR p_pFormat, ...);
protected:
public:
 CWildcardException() { }
 ~CWildcardException() { }
 LPCSTR GetErrorText() { return(m_ErrorText); }
};
class CWildcard : public CObject
{
private:
 static CWildcardException m_Exc;
 static const char *m_RusUpr;
 static const char *m_RusLwr;
protected:
public:
private:
 static BOOL MatchOkay(LPCSTR Pattern);
 static BOOL DoMatch(LPCSTR p_pString, LPCSTR p_pPattern, BOOL p_bCaseInsensitive = FALSE);
 static BOOL AreCharsEqual(const char p_Char1, const char p_Char2, const BOOL p_bCaseInsensitive);
protected:
public:
 CWildcard() { }
 ~CWildcard() { }
 static BOOL Match(LPCSTR p_pString, LPCSTR p_pPattern, BOOL p_bCaseInsensitive = FALSE);
};
#endif
