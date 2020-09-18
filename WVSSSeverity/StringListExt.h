#pragma once
namespace WVSS {
class CStringListExt : public CStringList
{
protected:
 CString m_Path;
public:
 CStringListExt() { }
 virtual ~CStringListExt() { }
 void MakePath();
 POSITION AddTail(const CString &p_NewElement) { POSITION Pos = CStringList::AddTail(p_NewElement); MakePath(); return(Pos); }
 CString RemoveTail() { CString Str = CStringList::RemoveTail(); MakePath(); return(Str); }
 LPCTSTR GetPath() { return(m_Path); }
};
} 
