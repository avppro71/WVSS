#ifndef INC_WVSS_TCPSERVER
#define INC_WVSS_TCPSERVER

#include <QTcpServer.h>
#include <vector>
#include "WVSSTcpSocket.h"

class CWVSSTcpServer : public QTcpServer
{
 Q_OBJECT

public:
 enum eLengthyOperationStatus { STARTED, FINISHED };

private:
 bool m_bStarted;
 eLengthyOperationStatus m_LengthyOperationStatus;
 QHostAddress m_TCPIPAddress;
 unsigned short m_nListenPort;

protected:
public:
 std::vector<CWVSSTcpSocket *> m_Sockets;

private:
 QString AddThreadID(QString p_Str);
 void SetLengthyOperationStatus(eLengthyOperationStatus p_LengthyOperationStatus) { m_LengthyOperationStatus = p_LengthyOperationStatus; }

protected:
 virtual void incomingConnection(qintptr socketDescriptor) Q_DECL_OVERRIDE;
 virtual bool hasPendingConnections() const;

public:
 CWVSSTcpServer();
 ~CWVSSTcpServer();
 // used to distribute a message in common format (mesg_header) to all connected clients
 void SendToClients(QSharedPointer<char>);
 //void AcceptConnections(bool p_bAccept) { m_bAccept = p_bAccept; }
 void StartStopListen(bool p_bStart);
 bool IsStarted() { return(m_bStarted); }
 eLengthyOperationStatus GetLengthyOperationStatus() { return(m_LengthyOperationStatus); }
 unsigned short GetListenPort() { return(m_nListenPort); }
 // pass an empty string or * for address to get the default (QHostAddress::AnyIPv4). Pass 0 for port to get the default of 2103;
 void SetAddressAndPort(QString p_TCPIPAddress, unsigned short m_nTCPIPPort);

signals:
 void InternalStartStopListen(bool p_bStart);
 void Message(CWVSSTcpServer *, QString);
 void Process(CWVSSTcpSocket *, QSharedPointer<char>);

private slots:
 // a response to the internal start listen
 void OnInternalStartStopListen(bool p_bStart);

public slots:
 void OnSocketMessage(CWVSSTcpSocket *, QString);
 // emits Process signal
 void OnProcess(CWVSSTcpSocket *p_Socket, QSharedPointer<char>p_Data);
 // a response to a sockets's SocketDeletedSignal
 void OnSocketDeleted(CWVSSTcpSocket *p_pSocket);
};

#endif
