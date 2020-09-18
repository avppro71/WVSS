#pragma once

#include "LineParser.h"
#include "3DObjLineParser.h"

class CMaterialLib : public C3DObjLineParser
{
private:
protected:
 enum eArrayType { MATERIALLIB_ARRTYPE_MATERIAL };
 CPathParts m_FileParts;

public:
 struct rMaterial {
   char m_Name[32];
   char m_TextureFileName[256];
   bool m_bTexture;
   unsigned int m_TextureID;
   };
 rMaterial *m_pMaterialArray, *m_pCurMaterial;
 unsigned long m_lMaterialArraySize, m_lMaterialCount;
 

private:
protected:
 void Reset();
 bool AddSpace(eArrayType p_Type);
 bool AddToArray(eArrayType p_Type, void *p_pSrc);
 virtual bool ProcessLine(char *p_pLine);
 virtual bool OnBeforeLoad(const char *p_pFileName);
 virtual bool OnLoaded();
 bool Process_map_Kd(char *p_pLine);
 
public:
 CMaterialLib();
 ~CMaterialLib();
 short GetMaterialIndexByName(char  *p_pMaterialName); // -1 on error
 bool LoadTextures(QGLContext *p_pContext);
 bool UnloadTextures(QGLContext *p_pContext);
};
