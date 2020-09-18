#include "MaterialLib.h"
#include "assert.h"

#pragma warning (disable: 4996)

CMaterialLib::CMaterialLib()
{
 m_pMaterialArray = NULL;
 m_pCurMaterial = NULL;
 Reset();
}

CMaterialLib::~CMaterialLib()
{
 Reset();
}

void CMaterialLib::Reset()
{
 if(m_pMaterialArray) delete [] m_pMaterialArray;
 m_pMaterialArray = NULL;
 m_pCurMaterial = NULL;
 m_lMaterialArraySize = 0;
 m_lMaterialCount = 0;
}

bool CMaterialLib::OnBeforeLoad(const char *p_pFileName)
{
 m_FileParts.ParseFileName(p_pFileName);
 return(true);
}

bool CMaterialLib::OnLoaded()
{
 return(true);
}

bool CMaterialLib::ProcessLine(char *p_pLine)
{
 char ch[2];
 //rf3 R3;
 rMaterial Mat;

 if(!p_pLine) return(false);

 ch[0] = *p_pLine;
 ch[1] = *(p_pLine + 1);
 switch(ch[0]) {
   case 'n':
     if(!strnicmp(p_pLine, "newmtl", 6)) {
       if(!ParseToStr(p_pLine, Mat.m_Name, sizeof(Mat.m_Name), true)) return(false);
       Mat.m_TextureID = 0;
       Mat.m_bTexture = false;
       memset(Mat.m_TextureFileName, 0, sizeof(Mat.m_TextureFileName));
       if(!AddToArray(MATERIALLIB_ARRTYPE_MATERIAL, &Mat)) return(false); }
     break;
   case 'm':
     if(!strnicmp(p_pLine, "map_Kd", 6))
       if(!Process_map_Kd(p_pLine + 6)) return(false);
     break;
   default:
     break;
   }
 return(true);
}

bool CMaterialLib::AddSpace(eArrayType p_Type)
{
 unsigned long lNewSize;
 rMaterial *pMaterialArray;

 switch(p_Type) {
   case MATERIALLIB_ARRTYPE_MATERIAL:
     lNewSize = m_lMaterialArraySize + 10;
     pMaterialArray = new rMaterial[lNewSize];
     if(!pMaterialArray) return(false);
     memset(pMaterialArray, 0, sizeof(rMaterial) * lNewSize);
     if(m_lMaterialCount)
       memcpy(pMaterialArray, m_pMaterialArray, sizeof(rMaterial) * m_lMaterialCount);
     if(m_pMaterialArray) delete [] m_pMaterialArray;
     m_pMaterialArray = pMaterialArray;
     m_lMaterialArraySize = lNewSize;
     break;
   }
 return(true);
}

short CMaterialLib::GetMaterialIndexByName(char *p_pMaterialName)
{
 unsigned long li;
 char *pWork;

 pWork = strtok(p_pMaterialName, " \t");
 if(!pWork) return(-1);

 for(li = 0; li < m_lMaterialCount; li ++) {
   if(!stricmp(pWork, m_pMaterialArray[li].m_Name))
     return(li); }
 return(-1);
}

bool CMaterialLib::AddToArray(eArrayType p_Type, void *p_pSrc)
{
 switch(p_Type) {
   case MATERIALLIB_ARRTYPE_MATERIAL:
     if(m_lMaterialArraySize == m_lMaterialCount)
       if(!AddSpace(p_Type)) return(false);
     m_pMaterialArray[m_lMaterialCount] = *(rMaterial *)p_pSrc;
     m_lMaterialCount ++;
     m_pCurMaterial = &m_pMaterialArray[m_lMaterialCount - 1];
     break;
   }

 return(true);
}

bool CMaterialLib::Process_map_Kd(char *p_pLine)
{
 char tstr[256];
 CPathParts FileParts;

 if(!m_pCurMaterial) { assert(false); return(false); }
 if(!ParseToStr(p_pLine, tstr, sizeof(tstr), true)) return(false);
 if(*tstr) {
   sprintf(m_pCurMaterial->m_TextureFileName, "%s/%s", m_FileParts.m_Path, tstr);
   FileParts.ParseFileName(m_pCurMaterial->m_TextureFileName);
   sprintf(m_pCurMaterial->m_TextureFileName, FileParts.FullName().toLocal8Bit());
   //QMessageBox::information(NULL, "Texture file", m_pCurMaterial->m_TextureFileName);
   }
 return(true);
}

bool CMaterialLib::LoadTextures(QGLContext *p_pContext)
{
 unsigned long li;

 //QMessageBox::information(NULL, "LoadTextures", "LoadTextures");

 for(li = 0; li < m_lMaterialCount; li ++) {
   if(*m_pMaterialArray[li].m_TextureFileName) {
     m_pMaterialArray[li].m_TextureID = p_pContext->bindTexture(QPixmap(QString(m_pMaterialArray[li].m_TextureFileName)), GL_TEXTURE_2D);
     //QMessageBox::information(NULL, "p_pContext->bindTexture", m_pMaterialArray[li].m_TextureFileName);
     //QMessageBox::information(NULL, "p_pContext->bindTexture2", QString::number(m_pMaterialArray[li].m_TextureID));
     m_pMaterialArray[li].m_bTexture = (m_pMaterialArray[li].m_TextureID != 0);
     }
   }
 return(true);
}

bool CMaterialLib::UnloadTextures(QGLContext *p_pContext)
{
 unsigned long li;

 for(li = 0; li < m_lMaterialCount; li ++) {
   if(*m_pMaterialArray[li].m_TextureFileName) {
     if(m_pMaterialArray[li].m_bTexture) {
       p_pContext->deleteTexture(m_pMaterialArray[li].m_TextureID);
       m_pMaterialArray[li].m_bTexture = false; }
     }
   }
 Reset();
 return(true);
}
