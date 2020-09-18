#pragma once

#include "qstring.h"
#include "WVSSTcpSocket.h"
#include "WVSSTcpServer.h"
#include "ZMQCxn.h"

class CWVSSGenericCxn : public QObject
{
 Q_OBJECT

public:
 enum eMode { CLIENT, SERVER };

private:
 bool m_bStarted;
 bool m_bTCPIPCxnEnabled, m_bZMQCxnEnabled;
 bool m_bSocketConnected;
 // TCPIP
 eMode m_TCPMode;
 QString m_TCPIPAddress;
 int m_nTCPIPPort;
 CWVSSTcpSocket m_Socket;
 CWVSSTcpServer m_TCPServer;
 QThread *m_pTCPThread;
 // ZMQ
 QString m_ZMQSendAddress;
 int m_nZMQSendPort;
 QString m_ZMQRecvAddress;
 int m_nZMQRecvPort;
 //QString m_DBServerAddress;
 //int m_nDBServerPort;
 CZMQCxn m_ZMQCxn;
 QThread *m_pZMQThread;
 //
 QString m_strThreadName; // messages are given with thread names and thread IDs. This makes it generic.

protected:
public:

private:
 QString AddThreadID(QString p_Str);
 char *GetMessageCharMemory(unsigned long p_lSize);
 void OnProcess(CWVSSTcpSocket *p_pSocket, CZMQCxn *p_pZMQCxn, QSharedPointer<char>p_Data);

protected:
public:
 CWVSSGenericCxn();
 ~CWVSSGenericCxn();
 void SetThreadName(QString p_strName) { m_strThreadName = p_strName; }
 void SetCxnType(bool p_bTCPIPCxnEnabled, eMode p_TCPMode, bool p_bZMQCxnEnabled, eMode p_ZMQMode);
 void SetAddressPort(QString p_TCPIPAddress, int p_nTCPIPPort, QString p_ZMQSendAddress, int p_nZMQSendPort, QString p_ZMQRecvAddress, int p_nZMQRecvPort);
 bool Start();
 //void Connect(); // this function does nothing unless TCPIP is on and we are a client. Used to reconnect. Questionable usefullness.
 void Stop();
 bool Send(QSharedPointer<char>p_Data);
 QString GetTCPIPSettings();

signals:
 void Message(QString p_Str);
 void Process(QSharedPointer<char>p_Data);
 void SocketConnectedDisconnected(bool p_bConnected); // refers to the TCPIP portion of comms. ZMQ doesn't have a corresponding signal
 void OKToClose();

public slots:
 void OnSocketConnectedDisconnected(CWVSSTcpSocket *, bool p_bConnected);
 void OnSocketMessage(CWVSSTcpSocket *, QString p_Str);
 void OnSocketProcess(CWVSSTcpSocket *p_pSocket, QSharedPointer<char>p_Data);
 void OnZMQMessage(CZMQCxn *, QString p_Str);
 void OnZMQProcess(CZMQCxn *p_pZMQCxn, QSharedPointer<char>p_Data);
 void OnTCPThreadFinished();
 void OnZMQThreadFinished();
 void OnTCPServerMessage(CWVSSTcpServer *, QString p_Str);
 void OnTCPServerProcess(CWVSSTcpSocket *, QSharedPointer<char>);
};
