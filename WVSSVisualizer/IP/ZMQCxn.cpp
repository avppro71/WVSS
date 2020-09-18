#include "ZMQCxn.h"
#include "assert.h"
#include "WVSSIPProtocol.h"
#include "qsharedpointer.h"

#pragma warning (disable: 4996)

void *CZMQCxn::m_pZMQContext = NULL; // one context for an entire application

CZMQCxn::CZMQCxn()
{
 m_Mode = SERVER;
 SetLengthyOperationStatus(FINISHED);
 m_pZMQSender = NULL;
 m_pZMQReceiver = NULL;
 //m_nZMQPullPort = 21011;
 //m_nZMQPubPort = 21012;
 m_strZMQSendAddress = "*";
 m_strZMQRecvAddress = "*";
 m_nZMQSendPort = 21012;
 m_nZMQRecvPort = 21011;
 m_bStarted = false;
 connect(this, SIGNAL(InternalStartStop(bool)), this, SLOT(OnInternalStartStop(bool)));
 connect(&m_ReceiverThread, SIGNAL(finished()), this, SLOT(OnReceiverThreadExited()));
}

CZMQCxn::~CZMQCxn()
{
 assert(m_pZMQSender == NULL);
 assert(m_pZMQReceiver == NULL);
 //assert(m_pZMQContext == NULL); // there is only one ZMQ context, so it's not deleted with every object
}

QString CZMQCxn::AddThreadID(QString p_Str)
{
 return(QString("ZMQ thread ") + QString::number((unsigned long)thread(), 16) + ": " + p_Str);
}

void CZMQCxn::SetMode(eMode p_Mode)
{
 m_Mode = p_Mode;
}

void CZMQCxn::SetAddressPort(QString p_strZMQSendAddress, short p_nZMQSendPort, QString p_strRecvAddress, short p_nZMQRecvPort)
{
 m_strZMQSendAddress = p_strZMQSendAddress;
 m_nZMQSendPort = p_nZMQSendPort;
 m_strZMQRecvAddress = p_strRecvAddress;
 m_nZMQRecvPort = p_nZMQRecvPort;
}

void CZMQCxn::StartStop(bool p_bStart)
{ 
 SetLengthyOperationStatus(STARTED); // This let the caller know that a "breathing" lengthy operation started. When it completes, the value will be reset.
 emit InternalStartStop(p_bStart);
}

void CZMQCxn::OnReceiverThreadExited()
{
 CheckOKToClose();
}

void CZMQCxn::StaticDestroyZMQContext()
{
 if(!m_pZMQContext) return;
 zmq_ctx_destroy(m_pZMQContext);
 m_pZMQContext = NULL;
}

void CZMQCxn::CheckOKToClose()
{
 if(m_pZMQSender) { int nVal = 0; zmq_setsockopt(m_pZMQSender, ZMQ_LINGER, &nVal, sizeof(int)); zmq_close(m_pZMQSender); m_pZMQSender = NULL; }
 if(m_pZMQReceiver) { 
   if(m_pZMQReceiver->IsRunning())
     return;
   delete m_pZMQReceiver; m_pZMQReceiver = NULL; }
 //if(m_pZMQContext) { zmq_ctx_destroy(m_pZMQContext); m_pZMQContext = NULL; } // there is only one ZMQ context, so it's not deleted with every object
 m_bStarted = false;
 SetLengthyOperationStatus(FINISHED);
 thread()->exit();
}

void CZMQCxn::OnReceiverMessage(CZMQReceiver * /*p_pReceiver*/, QString p_Str)
{
 emit Message(this, AddThreadID(p_Str));
}

bool CZMQCxn::StaticCreateZMQContext()
{
 if(m_pZMQContext) return(true);
 m_pZMQContext = zmq_ctx_new();
 return(m_pZMQContext != NULL);
}

void CZMQCxn::OnInternalStartStop(bool p_bStart)
{
 bool bVal = true;
 int nVal;
 char tstr[128];

 if(p_bStart) {
   if(!StaticCreateZMQContext()) {
     bVal = false;
     emit Message(this, AddThreadID(QString("ZMQ context failed creation. Error ") + QString::number(errno))); }

   m_pZMQSender = zmq_socket(m_pZMQContext, m_Mode == SERVER ? ZMQ_PUB : ZMQ_PUSH);
   if(!m_pZMQSender) {
     bVal = false;
     emit Message(this, AddThreadID(QString("ZMQ sender socket failed creation. Error ") + QString::number(errno))); }
   
   nVal = -1;
   sprintf(tstr, "tcp://%s:%d", m_strZMQSendAddress.toLocal8Bit().constData(), m_nZMQSendPort);
   if(m_Mode == SERVER)
     nVal = zmq_bind(m_pZMQSender, tstr);
   else if(m_Mode == CLIENT)
     nVal = zmq_connect(m_pZMQSender, tstr);

   sprintf(tstr, "ZMQ sender socket %s to %s:%d %s", (m_Mode == SERVER) ? "PUB" : "PUSH", m_strZMQSendAddress.toLocal8Bit().constData(), m_nZMQSendPort, (nVal == 0)? "OK" : "failed");
   if(nVal != 0)
     sprintf(tstr + strlen(tstr), ". Error %d", errno);
   emit Message(this, AddThreadID(tstr));
   if(nVal != 0) bVal = false;

   m_pZMQReceiver = new CZMQReceiver();
   if(!m_pZMQReceiver) {
     emit Message(this, AddThreadID(QString("ZMQ receiver socket failed creation. Error ")));
     bVal = false;
     }
   else {
     connect(m_pZMQReceiver, SIGNAL(Message(CZMQReceiver *, QString)), this, SLOT(OnReceiverMessage(CZMQReceiver *, QString)));
     connect(m_pZMQReceiver, SIGNAL(Process(CZMQReceiver *, QSharedPointer<char>)), this, SLOT(OnReceiverDataProcess(CZMQReceiver *, QSharedPointer<char>)));
     m_ReceiverThread.start();
     m_pZMQReceiver->moveToThread(&m_ReceiverThread);
     m_pZMQReceiver->SetContext(m_pZMQContext);
     //m_pZMQReceiver->SetModeAddressPort((m_Mode == SERVER) ? m_pZMQReceiver->PULL : m_pZMQReceiver->SUB, m_strZMQRecvAddress, m_nZMQRecvPort);
     m_pZMQReceiver->Create((m_Mode == SERVER) ? m_pZMQReceiver->PULL : m_pZMQReceiver->SUB, m_strZMQRecvAddress, m_nZMQRecvPort);
     m_pZMQReceiver->StartStop(true);
     }
   m_bStarted  = bVal;
   SetLengthyOperationStatus(FINISHED); // technically, it dowsn't finish here because of an internal signal in m_pZMQReceiver
   }
 else {
   if(m_pZMQReceiver) {
     if(m_pZMQReceiver->IsRunning())
       m_pZMQReceiver->StartStop(false);
     else
       CheckOKToClose(); }
   else
     CheckOKToClose();
   }
}

void CZMQCxn::OnReceiverDataProcess(CZMQReceiver *, QSharedPointer<char>p_Data)
{
 emit Process(this, p_Data);
}

bool CZMQCxn::Send(QSharedPointer<char>p_Data)
{
 zmq_msg_t message;
 CWVSSMsgHeader *pMsg;
 int nVal;

 pMsg = reinterpret_cast<CWVSSMsgHeader *>(p_Data.data());

 zmq_msg_init_size(&message, pMsg->msg_size);
 memcpy(zmq_msg_data(&message), p_Data.data(), pMsg->msg_size);
 nVal = zmq_msg_send(&message, m_pZMQSender, ZMQ_DONTWAIT); // switch ZMQ_DONTWAIT for 0
 zmq_msg_close(&message);

 if((size_t)nVal != pMsg->msg_size)
   emit Message(this, AddThreadID(QString("ZMQ Publish failed. Error ") + QString::number(errno)));

 return((size_t)nVal == pMsg->msg_size);
}
