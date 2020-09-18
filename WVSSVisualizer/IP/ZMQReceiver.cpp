#include "ZMQReceiver.h"
#include "qthread.h"
#include "WVSSIPProtocol.h"
#include "assert.h"
#include "qsharedpointer.h"
#include "WVSSIPCommonFunctions.h"

#pragma warning (disable: 4996)

CZMQReceiver::CZMQReceiver()
{
 m_Mode = PULL;
 m_pZMQContext = NULL;
 m_nZMQPort = 21011;
 m_pZMQReceiver = NULL;
 m_strZMQAddress = "*";
 m_bStop = false;
 m_bRunning = false;
 connect(this, SIGNAL(InternalStartStop(bool)), this, SLOT(OnInternalStartStop(bool)));
}

CZMQReceiver::~CZMQReceiver()
{
}

QString CZMQReceiver::AddThreadID(QString p_Str)
{
 return(QString("ZMQ thread ") + QString::number((unsigned long)thread(), 16) + ": " + p_Str);
}

bool CZMQReceiver::Create(eMode p_Mode, QString p_strZMQAddress, short p_nZMQPort)
{
 char tstr[128];
 int nVal;

 m_Mode = p_Mode;
 if(p_strZMQAddress.length()) m_strZMQAddress = p_strZMQAddress;
 if(p_nZMQPort) m_nZMQPort = p_nZMQPort;

 if(!m_pZMQContext) { emit Message(this, AddThreadID(QString("ZMQ receiver failed creation: no ZMQ context"))); return(false); }
 if(!m_pZMQReceiver) {
   m_pZMQReceiver = zmq_socket(m_pZMQContext, (m_Mode == PULL) ? ZMQ_PULL : ZMQ_SUB);
   if(!m_pZMQReceiver) {
     emit Message(this, AddThreadID(QString("ZMQ receiver socket failed creation. Error ") + QString::number(errno)));
     return(false); }
   nVal = -1;
   sprintf(tstr, "tcp://%s:%d", m_strZMQAddress.toLocal8Bit().constData(), m_nZMQPort);
   if(m_Mode == PULL)
     nVal = zmq_bind(m_pZMQReceiver, tstr);
   else if(m_Mode == SUB) {
     nVal = zmq_connect(m_pZMQReceiver, tstr);
     if(!nVal)
       nVal = zmq_setsockopt(m_pZMQReceiver, ZMQ_SUBSCRIBE, NULL, 0);
     }
   sprintf(tstr, "ZMQ receiver socket %s to %s:%d %s", (m_Mode == PULL) ? "PULL" : "SUB", m_strZMQAddress.toLocal8Bit().constData(), m_nZMQPort, (nVal == 0)? "OK" : "failed");
   if(nVal != 0)
     sprintf(tstr + strlen(tstr), ". Error %d", errno);
   emit Message(this, AddThreadID(tstr));
   if(nVal != 0) return(false);
   }
 return(true);
}

void CZMQReceiver::StartStop(bool p_bStart)
{
 if(p_bStart)
   emit InternalStartStop(p_bStart);
 else
  m_bStop = true;
}

void CZMQReceiver::OnInternalStartStop(bool p_bStart)
{
 if(p_bStart)
   RunPullLoop();
}

void CZMQReceiver::RunPullLoop()
{
 zmq_msg_t reply;
 size_t Length;
 int stReceived;
 char tstr[128], *pData;
 CWVSSMsgHeader *pMsg;
 int nVal;
 bool bVal;
 QSharedPointer<char>MsgHeader;

 if(m_bRunning) return;
 if(!m_pZMQReceiver) return;

 m_bStop = false;
 m_bRunning = true;

 while(!m_bStop) {
   zmq_msg_init(&reply);
   stReceived = zmq_msg_recv(&reply, m_pZMQReceiver, ZMQ_DONTWAIT);
   if(stReceived == -1) {
     if(errno != EAGAIN) break;
     QThread::msleep(100);
     continue; }
   Length = zmq_msg_size(&reply);
   pData = new char [Length];
   if(!pData)
     sprintf(tstr, "No memory for incoming message. Needed %lu bytes", (unsigned long)Length);
   else
     memcpy(pData, zmq_msg_data(&reply), Length);
   zmq_msg_close(&reply);
   pMsg = reinterpret_cast<CWVSSMsgHeader *>(pData);
   sprintf(tstr, "Message type 0x%x-%s [%lu bytes] received", pMsg->msg_type, GlobalMessageName(pMsg->msg_type).toLocal8Bit().constData(), pMsg->msg_size);
   emit Message(this, AddThreadID(tstr));

   bVal = false;
   for(;;) {
     if(pMsg->sync != WVSS_IP_PROTOCOL_TCPSYNCBYTES) {
       emit Message(this, AddThreadID("CZMQReceiver::RunPullLoop: sync byte mismatch, given ") + QString::number(pMsg->sync) + ", packet discarded");
       break; }
     if(pMsg->CalcCRC() != pMsg->crc) {
       emit Message(this, AddThreadID("CZMQReceiver::RunPullLoop: packet crc mismatch, msg type ") + QString::number(pMsg->msg_type) + ", packet discarded");
       break; }
     bVal = true;
     break; }
       
   if(bVal) {
     MsgHeader = QSharedPointer<char>(pData);
     emit Process(this, MsgHeader); }
   }

 nVal = 0;
 zmq_setsockopt(m_pZMQReceiver, ZMQ_LINGER, &nVal, sizeof(int));
 zmq_close(m_pZMQReceiver); m_pZMQReceiver = NULL;

 thread()->exit();
 m_bRunning = false;
}
