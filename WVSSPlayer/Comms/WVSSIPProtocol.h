#pragma once 

#pragma pack(push, 1)

#define WVSS_IP_PROTOCOL_VERSION      1
#define WVSS_IP_PROTOCOL_TCPSYNCBYTES 0xABCD

extern char *g_pXORKey;

enum eWVSSMsgType
{
 WVSS_msg_unknown                 = 0x00,
 // WVSS -> Visualizer
 WVSS_msg_data                    = 0x01,
 WVSS_msg_area                    = 0x02,
 WVSS_msg_planepos                = 0x03,
 WVSS_msg_loadplane               = 0x04,
 WVSS_msg_centertoplane           = 0x05,
 WVSS_msg_track                   = 0x06
};

struct rWVSSGeoPoint
{
 double m_dLat, m_dLong, m_dH; // lat and long are geo, H is in meters MSL
};

struct rWVSSToVisLine
{
 rWVSSGeoPoint m_Points[2];
 float m_fLineWidth; 
 unsigned char m_Color[3];
};

struct rWVSSToVisCircle
{
 rWVSSGeoPoint m_Center;
 float m_fUSRad; 
 unsigned char m_Color[3];
 float m_fLineWidth;
 float m_fHeading, m_fPHCti, m_fBank;
};

struct rWVSSToVisRectangle
{
 rWVSSGeoPoint m_Center;
 float m_fHorzExtent, m_fVertExtent; // m_fHorzExrent extent is the distance from the center to the left and right. Vertical - from the center up and down.
 unsigned char m_Color[3];
 float m_fLineWidth;
 float m_fHeading, m_fPHCti, m_fBank;
};

struct rWVSSToVisUnlimitedLine
{
 unsigned long p_lPointCount;
 unsigned char m_Color[3];
 float m_fLineWidth;
 // next follow rWVSSGeoPoint structures
 //rWVSSGeoPoint *m_pPoints;
};

class CWVSSMsgHeader
{
public:
 unsigned short sync;    // синхронизационные символы
 unsigned char crc;      // контрольная сумма
 short protocol_id;      // версия протокола 
 short msg_type;         // тип сообщения от лидара (см enum msg_type)
 unsigned long msg_size; // размер всего пакета сообщения (всего, вместе с этой структурой) 
public:
 CWVSSMsgHeader() { memset(this, 0, sizeof(CWVSSMsgHeader)); }
 inline static unsigned char XORCRC(unsigned char *p_pBytes, unsigned long p_lBytes) { 
   unsigned char CRC = 0;
   for(unsigned long  li = 0; li < p_lBytes; li ++)
     CRC = CRC ^ p_pBytes[li];
   return(CRC); }
 unsigned char CalcCRC() { 
   return(XORCRC((unsigned char *)&protocol_id, msg_size - (((unsigned char *)&crc - (unsigned char *)this + sizeof(crc)))));
   }
 void AssignCRC() { 
  crc = CalcCRC();
  }
 void BeforeSend() { // fill in values that are either consts or computed in an automated fashion
   sync = WVSS_IP_PROTOCOL_TCPSYNCBYTES;
   protocol_id = WVSS_IP_PROTOCOL_VERSION;
   AssignCRC(); }
};

class CWVSSArea : public CWVSSMsgHeader
{
public:
 char m_AreaName[32];
 //rWVSSGeoPoint m_Point;
 //double m_dX; // old visualizer variable
 //double m_dZ; // old visualizer variable
 //char m_ImageName[64];
 //unsigned long m_lAreaSize_m; // size of the texture coverage in meters (45000)

public:
 CWVSSArea() { memset(m_AreaName, 0, sizeof(m_AreaName)); /*m_Point.m_dLat = m_Point.m_dLong = m_Point.m_dH = 0; m_dX = m_dZ = 0; memset(m_ImageName, 0, sizeof(m_ImageName)); m_lAreaSize_m = 45000;*/ }
};

class CWVSSData : public CWVSSMsgHeader
{
public:
 unsigned long m_ulWVSSToVisLineCount;
 unsigned long m_ulWVSSToVisCircleCount;
 unsigned long m_ulWVSSToVisRectangleCount;
 // next follow rWVSSToVisLine, rWVSSToVisCircle and rWVSSToVisRectangle structures if present
public:
 CWVSSData() { m_ulWVSSToVisLineCount = m_ulWVSSToVisCircleCount = m_ulWVSSToVisRectangleCount = 0; }
};

class CWVSSTrack : public CWVSSMsgHeader
{
public:
 unsigned long m_ulWVSSToVisUnlimitedLineCount;
 // next follow rWVSSToVisUnlimitedLine structures
public:
 CWVSSTrack() { m_ulWVSSToVisUnlimitedLineCount = 0; }
};

struct rWVSSPlaneType
{
 char m_PlaneType[16];
};

struct rWVSSPCallSign
{
 char m_CallSign[32];
};

struct rWVSSToVisPlane
{
 rWVSSPlaneType m_PlaneType;
 rWVSSPCallSign m_CallSign;
 rWVSSGeoPoint m_Position;
 double m_dPHCti, m_dBank, m_dHeading;
 double m_dWingspan; // used when there is no plane match in the Vis and the default is chosen. There is a need to scale the default plane.
 unsigned short m_nScale; // to oversize planes for visibility purposes
 bool m_bShowAirBlock;
 bool m_bCenterTo; // the same idea as in WVSS_msg_centertoplane, only combined with positions
};

class CWVSSPlanePosition : public CWVSSMsgHeader
{
public:
 unsigned short p_nWVSSToVisPlaneCount;
 // next follow rWVSSToVisPlane structures if present
 //rWVSSToVisPlane *m_Planes;

public:
 CWVSSPlanePosition() { p_nWVSSToVisPlaneCount = 0; }
};

class CWVSSPlaneTypeList : public CWVSSMsgHeader
{
public:
 short m_nPlaneTypeCount;
 // next follow rWVSSPlaneType structures if present
 //rWVSSPlaneType *m_PlaneTypes;
public:
 CWVSSPlaneTypeList() { m_nPlaneTypeCount = 0; } 
};

class CWVSSCenterToPlane : public CWVSSMsgHeader
{
public:
 rWVSSPCallSign m_CallSign;
public:
 CWVSSCenterToPlane() { } 
};

#pragma pack(pop)
