#ifndef INC_WVSS_TCPSOCKET
#define INC_WVSS_TCPSOCKET

#include "QTcpSocket.h"
#include "WVSSIPProtocol.h"

class CWVSSTcpSocket : public QTcpSocket
{
 Q_OBJECT

public:
 enum eLengthyOperationStatus { STARTED, FINISHED };

private:
 CWVSSMsgHeader m_MsgHeader; // the buffer where we place the header until it's fully obtained
 unsigned short m_nMsgHeaderUsed; // how many bytes of the header are obtained
 char *m_pMessageBuff; // buffer for the message currently being obtained. There may be multiple calls to readyRead to fill up the buffer
 unsigned long m_lMsgBuffUsed; // how many bytes of the message are obtained
 eLengthyOperationStatus m_LengthyOperationStatus;

protected:
public:
 bool m_bDeleteWhenClosed;

private:
 QString AddThreadID(QString p_Str);
 void SetLengthyOperationStatus(eLengthyOperationStatus p_LengthyOperationStatus) { m_LengthyOperationStatus = p_LengthyOperationStatus; }

protected:
public:
 CWVSSTcpSocket();
 virtual ~CWVSSTcpSocket();
 virtual qint64 Send(void *p_pBuf, int p_nLen);
 virtual bool OutOfThreadSend(QSharedPointer<char>p_Msg); // when a send (write) done from another thread, OnReadyRead is not called on the counterpart ((. Why? A internal signal solves this.
 void Connect(QString &p_TCPIPAddress, quint16 p_nTCPIPPort); // It is recommended to use this method and not direct QTcpSocket::m_Socket.connectToHost. See body for details.
 eLengthyOperationStatus GetLengthyOperationStatus() { return(m_LengthyOperationStatus); }

public slots:
 void OnSocketStateChanged(QAbstractSocket::SocketState);
 void OnBytesWritten(qint64 p_llBytes);
 void OnReadyRead();
 bool OnOutOfThreadSend(QSharedPointer<char>p_Msg); // see OutOfThreadSend

signals:
 void Message(CWVSSTcpSocket *, QString);
 void Process(CWVSSTcpSocket *, QSharedPointer<char>);
 bool OutOfThreadSendSignal(QSharedPointer<char>); // see OutOfThreadSend
 // sent when prior to actual deletion to le the server know to remove the reference to the socket from its vector
 void SocketDeletedSignal(CWVSSTcpSocket *);
 void SocketConnectedDisconnectedSignal(CWVSSTcpSocket *, bool); // bool value is 0/1 for connected/disconnected
};

#endif
