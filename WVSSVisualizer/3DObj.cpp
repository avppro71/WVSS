#include "3DObj.h"
#include "assert.h"

#pragma warning (disable: 4996)

C3DObj::C3DObj()
{
 m_pVertexArray = NULL;
 m_pTextureArray = NULL;
 m_pIndexArray = NULL;
 m_pObjDescArray = NULL;
 m_pCurObjDesc = NULL;
 m_pStraightIndexArray = NULL;
 m_pStraightTextureArray = NULL;
 Reset();
}

C3DObj::~C3DObj()
{
 Reset();
}

void C3DObj::Reset()
{
 if(m_pVertexArray) delete [] m_pVertexArray;
 m_pVertexArray = NULL;
 if(m_pTextureArray) delete [] m_pTextureArray;
 m_pTextureArray = NULL;
 if(m_pIndexArray) delete [] m_pIndexArray;
 m_pIndexArray = NULL;
 if(m_pObjDescArray) delete [] m_pObjDescArray;
 m_pObjDescArray = NULL;
 m_pCurObjDesc = NULL;
 if(m_pStraightIndexArray) delete [] m_pStraightIndexArray;
 m_pStraightIndexArray = NULL;
 if(m_pStraightTextureArray) delete [] m_pStraightTextureArray;
 m_pStraightTextureArray = NULL;
 m_lVertexArraySize = m_lVertexCount = 0;
 m_lTextureArraySize = m_lTextureCount = 0;
 m_lIndexArraySize = m_lIndexCount = 0;
 m_lObjDescArraySize = m_lObjDescCount = 0;
 m_lVertexArrayResizeCount = 0;
 m_lTextureArrayResizeCount = 0;
 m_lIndexArrayResizeCount = 0;
}

bool C3DObj::OnBeforeLoad(const char *p_pFileName)
{
 Reset();
 m_lTotalVerticesAdded = 0;
 m_FileParts.ParseFileName(p_pFileName);
 return(true);
}

bool C3DObj::OnLoaded()
{
 unsigned long li;

 CreateUniqueVertexTextureIndices(); // create unique vertex for the last object

 if(m_lIndexCount) {
   m_pStraightIndexArray = new GLuint [m_lIndexCount];
   if(!m_pStraightIndexArray) return(false);
   memset(m_pStraightIndexArray, 0, sizeof(GLuint) * m_lIndexCount);
   for(li = 0; li < m_lIndexCount; li ++)
     m_pStraightIndexArray[li] = m_pIndexArray[li].VertexIndex;
   m_pStraightTextureArray = new rf2[m_lVertexCount];
   if(!m_pStraightTextureArray) return(false);
   memset(m_pStraightTextureArray, 0, sizeof(rf2) * m_lVertexCount);
   for(li = 0; li < m_lIndexCount; li ++)
     m_pStraightTextureArray[m_pIndexArray[li].VertexIndex] = m_pTextureArray[m_pIndexArray[li].TextureIndex];
   }  
 return(true);
}

bool C3DObj::ProcessLine(char *p_pLine)
{
 char ch[2];
 rf3 R3;
 ri3 I3[3];
 rObjDesc OD;
 char tstr[256], tstr2[256];

 if(!p_pLine) return(false);

 ch[0] = *p_pLine;
 ch[1] = *(p_pLine + 1);

 switch(ch[0]) {
   case 'v':
     if(!ParseToR3(p_pLine, &R3)) return(false);
     switch(ch[1]) {
       case 't':
         if(!AddToArray(C3DOBJ_ARRTYPE_TEXTURE, &R3)) return(false);
         assert(m_pCurObjDesc);
         m_pCurObjDesc->lTextureCount ++;
         break;
       case 'n':
         assert(m_pCurObjDesc);
         //if(!AddToArray(C3DOBJ_ARRTYPE_NORMAL, &R3)) return(false);
         //m_pCurObjDesc->lNormalCount ++;
         break;
       case ' ':
       case '\t':
         if(!m_pCurObjDesc) { // object name not specified
           sprintf(tstr, "o Object_%.3d", m_lObjDescCount + 1);
           if(!ParseToObjDesc(tstr, &OD)) return(false);
           if(!AddToArray(C3DOBJ_ARRTYPE_OBJDESC, &OD)) return(false);
           }
         if(!AddToArray(C3DOBJ_ARRTYPE_VERTEX, &R3)) return(false);
         m_pCurObjDesc->lVertexCount ++;
         break;
       }
     break;
   case 'f':
     if(!ParseTo3I3(p_pLine, I3)) return(false);
     I3[0].VertexIndex --; // OBJ files has 1-based indices, a C++ program has 0-based arrays
     I3[1].VertexIndex --;
     I3[2].VertexIndex --;
     
     if(m_lTotalVerticesAdded)
       m_lTotalVerticesAdded = m_lTotalVerticesAdded;

     I3[0].VertexIndex += m_lTotalVerticesAdded;
     I3[1].VertexIndex += m_lTotalVerticesAdded;
     I3[2].VertexIndex += m_lTotalVerticesAdded;
     
     I3[0].TextureIndex --;
     I3[1].TextureIndex --;
     I3[2].TextureIndex --;
     I3[0].NormalIndex --;
     I3[1].NormalIndex --;
     I3[2].NormalIndex --;
     if(!AddToArray(C3DOBJ_ARRTYPE_INDEX, &I3)) return(false);
     m_pCurObjDesc->lIndexCount += 3;
     break;
   case 'g':
     CreateUniqueVertexTextureIndices();
     m_pCurObjDesc = NULL; // new group. The next vertex will definitely be in a new object, whether we get an o before v or not.
     break;
   case 'o':
     CreateUniqueVertexTextureIndices(); // create unique vertex/texture pairs for the previous object
     if(!ParseToObjDesc(p_pLine, &OD)) return(false);
     if(!AddToArray(C3DOBJ_ARRTYPE_OBJDESC, &OD)) return(false);
     break;
   case 'm':
     if(!strnicmp(p_pLine, "mtllib", 6)) {
       if(!ParseToStr(p_pLine, tstr, sizeof(tstr), true)) return(false);
       sprintf(tstr2, "%s/%s", m_FileParts.m_Path, tstr);
       //QMessageBox::information(NULL, "Material file", tstr2);
       if(!m_MaterialLib.Load(tstr2)) return(false); }
     break;
   case 'u':
     if(!strnicmp(p_pLine, "usemtl", 6)) {
       assert(m_pCurObjDesc);
       m_pCurObjDesc->MaterialIndex = m_MaterialLib.GetMaterialIndexByName(p_pLine + 6);
       }
     break;

   default: break;
   }

 return(true);
}

bool C3DObj::AddSpace(eArrayType p_Type)
{
 unsigned long lNewSize;
 rf3 *pVertexArray;
 rf2 *pTextureArray;
 ri3 *pIndexArray;
 rObjDesc *pObjDescArray;

 switch(p_Type) {
   case C3DOBJ_ARRTYPE_VERTEX:
     lNewSize = (unsigned long)((float)m_lVertexArraySize * (m_lVertexArraySize < 10000 ? 2 : 1.1));
     if(lNewSize < 100) lNewSize = 100;
     pVertexArray = new rf3[lNewSize];
     if(!pVertexArray) return(false);
     memset(pVertexArray, 0, sizeof(rf3) * lNewSize);
     if(m_lVertexCount)
       memcpy(pVertexArray, m_pVertexArray, sizeof(rf3) * m_lVertexCount);
     if(m_pVertexArray) delete [] m_pVertexArray;
     m_pVertexArray = pVertexArray;
     m_lVertexArraySize = lNewSize;
     m_lVertexArrayResizeCount ++;
     break;
   case C3DOBJ_ARRTYPE_TEXTURE:
     lNewSize = (unsigned long)((float)m_lTextureArraySize * (m_lTextureArraySize < 10000 ? 2 : 1.1));
     if(lNewSize < 100) lNewSize = 100;
     pTextureArray = new rf2[lNewSize];
     if(!pTextureArray) return(false);
     memset(pTextureArray, 0, sizeof(rf2) * lNewSize);
     if(m_lTextureCount)
       memcpy(pTextureArray, m_pTextureArray, sizeof(rf2) * m_lTextureCount);
     if(m_pTextureArray) delete [] m_pTextureArray;
     m_pTextureArray = pTextureArray;
     m_lTextureArraySize = lNewSize;
     m_lTextureArrayResizeCount ++;
     break;
   case C3DOBJ_ARRTYPE_INDEX:
     lNewSize = (unsigned long)((float)m_lIndexArraySize * (m_lIndexArraySize < 10000 ? 2 : 1.1));
     if(lNewSize < 100) lNewSize = 100;
     pIndexArray = new ri3[lNewSize];
     if(!pIndexArray) return(false);
     memset(pIndexArray, 0, sizeof(ri3) * lNewSize);
     if(m_lIndexCount)
       memcpy(pIndexArray, m_pIndexArray, sizeof(ri3) * m_lIndexCount);
     if(m_pIndexArray) delete [] m_pIndexArray;
     m_pIndexArray = pIndexArray;
     m_lIndexArraySize = lNewSize;
     m_lIndexArrayResizeCount ++;
     break;
   case C3DOBJ_ARRTYPE_OBJDESC:
     lNewSize = m_lObjDescArraySize + 10;
     pObjDescArray = new rObjDesc[lNewSize];
     if(!pObjDescArray) return(false);
     memset(pObjDescArray, 0, sizeof(rObjDesc) * lNewSize);
     if(m_lObjDescCount)
       memcpy(pObjDescArray, m_pObjDescArray, sizeof(rObjDesc) * m_lObjDescCount);
     if(m_pObjDescArray) delete [] m_pObjDescArray;
     m_pObjDescArray = pObjDescArray;
     m_lObjDescArraySize = lNewSize;
     break;
   }
 return(true);
}

bool C3DObj::ParseToObjDesc(char *p_pLine, rObjDesc *p_pOD)
{
 p_pOD->lVertexArrayStart = m_lVertexCount;
 p_pOD->lTextureArrayStart = m_lTextureCount;
 p_pOD->lIndexArrayStart = m_lIndexCount;
 p_pOD->lVertexCount = 0;
 p_pOD->lTextureCount = 0;
 p_pOD->lIndexCount = 0;

 p_pOD->DrawFlags.m_bDraw = true;
 p_pOD->DrawFlags.m_Alpha = 128;
 p_pOD->DrawFlags.m_bSelectFace = false;
 p_pOD->DrawFlags.m_lSelectFaceNum = false;
 p_pOD->DrawFlags.m_bTransparent = false;
 p_pOD->DrawFlags.m_bWireframe = false;
 p_pOD->MaterialIndex = -1;

 return(ParseToStr(p_pLine, p_pOD->Name, sizeof(p_pOD->Name), true));
}

bool C3DObj::AddToArray(eArrayType p_Type, void *p_pSrc)
{
 rf3 *pR3;
 ri3 *pI3;

 switch(p_Type) {
   case C3DOBJ_ARRTYPE_VERTEX:
     if(m_lVertexArraySize == m_lVertexCount) if(!AddSpace(p_Type)) return(false);
     m_pVertexArray[m_lVertexCount] = *(rf3 *)p_pSrc;
     m_lVertexCount ++;
     break;
   case C3DOBJ_ARRTYPE_TEXTURE:
     if(m_lTextureArraySize == m_lTextureCount) if(!AddSpace(p_Type)) return(false);
     pR3 = (rf3 *)p_pSrc;
     m_pTextureArray[m_lTextureCount].x = pR3->x;
     m_pTextureArray[m_lTextureCount].y = pR3->y;
     m_lTextureCount ++;
     break;
   case C3DOBJ_ARRTYPE_INDEX:
     pI3 = (ri3 *)p_pSrc;
     if(m_lIndexArraySize == m_lIndexCount) if(!AddSpace(p_Type)) return(false);
     pI3->lOrdinal = m_lIndexCount;
     m_pIndexArray[m_lIndexCount] = *pI3;
     m_lIndexCount ++;
     ++ pI3;
     if(m_lIndexArraySize == m_lIndexCount) if(!AddSpace(p_Type)) return(false);
     pI3->lOrdinal = m_lIndexCount;
     m_pIndexArray[m_lIndexCount] = *pI3;
     m_lIndexCount ++;
     ++ pI3;
     if(m_lIndexArraySize == m_lIndexCount) if(!AddSpace(p_Type)) return(false);
     pI3->lOrdinal = m_lIndexCount;
     m_pIndexArray[m_lIndexCount] = *pI3;
     m_lIndexCount ++;
     ++ pI3;
     break;
   case C3DOBJ_ARRTYPE_OBJDESC:
     if(m_lObjDescArraySize == m_lObjDescCount)
       if(!AddSpace(p_Type)) return(false);
     m_pObjDescArray[m_lObjDescCount] = *(rObjDesc *)p_pSrc;
     m_lObjDescCount ++;
     m_pCurObjDesc = &m_pObjDescArray[m_lObjDescCount - 1];
     break;
   }

 return(true);
}

bool C3DObj::CreateUniqueVertexTextureIndices()
{
 unsigned long li, lIndexArrayEnd, lVertexAddedCount;
 ri3 CurIndexRec;
 GLuint VertexReplacementIndex;

 if(!m_lObjDescCount) return(true);
 if(!m_pCurObjDesc) return(true);
 if(m_pCurObjDesc->lIndexCount < 3) return(true);

 qsort(m_pIndexArray + m_pCurObjDesc->lIndexArrayStart, m_pCurObjDesc->lIndexCount, sizeof(ri3), Static_CompareIndexArrayBy12);

 lVertexAddedCount = 0;
 lIndexArrayEnd = m_pCurObjDesc->lIndexArrayStart + m_pCurObjDesc->lIndexCount;
 CurIndexRec = m_pIndexArray[m_pCurObjDesc->lIndexArrayStart];
 VertexReplacementIndex = CurIndexRec.VertexIndex;
 for(li = m_pCurObjDesc->lIndexArrayStart; li < lIndexArrayEnd; li ++) {
   if(m_pIndexArray[li].VertexIndex != CurIndexRec.VertexIndex) {
     CurIndexRec = m_pIndexArray[li];
     VertexReplacementIndex = CurIndexRec.VertexIndex;
     }
   else {
     if(m_pIndexArray[li].TextureIndex == CurIndexRec.TextureIndex) {
       if(m_pIndexArray[li].VertexIndex != VertexReplacementIndex)
         m_pIndexArray[li].VertexIndex = VertexReplacementIndex;
       }
     else {
       // create a new vertex
       if(m_lVertexArraySize == m_lVertexCount) if(!AddSpace(C3DOBJ_ARRTYPE_VERTEX)) return(false);
       m_pVertexArray[m_lVertexCount] = m_pVertexArray[m_pIndexArray[li].VertexIndex];
       VertexReplacementIndex = m_lVertexCount;
       m_lVertexCount ++;
       lVertexAddedCount ++;
       CurIndexRec = m_pIndexArray[li]; // save the original state. Comparisons are made on the original values.
       m_pIndexArray[li].VertexIndex = VertexReplacementIndex;
       // create the corresponding texture for the new vertex. The texture is from the changed texture index.
       //if(m_lTextureArraySize == m_lTextureCount) if(!AddSpace(C3DOBJ_ARRTYPE_TEXTURE)) return(false);
       //m_pTextureArray[m_lTextureCount] = m_pTextureArray[m_pIndexArray[li].TextureIndex];
       //m_lTextureCount ++;
       }
     }
   }

 m_pCurObjDesc->lVertexCount += lVertexAddedCount;
 m_pCurObjDesc->lTextureCount += lVertexAddedCount;
 m_lTotalVerticesAdded += lVertexAddedCount;
 qsort(m_pIndexArray + m_pCurObjDesc->lIndexArrayStart, m_pCurObjDesc->lIndexCount, sizeof(ri3), Static_CompareIndexArrayByOrdinal);
 return(true);
}

int C3DObj::Static_CompareIndexArrayBy12(const void *p_p1, const void *p_p2)
{
 ri3 *p1, *p2;

 p1 = (ri3 *)p_p1;
 p2 = (ri3 *)p_p2;

 if(p1->VertexIndex < p2->VertexIndex) return(-1);
 else if(p1->VertexIndex > p2->VertexIndex) return(1);
 if(p1->TextureIndex < p2->TextureIndex) return(-1);
 else if(p1->TextureIndex > p2->TextureIndex) return(1);
 if(p1->NormalIndex < p2->NormalIndex) return(-1);
 else if(p1->NormalIndex > p2->NormalIndex) return(1);
 return(0);
}

int C3DObj::Static_CompareIndexArrayByOrdinal(const void *p_p1, const void *p_p2)
{
 ri3 *p1, *p2;

 p1 = (ri3 *)p_p1;
 p2 = (ri3 *)p_p2;

 if(p1->lOrdinal < p2->lOrdinal) return(-1);
 else if(p1->lOrdinal > p2->lOrdinal) return(1);
 return(0);
}

void C3DObj::Draw()
{
 unsigned long li;
 CMaterialLib::rMaterial *pMaterial;

 //glColor3ub(255, 0, 0);
 for(li = 0; li < m_lObjDescCount; li ++) {
   if(!m_pObjDescArray[li].DrawFlags.m_bDraw) continue;
   pMaterial = m_pObjDescArray[li].MaterialIndex != -1 ? &m_MaterialLib.m_pMaterialArray[m_pObjDescArray[li].MaterialIndex] : NULL;
   glVertexPointer(3, GL_FLOAT, 0, m_pVertexArray);
   glTexCoordPointer(2, GL_FLOAT, 0, m_pStraightTextureArray);
   if(pMaterial) {
     if(pMaterial->m_bTexture) {
       glEnable(GL_TEXTURE_2D);
       glBindTexture(GL_TEXTURE_2D, pMaterial->m_TextureID); } }
   glPushMatrix(); 
   glRotatef(-90, 0.0f, 1.0f, 0.0f);
   glRotatef(-90, 1.0f, 0.0f, 0.0f);
   // face
   if(m_pObjDescArray[li].DrawFlags.m_bSelectFace) {
     glColor3ub(255, 0, 0);
     glDrawElements(GL_TRIANGLES, 3, GL_UNSIGNED_INT, m_pStraightIndexArray + m_pObjDescArray[li].lIndexArrayStart + m_pObjDescArray[li].DrawFlags.m_lSelectFaceNum * 3); 
     }
   // end face
   // object
   if(m_pObjDescArray[li].DrawFlags.m_bTransparent) {
     glEnable(GL_BLEND);
     glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA); }
   glColor4ub(255, 255, 255, m_pObjDescArray[li].DrawFlags.m_Alpha); // use material here
   glDrawElements(GL_TRIANGLES, m_pObjDescArray[li].lIndexCount, GL_UNSIGNED_INT, m_pStraightIndexArray + m_pObjDescArray[li].lIndexArrayStart); 
   if(m_pObjDescArray[li].DrawFlags.m_bWireframe) {
     glLineWidth(1);
     glDrawElements(GL_LINES, m_pObjDescArray[li].lIndexCount, GL_UNSIGNED_INT, m_pStraightIndexArray + m_pObjDescArray[li].lIndexArrayStart); }
   if(m_pObjDescArray[li].DrawFlags.m_bTransparent)
     glDisable(GL_BLEND);
   // end object
   glPopMatrix();
   if(pMaterial) {
     if(pMaterial->m_bTexture)
       glDisable(GL_TEXTURE_2D); }
   }
}

bool C3DObj::Unload(QGLContext *p_pContext)
{
 bool bVal;
 bVal = m_MaterialLib.UnloadTextures(p_pContext);
 Reset();
 return(bVal);
}

void C3DObj::SetDrawFlags(rObjDraw *p_prObjDraw)
{
 size_t li;

 for(li = 0; li < m_lObjDescCount; li ++) {
   m_pObjDescArray[li].DrawFlags.m_bTransparent = p_prObjDraw->m_bTransparent;
   m_pObjDescArray[li].DrawFlags.m_Alpha = p_prObjDraw->m_Alpha;
   m_pObjDescArray[li].DrawFlags.m_bWireframe = p_prObjDraw->m_bWireframe;
   m_pObjDescArray[li].DrawFlags.m_bSelectFace = p_prObjDraw->m_bSelectFace;
   m_pObjDescArray[li].DrawFlags.m_lSelectFaceNum = p_prObjDraw->m_lSelectFaceNum;
   }
}
