#pragma once

#include "qobject.h"
#include "qtimer.h"
#include "qthread.h"
#include "ZMQReceiver.h"

class CZMQCxn : public QObject
{
 Q_OBJECT

public:
 enum eMode { CLIENT, SERVER };
 enum eLengthyOperationStatus { STARTED, FINISHED };

private:
 bool m_bStarted;
 eMode m_Mode;
 // addresses and ports are dependent on the mode. Server mode: both addresses are *, ports are listen ports, Client: real addresses, connect ports.
 QString m_strZMQSendAddress, m_strZMQRecvAddress;
 short m_nZMQSendPort, m_nZMQRecvPort;
 void *m_pZMQSender;
 CZMQReceiver *m_pZMQReceiver;
 QThread m_ReceiverThread;
 eLengthyOperationStatus m_LengthyOperationStatus;

protected:
public:
 static void *m_pZMQContext; // one context for an entire application

private:
 QString AddThreadID(QString p_Str);
 void CheckOKToClose();
 void SetLengthyOperationStatus(eLengthyOperationStatus p_LengthyOperationStatus) { m_LengthyOperationStatus = p_LengthyOperationStatus; }

protected:
public:
 static bool StaticCreateZMQContext(); // one context for an entire application
 static void StaticDestroyZMQContext(); // one context for an entire application
 //
 CZMQCxn();
 ~CZMQCxn();
 void StartStop(bool p_bStart);
 void SetMode(eMode p_Mode);
 void SetAddressPort(QString p_strZMQSendAddress, short p_nZMQSendPort, QString p_strRecvAddress, short p_nZMQRecvPort); // mode dependent
 bool IsStarted() { return(m_bStarted); }
 bool Send(QSharedPointer<char>p_Data);
 eLengthyOperationStatus GetLengthyOperationStatus() { return(m_LengthyOperationStatus); }

private slots:
 void OnInternalStartStop(bool p_bStart);
 void OnReceiverThreadExited();

public slots:
 void OnReceiverMessage(CZMQReceiver *p_pReceiver, QString p_Str);
 void OnReceiverDataProcess(CZMQReceiver *, QSharedPointer<char>p_Data);

signals:
 void Message(CZMQCxn *, QString p_Str);
 void InternalStartStop(bool p_bStart);
 void Process(CZMQCxn *, QSharedPointer<char>);
};
