#include "WVSSTcpSocket.h"
#include "WVSSIPCommonVariables.h"
#include "WVSSIPCommonFunctions.h"
#include "qsharedpointer.h"
#include "qthread.h"

CWVSSTcpSocket::CWVSSTcpSocket()
{
 SetLengthyOperationStatus(FINISHED);

 connect(this, SIGNAL(stateChanged(QAbstractSocket::SocketState)), this, SLOT(OnSocketStateChanged(QAbstractSocket::SocketState)));
 connect(this, SIGNAL(bytesWritten(qint64)), this, SLOT(OnBytesWritten(qint64)));
 connect(this, SIGNAL(readyRead()), this, SLOT(OnReadyRead()));

 connect(this, SIGNAL(OutOfThreadSendSignal(QSharedPointer<char>)), this, SLOT(OnOutOfThreadSend(QSharedPointer<char>)));

 m_bDeleteWhenClosed = false;
 m_pMessageBuff = NULL;
 m_lMsgBuffUsed = 0;
 m_nMsgHeaderUsed = 0;
}

CWVSSTcpSocket::~CWVSSTcpSocket()
{

}

void CWVSSTcpSocket::Connect(QString &p_TCPIPAddress, quint16 p_nTCPIPPort)
// recommended to use this method and not direct QTcpSocket::m_Socket.connectToHost.
// 20150219: The reson is that connectToHost is asyncronous - the method returns before a connection is established. The caller may or may not want a non-blocking operation in his call to Connect.
// connectToHost returns void and later tells you if a connection has been established with a signal. Thus, there is no way to make a blocking caller. To fix this, a lengthy operation status flag
// is introduced. It has two states: STARTED and FINISHED. If you don't call this method, the flag will never be set. If you do, you may wait in the caller until GetLengthyOperationStatus returns FINISHED.
{
 SetLengthyOperationStatus(STARTED);
 connectToHost(p_TCPIPAddress, p_nTCPIPPort);
}

void CWVSSTcpSocket::OnSocketStateChanged(QAbstractSocket::SocketState p_SocketState)
{
 QString Str;
 //qintptr nVal;
 
 /*nVal = */socketDescriptor();
 Str = QString("Socket ") + QString::number(socketDescriptor()) + " state changed to " + QString::number(p_SocketState) + ": " + g_pSocketStateText[p_SocketState];
 emit Message(this, AddThreadID(Str));

 if(p_SocketState == ConnectedState || p_SocketState == UnconnectedState)
   SetLengthyOperationStatus(FINISHED);

 if(p_SocketState == ConnectedState)
   emit SocketConnectedDisconnectedSignal(this, true);
 else if(p_SocketState == UnconnectedState)
   emit SocketConnectedDisconnectedSignal(this, false);

 if(m_bDeleteWhenClosed && p_SocketState == UnconnectedState) {
   emit SocketDeletedSignal(this);
   deleteLater(); }
}

QString CWVSSTcpSocket::AddThreadID(QString p_Str)
{
 return(QString("CWVSSTcpSocket thread ") + QString::number((unsigned long)thread(), 16) + ", " + p_Str);
}

qint64 CWVSSTcpSocket::Send(void *p_pBuf, int p_nLen)
// full CWVSSMsgHeader-derived messages are sent with calls to this method
// The call is made blocking until all data are sent out
{
 CWVSSMsgHeader *pMsgHeader;
 qint64 llWritten, llTotalWritten = 0;

//#if !defined(NDEBUG)
// #pragma message(PPMSG("Need to test partial send capability !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!! "))
//#endif

 pMsgHeader = reinterpret_cast<CWVSSMsgHeader *>(p_pBuf);

 for(;;) {
   llWritten = write((char *)p_pBuf + llTotalWritten, p_nLen - llTotalWritten);
   if(llWritten != -1)
     llTotalWritten += llWritten;
   else {
     emit Message(this, AddThreadID(QString("LidarSocket::Send write returned -1: ") + errorString()));
     return(-1); }
   if(llWritten < p_nLen)
     waitForBytesWritten(1000);
   else
     break;
   }
 emit Message(this, AddThreadID("CWVSSTcpSocket::Send " + QString::number(llTotalWritten) + " bytes sent, message 0x" + QString::number(pMsgHeader->msg_type, 16) + " - " + GlobalMessageName(pMsgHeader->msg_type)));
 return(llTotalWritten);
}

void CWVSSTcpSocket::OnBytesWritten(qint64 /*p_llBytes*/)
{
 //short a;
 //a = 10;
}

void CWVSSTcpSocket::OnReadyRead()
// The exchange is through packets derived from CWVSSMsgHeader
// The size can be any, because the lidar can work in a combined mode
// 150825: synchronization
{
 qint64 llBytedRead;
 QSharedPointer<char>MsgHeader;
 //char *pMsgBuffSave;
 short nBytesToDiscard;
 short i;
 CWVSSMsgHeader *pMsg;
 unsigned short SyncVal = WVSS_IP_PROTOCOL_TCPSYNCBYTES;
 //unsigned long ulDiscarded;

 //pMsgBuffSave = NULL;
 //ulDiscarded = 0;
 for(;;) {

   if(m_lMsgBuffUsed < sizeof(CWVSSMsgHeader)) { // try to read the header. It conatins our packet size. We have to save the header separately into a prefefined structure - we don't have the message size.
     llBytedRead = read((char *)&m_MsgHeader + m_nMsgHeaderUsed, sizeof(CWVSSMsgHeader) - m_nMsgHeaderUsed);

     if(!llBytedRead || llBytedRead == -1) break;

     m_nMsgHeaderUsed += (short)llBytedRead;

     // at least one byte has been read
     // sync
     if(m_nMsgHeaderUsed >= sizeof(m_MsgHeader.sync)) {
       if(m_MsgHeader.sync != SyncVal) {
         // Ok, data is out of sync. The easiest way to handle it is to discard bad data - these very two bytes until there is a match.
         // We just have to be careful not to discard both bytes if the second one == IANS_LIDAR_PROTOCOL_TCPSYNCBYTES[0]
         // The method below may not be efficient, especially when IANS_LIDAR_PROTOCOL_TCPSYNCBYTES gets to be longer than two bytes.
         // Since we have at least one byte read, we can start the serch with the second byte.
         // The reason: 
         //    we know that the combination as a whole is bad
         //    if the first byte == (unsigned char)IANS_LIDAR_PROTOCOL_TCPSYNCBYTES, we will stop searching and be where we started, not move anything and loop forever
         //    when starting with the second byte, we will definitely move something, at leats a byte
         for(i = 1; i < m_nMsgHeaderUsed/*sizeof(m_MsgHeader.sync)*/; i ++) {
           if(*((unsigned char *)&m_MsgHeader.sync + i) == (unsigned char)SyncVal)
             break;
           }
         // now we know how many bytes to discard. Maybe, all.
         if(i == m_nMsgHeaderUsed) { // haven't found (unsigned char)IANS_LIDAR_PROTOCOL_TCPSYNCBYTES at all, discard everything
           nBytesToDiscard = m_nMsgHeaderUsed;
           m_nMsgHeaderUsed = 0; }
         else {
           nBytesToDiscard = i;
           memmove(&m_MsgHeader, (unsigned char *)&m_MsgHeader + nBytesToDiscard, m_nMsgHeaderUsed - nBytesToDiscard);
           m_nMsgHeaderUsed -= nBytesToDiscard; }
         emit Message(this, AddThreadID("CWVSSTcpSocket::OnReadyRead: packet out of sync, ") + QString::number(nBytesToDiscard) + " bytes discarded");
         continue; // proceed to receive more bytes
         }
       }
     // end sync

     if(m_nMsgHeaderUsed < sizeof(CWVSSMsgHeader)) break; // coudn't get the whole header. Can't do much else.
     // We have the whole header and this is the first time in this message that we know that. Allocate the memory for the message.
     m_pMessageBuff = new char [m_MsgHeader.msg_size];
     if(!m_pMessageBuff) {
       emit Message(this, AddThreadID("CWVSSTcpSocket::OnReadyRead: packet ") + QString::number(m_MsgHeader.msg_type) + ", couldn't allocate " + QString::number(m_MsgHeader.msg_size));
       break; }
     memset(m_pMessageBuff, 0, m_MsgHeader.msg_size); // cppcheck will give a warning here, but I am keeping this line. memset is a fine way to zero out a buffer.
     memcpy(m_pMessageBuff, &m_MsgHeader, sizeof(m_MsgHeader));
     m_lMsgBuffUsed = sizeof(m_MsgHeader);
     }

   // we have the header and memory has been allocated
   if(m_lMsgBuffUsed < m_MsgHeader.msg_size) {
     llBytedRead = read(m_pMessageBuff + m_lMsgBuffUsed, m_MsgHeader.msg_size - m_lMsgBuffUsed);

     if(!llBytedRead || llBytedRead == -1) break;

     m_lMsgBuffUsed += llBytedRead;
     if(m_lMsgBuffUsed < m_MsgHeader.msg_size) {
       // don't have a full message, exit
       break; }
     }
   // Ok, a full message. Check CRC and discard the entire packet if  it doesn't match
   pMsg = reinterpret_cast<CWVSSMsgHeader *>(m_pMessageBuff);
   if(pMsg->CalcCRC() == pMsg->crc) {
     // we don't want to process every WVSS_msg_planepos message because the next one may override this one. So if there are lots of WVSS_msg_planepos messages coming through, we save each
     // one and don't send it until either the next message is not WVSS_msg_planepos, or there are no more messages to receive (after the loop)
     //if(pMsg->msg_type == WVSS_msg_planepos) {
  	  //  pMsgBuffSave = m_pMessageBuff;
     //  ulDiscarded ++;
     //  //QThread::msleep(5);
     //  }
	    //else {
     //  if(pMsgBuffSave) {
     //    MsgHeader = QSharedPointer<char>(pMsgBuffSave);
     //    emit Process(this, MsgHeader);         
     //    }
       MsgHeader = QSharedPointer<char>(m_pMessageBuff);
       emit Process(this, MsgHeader);
     //  pMsgBuffSave = NULL; }
     }
   else
     emit Message(this, AddThreadID("CWVSSTcpSocket::OnReadyRead: packet crc mismatch, msg type ") + QString::number(pMsg->msg_type) + ", packet discarded");

   // reset current message
   m_pMessageBuff = NULL;
   m_lMsgBuffUsed = 0;
   m_nMsgHeaderUsed = 0;
   } // while(true)

 //if(pMsgBuffSave) {
 //  MsgHeader = QSharedPointer<char>(pMsgBuffSave);
 //  emit Process(this, MsgHeader);
 //  ulDiscarded --; }

 //emit Message(this, AddThreadID("CWVSSTcpSocket::OnReadyRead: ") + QString::number(ulDiscarded) + " packets discarded");
}

bool CWVSSTcpSocket::OutOfThreadSend(QSharedPointer<char>p_Msg)
{
 emit OutOfThreadSendSignal(p_Msg);
 return(true);
}

bool CWVSSTcpSocket::OnOutOfThreadSend(QSharedPointer<char>p_Msg)
{
 const CWVSSMsgHeader *pMsgHeader = reinterpret_cast<CWVSSMsgHeader *>(p_Msg.data());
 return(Send(p_Msg.data(), pMsgHeader->msg_size));
}
