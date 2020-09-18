#pragma once

#include "qobject.h"
#include "zmq.h"

class CZMQReceiver : public QObject
{
 Q_OBJECT

public:
 enum eMode { PULL, SUB };

private:
 short m_nZMQPort;
 QString m_strZMQAddress;
 void *m_pZMQReceiver;
 bool m_bStop, m_bRunning;
 eMode m_Mode;

protected:
public:
 void *m_pZMQContext;

private:
 QString AddThreadID(QString p_Str);
 void RunPullLoop();

public:
 CZMQReceiver();
 ~CZMQReceiver();
 bool IsRunning() { return(m_bRunning); }
 // Create: depending on the mode, it's either PULL, or SUB. Thus, supply either p_nZMQPullPort or a pair p_strZMQSubAddress + p_nZMQSubPort
 void SetModeAddressPort(eMode p_Mode, QString p_strZMQAddress, short p_nZMQPort);
 bool Create(eMode p_Mode, QString p_strZMQAddress, short p_nZMQPort);
 void StartStop(bool p_bStart);
 void SetContext(void *p_pZMQContext) { m_pZMQContext = p_pZMQContext; }

private slots:
 void OnInternalStartStop(bool p_bStart);

public slots:

signals:
 void Message(CZMQReceiver *p_pReceiver, QString p_Str);
 void InternalStartStop(bool p_bStart);
 void Process(CZMQReceiver *p_pReceiver, QSharedPointer<char>);
};
