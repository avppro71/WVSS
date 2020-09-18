#pragma once

#include <QTOpenGL>
#include "LineParser.h"
#include "3DObjLineParser.h"
#include "MaterialLib.h"

class C3DObj : public C3DObjLineParser
{
private:
 unsigned long m_lVertexArrayResizeCount;
 unsigned long m_lTextureArrayResizeCount;
 unsigned long m_lIndexArrayResizeCount;

public:
 struct rObjDraw { // override flags
   bool m_bDraw, m_bTransparent, m_bWireframe, m_bSelectFace;
   GLubyte m_Alpha; // user override. defaults are in the material.
   unsigned long m_lSelectFaceNum;
   };

protected:
 enum eArrayType { C3DOBJ_ARRTYPE_VERTEX, C3DOBJ_ARRTYPE_TEXTURE, C3DOBJ_ARRTYPE_INDEX, C3DOBJ_ARRTYPE_OBJDESC };
 struct rObjDesc {
   char Name[64];
   unsigned long lVertexArrayStart, lTextureArrayStart, lIndexArrayStart;
   unsigned long lVertexCount, lTextureCount, lIndexCount;
   rObjDraw DrawFlags;
   short MaterialIndex; // -1 on error
   };
 struct rf2 { GLfloat x, y; };
 unsigned long m_lTotalVerticesAdded; // since file load. Needed to add to the currect index number for objects starting with the second one
 CPathParts m_FileParts;

 CMaterialLib m_MaterialLib;

public: // make whatever needs to be public public, the rest - protected
 rf3 *m_pVertexArray;
 unsigned long m_lVertexArraySize, m_lVertexCount;
 rf2 *m_pTextureArray;
 unsigned long m_lTextureArraySize, m_lTextureCount;
 ri3 *m_pIndexArray;
 unsigned long m_lIndexArraySize, m_lIndexCount;
 rObjDesc *m_pObjDescArray, *m_pCurObjDesc;
 unsigned long m_lObjDescArraySize, m_lObjDescCount;
 // OpenGL output arrays. The vertex one is the only one ready for use from m_pVertexArray
 GLuint *m_pStraightIndexArray; // indices into the vertex array that has been made unique. Uses m_pIndexArray.VertexIndex
 rf2 *m_pStraightTextureArray; // made straightforward from the unuquely-valued m_pTextureArray and m_pIndexArray.TextureIndex

public:

private:
protected:
 void Reset();
 virtual bool ProcessLine(char *p_pLine);
 bool AddSpace(eArrayType p_Type);
 bool ParseToObjDesc(char *p_pLine, rObjDesc *p_pOD); // strtok changes the line
 bool AddToArray(eArrayType p_Type, void *p_pSrc);
 bool CreateUniqueVertexTextureIndices();
 static int Static_CompareIndexArrayBy12(const void *p_p1, const void *p_p2);
 static int Static_CompareIndexArrayByOrdinal(const void *p_p1, const void *p_p2);
 virtual bool OnBeforeLoad(const char *p_pFileName);
 virtual bool OnLoaded();

public:
 C3DObj();
 ~C3DObj();
 void Draw();
 bool LoadTextures(QGLContext *p_pContext) { return(m_MaterialLib.LoadTextures(p_pContext)); }
 bool Unload(QGLContext *p_pContext);
 void SetDrawFlags(rObjDraw *p_prObjDraw);
 const CPathParts GetFileParts() { return(m_FileParts); }
};
