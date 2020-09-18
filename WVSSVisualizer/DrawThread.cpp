#include "DrawThread.h"
#include "AVP3D.h"

CDrawThread::CDrawThread(AVP3D *p_pCaller)
{
 m_bExit = false;
 m_bRedraw = false;
 m_pCaller = p_pCaller;
}

void CDrawThread::run()
{
 while(!m_bExit) {
   if(m_bRedraw) {
    m_pCaller->updateGL();
    m_bRedraw = false; }
   Sleep(10);
   }
}
