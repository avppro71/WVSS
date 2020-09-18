#include "qsharedpointer.h"
#include "qapplication.h"
#include "WVSSGenericCxn.h"
#include "WVSSIPCommonFunctions.h"

CWVSSGenericCxn::CWVSSGenericCxn()
{
 m_bStarted = false;
 m_strThreadName = "CWVSSGenericCxn";

 GlobalFillMessageNames();

 m_TCPMode = CLIENT; // connects to a server elsewhere
 m_bTCPIPCxnEnabled = m_bZMQCxnEnabled = true;
 m_bSocketConnected = false;

 m_pZMQThread = NULL;
 m_pTCPThread = NULL;

 m_TCPIPAddress = "127.0.0.1";
 m_nTCPIPPort = 21100;
 m_ZMQSendAddress = "127.0.0.1";
 m_nZMQSendPort = 21101;
 m_ZMQRecvAddress = "127.0.0.1";
 m_nZMQRecvPort = 21102;

 qRegisterMetaType<QAbstractSocket::SocketState>("QAbstractSocket::SocketState");
 qRegisterMetaType<QSharedPointer<char> >("QSharedPointer<char>");

 //connect(this, SIGNAL(InternalOKToClose()), this, SLOT(OnOKToClose()));

 connect(&m_Socket, SIGNAL(Message(CWVSSTcpSocket *, QString)), this, SLOT(OnSocketMessage(CWVSSTcpSocket *, QString)));
 connect(&m_Socket, SIGNAL(Process(CWVSSTcpSocket *, QSharedPointer<char>)), this, SLOT(OnSocketProcess(CWVSSTcpSocket *, QSharedPointer<char>)));
 connect(&m_Socket, SIGNAL(SocketConnectedDisconnectedSignal(CWVSSTcpSocket *, bool)), this, SLOT(OnSocketConnectedDisconnected(CWVSSTcpSocket *, bool)));
 connect(&m_TCPServer, SIGNAL(Message(CWVSSTcpServer *, QString)), this, SLOT(OnTCPServerMessage(CWVSSTcpServer *, QString)));
 connect(&m_TCPServer, SIGNAL(Process(CWVSSTcpSocket *, QSharedPointer<char>)), this, SLOT(OnTCPServerProcess(CWVSSTcpSocket *, QSharedPointer<char>)));

 m_ZMQCxn.SetMode(CZMQCxn::CLIENT);
 m_ZMQCxn.SetAddressPort(m_ZMQSendAddress, m_nZMQSendPort, m_ZMQRecvAddress, m_nZMQRecvPort);
 connect(&m_ZMQCxn, SIGNAL(Process(CZMQCxn *, QSharedPointer<char>)), this, SLOT(OnZMQProcess(CZMQCxn *, QSharedPointer<char>)));
 connect(&m_ZMQCxn, SIGNAL(Message(CZMQCxn *, QString)), this, SLOT(OnZMQMessage(CZMQCxn *, QString)));
}

CWVSSGenericCxn::~CWVSSGenericCxn()
{
 if(m_pZMQThread) delete m_pZMQThread;
 if(m_pTCPThread) delete m_pTCPThread;
}

QString CWVSSGenericCxn::AddThreadID(QString p_Str)
{
 return(m_strThreadName + " (ThreadID " + QString::number((unsigned long)thread(), 16) + "): " + p_Str);
}

void CWVSSGenericCxn::SetCxnType(bool p_bTCPIPCxnEnabled, eMode p_TCPMode, bool p_bZMQCxnEnabled, eMode p_ZMQMode)
{
 m_bTCPIPCxnEnabled = p_bTCPIPCxnEnabled;
 m_bZMQCxnEnabled = p_bZMQCxnEnabled;
 m_TCPMode = p_TCPMode;
 m_ZMQCxn.SetMode(p_ZMQMode == SERVER ? CZMQCxn::SERVER : CZMQCxn::CLIENT);
}

void CWVSSGenericCxn::SetAddressPort(QString p_TCPIPAddress, int p_nTCPIPPort, QString p_ZMQSendAddress, int p_nZMQSendPort, QString p_ZMQRecvAddress, int p_nZMQRecvPort)
{
 m_TCPIPAddress = p_TCPIPAddress;
 m_nTCPIPPort = p_nTCPIPPort;
 m_TCPServer.SetAddressAndPort(m_TCPIPAddress, m_nTCPIPPort);

 m_ZMQSendAddress = p_ZMQSendAddress;
 m_nZMQSendPort = p_nZMQSendPort;
 m_ZMQRecvAddress = p_ZMQRecvAddress;
 m_nZMQRecvPort = p_nZMQRecvPort;
 m_ZMQCxn.SetAddressPort(m_ZMQSendAddress, m_nZMQSendPort, m_ZMQRecvAddress, m_nZMQRecvPort);
}

bool CWVSSGenericCxn::Start()
// blocking as far as ZMQ
// a partial successful start is possible, so immediately call Stop when Start returns false
// 20160219: the call was blocking except for the socket connect call. Now it's blocking for all types of connections.
{
 if(m_bStarted) return(true);

//#if !defined(NDEBUG)
// #pragma message("**************************************************** CWVSSGenericCxn::Start: QApplication::processEvents is used, make sure it doesn't affect performance")
//#endif

 if(m_bTCPIPCxnEnabled) {
   if(m_TCPMode == SERVER) {
     if(!m_pTCPThread)
       m_pTCPThread = new QThread;
     if(!m_pTCPThread) return(m_bStarted);
     connect(m_pTCPThread, SIGNAL(finished()), this, SLOT(OnTCPThreadFinished()));
     m_pTCPThread->start();
     emit Message(AddThreadID(QString("TCP thread ") + QString::number((unsigned long)m_pTCPThread, 16)));
     m_TCPServer.moveToThread(m_pTCPThread);
     m_TCPServer.StartStopListen(true);
     while(m_TCPServer.GetLengthyOperationStatus() == m_TCPServer.STARTED)
       QApplication::processEvents(QEventLoop::AllEvents, 100);
     }
   else {
     m_Socket.Connect(m_TCPIPAddress, m_nTCPIPPort); // this call asyncronous
     while(m_Socket.GetLengthyOperationStatus() == m_Socket.STARTED)
       QApplication::processEvents(QEventLoop::AllEvents, 100);
     }
   }
 if(m_bZMQCxnEnabled) {
   if(!m_pZMQThread)
     m_pZMQThread = new QThread;
   if(!m_pZMQThread) return(m_bStarted);
   connect(m_pZMQThread, SIGNAL(finished()), this, SLOT(OnZMQThreadFinished()));
   m_pZMQThread->start();
   emit Message (AddThreadID(QString("ZMQ thread address ") + QString::number((unsigned long)m_pZMQThread, 16)));
   m_ZMQCxn.moveToThread(m_pZMQThread);
   m_ZMQCxn.StartStop(true);
   while(m_ZMQCxn.GetLengthyOperationStatus() == m_ZMQCxn.STARTED)
     QApplication::processEvents(QEventLoop::AllEvents, 100);
   }
 
 m_bStarted = ((!m_bTCPIPCxnEnabled || ((m_TCPMode == SERVER && m_TCPServer.IsStarted()) || (m_TCPMode == CLIENT && m_bSocketConnected))) && (!m_bZMQCxnEnabled || m_ZMQCxn.IsStarted()));
 return(m_bStarted);
}

/*
void CWVSSGenericCxn::Connect()
// this function does nothing unless TCPIP is on and we are a client.
{
 if(!(m_bTCPIPCxnEnabled || m_TCPMode == CLIENT)) return;
 m_Socket.connectToHost(m_TCPIPAddress, m_nTCPIPPort);
}
*/

void CWVSSGenericCxn::Stop()
// blocking
{
 bool bVal, bThreadExitRequested[2] = { false, false };

 if(m_bTCPIPCxnEnabled) {
   if(m_TCPMode == CLIENT)
     m_Socket.close();
   else if(m_TCPMode == SERVER)
     m_TCPServer.StartStopListen(false); 
   }
 if(m_bZMQCxnEnabled)
   m_ZMQCxn.StartStop(false);
 
 for(;;) {
   bVal = true;
   if(bVal) {
     if(m_bTCPIPCxnEnabled) {
       if(m_TCPServer.GetLengthyOperationStatus() != m_TCPServer.FINISHED)
         bVal = false;
       else if(!bThreadExitRequested[0]) {
         if(m_pTCPThread)
           m_pTCPThread->exit(); // thread is closed from outside the thread. This chunk of code is in a loop, hence a state variable bThreadExitRequested
         bThreadExitRequested[0] = true; }
       if(m_pTCPThread) {
         if(!m_pTCPThread->isFinished())
           bVal = false;
         }
       }
     } // bVal
   if(bVal) {
     if(m_bZMQCxnEnabled) {
       if(m_ZMQCxn.GetLengthyOperationStatus() != m_ZMQCxn.FINISHED)
         bVal = false;
       else if(!bThreadExitRequested[1]) {
         if(m_pZMQThread)
           m_pZMQThread->exit();
         bThreadExitRequested[1] = true; }
       if(m_pZMQThread) {
         if(!m_pZMQThread->isFinished())
           bVal = false;
         }
       }
     } // bVal
   
   if(bVal) break;
   QApplication::processEvents(QEventLoop::AllEvents, 100);
   }

 //QApplication::processEvents(QEventLoop::AllEvents, 100);
 //QThread::msleep(1500);
 m_bStarted = false;
}

char *CWVSSGenericCxn::GetMessageCharMemory(unsigned long p_lSize)
{
 CWVSSMsgHeader *pMsgHeader;
 char *pCharMsg = new char [p_lSize];
 if(!pCharMsg) {
  Message(AddThreadID("CWVSSGenericCxn:GetCharMemory: no memory, needed ") + QString::number(p_lSize) + " bytes");
  return(NULL); }
 
 pMsgHeader = reinterpret_cast<CWVSSMsgHeader *>(pCharMsg);
 pMsgHeader->sync = WVSS_IP_PROTOCOL_TCPSYNCBYTES;
 pMsgHeader->protocol_id = WVSS_IP_PROTOCOL_VERSION;
 pMsgHeader->msg_size = p_lSize;
 return(pCharMsg);
}

void CWVSSGenericCxn::OnSocketConnectedDisconnected(CWVSSTcpSocket *, bool p_bConnected)
{
 m_bSocketConnected = p_bConnected;
 emit SocketConnectedDisconnected(p_bConnected);
}

void CWVSSGenericCxn::OnSocketMessage(CWVSSTcpSocket *, QString p_Str)
{
 emit Message(p_Str);
}

void CWVSSGenericCxn::OnZMQMessage(CZMQCxn *, QString p_Str)
{
 emit Message(p_Str);
}

void CWVSSGenericCxn::OnSocketProcess(CWVSSTcpSocket *p_pSocket, QSharedPointer<char>p_Data)
// processing messages from the server via TCPIP
{
 OnProcess(p_pSocket, NULL, p_Data);
}

void CWVSSGenericCxn::OnZMQProcess(CZMQCxn *p_pZMQCxn, QSharedPointer<char>p_Data)
// processing messages from the server via ZMQ
{
 OnProcess(NULL, p_pZMQCxn, p_Data);
}

void CWVSSGenericCxn::OnTCPServerProcess(CWVSSTcpSocket *, QSharedPointer<char>p_Data)
{
 emit Process(p_Data);
}

void CWVSSGenericCxn::OnProcess(CWVSSTcpSocket * /*p_pSocket*/, CZMQCxn * /*p_pZMQCxn*/, QSharedPointer<char>p_Data)
{
 emit Process(p_Data);
}

void CWVSSGenericCxn::OnZMQThreadFinished()
{
 emit Message(AddThreadID("ZMQ thread finished"));
}

void CWVSSGenericCxn::OnTCPThreadFinished()
{
 emit Message(AddThreadID("TCP thread finished"));
}

void CWVSSGenericCxn::OnTCPServerMessage(CWVSSTcpServer *, QString p_Str)
{
 emit Message(AddThreadID("TCP server message " + p_Str));
}

bool CWVSSGenericCxn::Send(QSharedPointer<char>p_Data)
{
 bool bVal[2] = { false, false };

 if(m_bTCPIPCxnEnabled) {
   if(m_TCPMode == SERVER)
     m_TCPServer.SendToClients(p_Data);
   else
     bVal[0] = m_Socket.OutOfThreadSend(p_Data);
   
   }

 if(m_bZMQCxnEnabled)
   bVal[1] = m_ZMQCxn.Send(p_Data);

 return(bVal[0] || bVal[1]); // true if at least one channel sent successfully
}

QString CWVSSGenericCxn::GetTCPIPSettings() 
{ 
 return(m_TCPIPAddress + "[" + QString::number(m_nTCPIPPort) + "]");
}
