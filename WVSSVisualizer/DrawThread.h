#pragma once

#include "qthread.h"

class AVP3D;

class CDrawThread : public QThread
{
private:
 AVP3D *m_pCaller;
 bool m_bRedraw;
 bool m_bExit;

protected:
public:

private:
 void run() Q_DECL_OVERRIDE;

protected:
public:
 CDrawThread(AVP3D *p_pCaller);
 virtual ~CDrawThread() { }
 void Do() { m_bRedraw = true; }
 void Stop() { m_bExit = true; }
};
