#include "WVSSTcpServer.h"
#include "qtcpsocket.h"
#include "qsharedpointer.h"

CWVSSTcpServer::CWVSSTcpServer()
{
 SetLengthyOperationStatus(FINISHED);
 m_bStarted = false;
 SetAddressAndPort("", 0); // set defaults, see SetAddressAndPort
 connect(this, SIGNAL(InternalStartStopListen(bool)), this, SLOT(OnInternalStartStopListen(bool)));
}

CWVSSTcpServer::~CWVSSTcpServer()
{

}

void CWVSSTcpServer::SetAddressAndPort(QString p_TCPIPAddress, unsigned short p_nTCPIPPort)
{
 m_TCPIPAddress = ((p_TCPIPAddress.length() || p_TCPIPAddress == "*")? QHostAddress(p_TCPIPAddress) : QHostAddress::AnyIPv4);
 m_nListenPort = (p_nTCPIPPort != 0 ? p_nTCPIPPort : 21013);
 QString Str = m_TCPIPAddress.toString();
}

void CWVSSTcpServer::StartStopListen(bool p_bStart)
{ 
 SetLengthyOperationStatus(STARTED); // This let the caller know that a "breathing" lengthy operation started. When it completes, the value will be reset.
 emit(InternalStartStopListen(p_bStart));
}

void CWVSSTcpServer::OnInternalStartStopListen(bool p_bStart)
{
 if(p_bStart) {
   if(!isListening()) {
     listen(m_TCPIPAddress, m_nListenPort);
     }
   if(!isListening())
     emit Message(this, AddThreadID("Listen failed"));
   else {
     emit Message(this, AddThreadID("Listening on port " + QString::number(serverPort())));
     m_bStarted = true; }
   }
 else {
   if(isListening())
     close();
   emit Message(this, AddThreadID("Listening stopped"));
   m_bStarted = false;
   }
 SetLengthyOperationStatus(FINISHED);
}

void CWVSSTcpServer::incomingConnection(qintptr p_nSocketDescriptor)
{
 emit Message(this, AddThreadID("Incoming connection"));
 CWVSSTcpSocket *pSocket = new CWVSSTcpSocket;
 if(!pSocket) { emit Message(this, AddThreadID("new QTcpSocket failed")); return; }
 pSocket->m_bDeleteWhenClosed = true;
 connect(pSocket, SIGNAL(Message(CWVSSTcpSocket *, QString)), this, SLOT(OnSocketMessage(CWVSSTcpSocket *, QString)));
 connect(pSocket, SIGNAL(Process(CWVSSTcpSocket *, QSharedPointer<char>)), this, SLOT(OnProcess(CWVSSTcpSocket *, QSharedPointer<char>)));
 connect(pSocket, SIGNAL(SocketDeletedSignal(CWVSSTcpSocket *)), this, SLOT(OnSocketDeleted(CWVSSTcpSocket *)));
 if(!pSocket->setSocketDescriptor(p_nSocketDescriptor)) {
   emit Message(this, AddThreadID("setSocketDescriptor failed"));
   return; }
 emit Message(this, AddThreadID("connected to descriptor " + QString::number(p_nSocketDescriptor)));
 m_Sockets.push_back(pSocket);
}

void CWVSSTcpServer::OnSocketMessage(CWVSSTcpSocket *, QString p_Str)
{
 emit Message(this, AddThreadID(p_Str));
}

QString CWVSSTcpServer::AddThreadID(QString p_Str)
{
 return(QString("CTcpServer thread ") + QString::number((unsigned long)thread(), 16) + ", " + p_Str);
}

void CWVSSTcpServer::OnProcess(CWVSSTcpSocket *p_Socket, QSharedPointer<char>p_Data)
{
 emit Process(p_Socket, p_Data);
}

void CWVSSTcpServer::SendToClients(QSharedPointer<char>p_Msg)
{
 size_t i;
 for(i = 0; i < m_Sockets.size(); i ++)
   m_Sockets[i]->OutOfThreadSend(p_Msg);
}

void CWVSSTcpServer::OnSocketDeleted(CWVSSTcpSocket *p_pSocket)
{
 std::vector<CWVSSTcpSocket *>::iterator It;

 //#pragma message(PPMSG("If you don't break and erase ai item, the iterator does NOT change and comparison against end() asserts. This is bad. Need to understand why this is."))
 for(It = m_Sockets.begin(); It != m_Sockets.end();) {
   if(*It == p_pSocket) It = m_Sockets.erase(It);
   else ++It;
   }
}

bool CWVSSTcpServer::hasPendingConnections() const
{
 return(QTcpServer::hasPendingConnections());
}
