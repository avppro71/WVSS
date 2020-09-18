#include "stdafx.h"
#include "StringListExt.h"
using namespace WVSS;
void CStringListExt::MakePath()
{
 POSITION Pos;
 m_Path = "";
 if(IsEmpty()) return;
 Pos = GetHeadPosition();
 while(Pos != NULL) {
   m_Path += GetNext(Pos);
   if(Pos != NULL)
     m_Path += "/";
   } 
}
