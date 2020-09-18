#pragma once

#include "vector"
#include "3dobj.h"

class CPlaneLibrary
{
private:
 struct rPlaneType {
   char m_PlaneType[16];
   char m_FileName[256];
   bool m_bDefault;
   double m_dWingspan; // used only with the default plane. otherwise 0.
   C3DObj *m_p3DObj;
   };
 struct rPlaneProxy {
   char m_PlaneType[16];
   char m_ExistingPlaneType[16];
   };

protected:
 std::vector<rPlaneType>m_Planes;
 std::vector<rPlaneProxy>m_PlaneProxies;
public:

private:
 // get a supported (existing modelled) plabe type from a user requested type. Proxies are used. returns an index into m_Planes or -1 on error
 short GetExistingPlaneTypeFromType(const char *p_pTypeName);

protected:
public:
 CPlaneLibrary();
 ~CPlaneLibrary();
 // adds a type to be potentially loadable
 void Add(const char *p_pTypeName, const char *p_pFileName, bool p_bDefault, double p_dWingspan);
 // loads a 3D model from disk
 bool Load(const char *p_pTypeName, QGLContext *p_pContext);
 void Unload(QGLContext *p_pContext);
 C3DObj *GetPlaneObjectByType(const char *p_pTypeName, double *p_pdWingspan = NULL);
 // add a plane proxy - a model close enough to display for the type
 void AddProxy(const char *p_pTypeName, const char *p_pExistingTypeName);
 // aux methods for other classes
 size_t GetPlaneCount() { return(m_Planes.size()); }
 const char *GetPlaneFileNameByIndex(size_t p_nIndex);
 void SetDrawFlags(C3DObj::rObjDraw *p_prObjDraw);
};
