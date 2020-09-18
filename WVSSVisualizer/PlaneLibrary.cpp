#include "PlaneLibrary.h"

#pragma warning (disable: 4996)

CPlaneLibrary::CPlaneLibrary()
{

}

CPlaneLibrary::~CPlaneLibrary()
{
}

void CPlaneLibrary::Add(const char *p_pTypeName, const char *p_pFileName, bool p_bDefault, double p_dWingspan)
{
 short i;
 rPlaneType PT;

 for(i = 0; i < (short)m_Planes.size(); i ++) {
   if(!stricmp(m_Planes[i].m_PlaneType, p_pTypeName))
     break; }
 if(i != m_Planes.size()) return; // already added
 memset(PT.m_PlaneType, 0, sizeof(PT.m_PlaneType));
 strncpy(PT.m_PlaneType, p_pTypeName, sizeof(PT.m_PlaneType) - 1);
 memset(PT.m_FileName, 0, sizeof(PT.m_FileName));
 strncpy(PT.m_FileName, p_pFileName, sizeof(PT.m_FileName) - 1);
 PT.m_bDefault = p_bDefault;
 PT.m_dWingspan = p_dWingspan;
 PT.m_p3DObj = NULL;
 m_Planes.push_back(PT);
}

void CPlaneLibrary::Unload(QGLContext *p_pContext)
{
 short i;
 for(i = 0; i < (short)m_Planes.size(); i ++) {
   if(m_Planes[i].m_p3DObj) {
     m_Planes[i].m_p3DObj->Unload(p_pContext);
     delete m_Planes[i].m_p3DObj; }
   }
 m_Planes.clear();
 m_PlaneProxies.clear();
}

bool CPlaneLibrary::Load(const char *p_pTypeName, QGLContext *p_pContext)
{
 bool bVal;
 short PlaneIndex;

 PlaneIndex = GetExistingPlaneTypeFromType(p_pTypeName);
 if(PlaneIndex == -1) return(false);

 if(m_Planes[PlaneIndex].m_p3DObj)
   return(true); // already loaded. If load failed before and the pointer, while not NULL, doesn't have a model, do no attempt to load again - it will fail anyway

 m_Planes[PlaneIndex].m_p3DObj = new C3DObj();
 if(!m_Planes[PlaneIndex].m_p3DObj) return(false);

 bVal = m_Planes[PlaneIndex].m_p3DObj->Load(m_Planes[PlaneIndex].m_FileName);
 if(bVal)
   m_Planes[PlaneIndex].m_p3DObj->LoadTextures(p_pContext);
 return(bVal);
}

C3DObj *CPlaneLibrary::GetPlaneObjectByType(const char *p_pTypeName, double *p_pdWingspan /* = NULL */)
{
 short PlaneIndex;

 if(p_pdWingspan) *p_pdWingspan = 0;
 PlaneIndex = GetExistingPlaneTypeFromType(p_pTypeName);
 if(PlaneIndex == -1) return(NULL);
 
 if(p_pdWingspan)
   *p_pdWingspan = m_Planes[PlaneIndex].m_dWingspan;
 return(m_Planes[PlaneIndex].m_p3DObj);
}

short CPlaneLibrary::GetExistingPlaneTypeFromType(const char *p_pTypeName)
// get a supported (existing modelled) plane type from a user requested type. Proxies are used. Returns an index into m_Planes or -1 on error
// If a default is set, the default will be returned when no exact match and no proxy match is made.
{
 short i, j, DefaultIndex = -1;
 const char *pTypeName = p_pTypeName;
 
 for(i = 0; i < 2; i ++) {
   for(j = 0; j < (short)m_Planes.size(); j ++) {
     if(m_Planes[j].m_bDefault) DefaultIndex = j;
     if(!stricmp(m_Planes[j].m_PlaneType, pTypeName))
       return(j); }
   // type not found, try proxies
   if(i) break; // only try to find a proxy after the first run
   for(j = 0; j < (short)m_PlaneProxies.size(); j ++) { // try to find a proxy
     if(!stricmp(m_PlaneProxies[j].m_PlaneType, p_pTypeName)) {
       pTypeName = m_PlaneProxies[j].m_ExistingPlaneType;
       break;
       } } }
 
 return(DefaultIndex);
}

void CPlaneLibrary::AddProxy(const char *p_pTypeName, const char *p_pExistingTypeName)
// add a plane proxy - a model close enough to display for the type
{
 short i;
 rPlaneProxy PP;

 for(i = 0; i < (short)m_PlaneProxies.size(); i ++) {
   if(!stricmp(m_Planes[i].m_PlaneType, p_pTypeName))
     break; }
 
 if(i != m_PlaneProxies.size()) return; // already added
 memset(PP.m_PlaneType, 0, sizeof(PP.m_PlaneType));
 strncpy(PP.m_PlaneType, p_pTypeName, sizeof(PP.m_PlaneType) - 1);
 memset(PP.m_ExistingPlaneType, 0, sizeof(PP.m_ExistingPlaneType));
 strncpy(PP.m_ExistingPlaneType, p_pExistingTypeName, sizeof(PP.m_ExistingPlaneType) - 1);
 m_PlaneProxies.push_back(PP);
}

const char *CPlaneLibrary::GetPlaneFileNameByIndex(size_t p_nIndex)
{
 if(p_nIndex > m_Planes.size() - 1) return(NULL);
 return(m_Planes[p_nIndex].m_FileName);
}

void CPlaneLibrary::SetDrawFlags(C3DObj::rObjDraw *p_prObjDraw)
{
 size_t i;

 for(i = 0; i < m_Planes.size(); i ++) {
   if(m_Planes[i].m_p3DObj)
     m_Planes[i].m_p3DObj->SetDrawFlags(p_prObjDraw);
   }
}
