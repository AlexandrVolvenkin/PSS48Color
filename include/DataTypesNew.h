//------------------------------------------------------------------------------
// �������� ����� ������
//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
#ifndef __DATA_TYPES_H
#define __DATA_TYPES_H
//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
// ������ ������������ �����������
struct TFirmwareVersion
{
 unsigned char Major;
 unsigned char Minor;
 unsigned char Hardware;
};
//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
// ������������ ���������� (����� v1.60)
//#pragma bitfields = default                                                     // OK
struct TDevConfig
{
 unsigned char  ICount;			// ����� ������� �����
 unsigned char  OCount;			// ����� ������� ������
 unsigned char  Windows;		// ����� ���� ������������

 unsigned short FixConfig :  1;		// ������� ������������� ������������ 
 unsigned short MBusMaster:  1;		// ������� ������ ModBus-Master
 unsigned short 	  : 14;		// ������	

 unsigned char MBTimeout;		// ������� ������ ModBus
 unsigned char PRTimeout;               // ������� ���������������� ����������������� ������������

 char BYTE[13];				// ������
};
//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
// ��������� ���������� ModBus
struct TMBusSetting
{
 unsigned char NetAddress;		// ������� �����
 unsigned char BaudRate;		// �������� ������
 unsigned char LinkParam;		// ����� ����-���, �������
 unsigned char Interval;		// �������� �������
 unsigned char Prescaler;		// �������� �������
 unsigned char Silent[2];		// 2 ����� ��� �������������� ��������

 enum
 {
  STOPBITS_MASK = 0x08,
  PARITY_MASK   = 0x30,

  ONESTOPBITS	= 0x00,			// ���� ����-���
  TWOSTOPBITS	= 0x08,			// ��� ����-����
  NOPARITY	= 0x00,			// ��� ��������
  EVENPARITY	= 0x20,			// ������ �������
  ODDPARITY	= 0x30			// �������� ��������
 };
};
//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
// ��������� �������� ������� ���������
#pragma bitfields = reversed
union TChannelSetting
{
 struct
 {
  unsigned char NoSound:	1;	// ������� ���������� �����
  unsigned char :		3;
  unsigned char Type:		4;	// ��� ����������� / ���������
 };

 unsigned char Byte;

 enum TYPE_INDICATION
 {
  EMPTY		 = 0,			// ��� �����������
  INDICATION	 = 1,			// ���������
  PREVENTIVE	 = 2,			// ����������������� ������������
  EMERGENCY	 = 3,			// ��������� ������������

  IND_PREVENTIVE = 4,			// ��������� + ����������������� ������������
  IND_EMERGENCY  = 5			// ��������� + ��������� ������������
 };
};
//#pragma bitfields = default
//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
// ��������� ������������ ������� ��������� ModBus
struct TMBusHandler
{
 unsigned char Function;		// ����� �������
 unsigned char Lenght;			// ������ ���������
 unsigned char Flags;			// �����

 enum MBSH_FLAGS
 {
  FIX_LENGHT  = 0x00,			// ������ ������������� ������
  VAR_LENGHT  = 0x80			// ���� ���������� ������ ���������
 };

 unsigned char (*Handler)(unsigned char *Buff,unsigned char Lenght);

// TMBusHandler(void):Function(0xFF),Lenght(0),Flags(0),Handler(0){ };
};
//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
// ��������� ������� ModBus Master
struct TMBMRequest
{
 unsigned char  NetAddress;		// ������� ����� ����������
 unsigned char  Function;		// �������
 unsigned short Address;		// ����� ������
 unsigned char  Value;			// ��������
};
//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
// ��������� ������ �������� ModBus Master
struct TMBMRequestList
{
 unsigned char Number;			// ����� ��������� ������
 TMBMRequest   Request[16];		// ������� ModBus Master
};
//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
// ��������� �������� ������ ������ ������
#pragma bitfields = reversed
union TOutSetting
{
 struct
 {
  unsigned char :		2;
  unsigned char LocalReceipt:	1;	// ������������ � ������
  unsigned char LocalUnset:	1;	// ����� � ������
  unsigned char MBusReceipt:	1;	// ������������ � �������� ������
  unsigned char MBusUnset:	1;	// ����� � �������� ������
  unsigned char Mode:		2;	// ����� ������
 };

 unsigned char Byte;

 enum OUT_MODE
 {
  LIGHT_SIGNALING  = 0x00,		// �������� ������������
  SOUND_SIGNALING  = 0x02,		// �������� ������������
  CONTROL	   = 0x01,		// ����������� �����
  SELFHOLD_CONTROL = 0x03		// ����������� ����� � ���������������
 };

 operator unsigned char()		// �������� �������������� � unsigned char
 {
  return Byte; 
 };
};
//#pragma bitfields = default
//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
// ������, ������������ ������� ������ [##]
#pragma bitfields = reversed
struct TOutData
{
 unsigned char Current;			// ������� ���������
 unsigned char New;			// ����� ��������� ������������
 unsigned char ExtReceipt;		// ������� ������������
 unsigned char ExtUnset;		// ������� �����

 unsigned char :		3;
 unsigned char Blocking:	1;	// ����������
 unsigned char LocalUnset:	1;	// ����� � ������
 unsigned char LocalReceipt:	1;	// ������������ � ������
 unsigned char MBusUnset:	1;	// ����� � �������� ������
 unsigned char MBusReceipt:	1;	// ������������ � �������� ������
};
//#pragma bitfields = default
//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
// ��������� ������ ������
struct TOutputData
{
 unsigned char OutBase[8];		// ��������� ��-54
 unsigned char ReceiptAddr[8];		// ������ ������ �������� ������������
 unsigned char UnsetAddr[8];		// ������ ������ �������� ������
};
//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
/*
// ������������ ����������
struct TDevConfiguration
{
 unsigned char ICount;			// ����� ������� �����
 unsigned char OCount;			// ����� ������� ������

 bool FixConfig;			// ������� ������������� ������������ 
 bool MBusMaster;			// ������� ������ ModBus-Master
 bool LED16Mode;			// ������� ������������� ������� ���� ���������

 unsigned char MBTimeout;		// ������� ������ ModBus

 char BYTE[10];				// �������������� ����� ������
}; 
*/ 
//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
// �� �������� �������
struct TDataBase
{
 TDevConfig	DevConfig;		// ������������ ����������
 unsigned short CRC0;

 TMBusSetting   MBSet;			// ��������� ���������� ModBus
 unsigned short CRC1;

 unsigned char  InputType[8];		// ��� �����
 unsigned short CRC2;

 unsigned char  CrossBar[64];		// �������������
 unsigned short CRC3;

 unsigned char  ChannelType[32];	// ���� ������������ �� ����� 
 unsigned short CRC4;

 TOutputData    OutConfig;		// �� ������� ������
 unsigned short CRC5;

 unsigned char  Relay[64];		// �������� ����, �������������� ���������� ������
 unsigned short CRC6;

 TMBMRequestList InputList;		// ������ �������� ����� ������
 unsigned short  CRC7;

 TMBMRequestList ReceiptList;		// ������ �������� ������������
 unsigned short  CRC8;

 TMBMRequestList UnsetList;		// ������ �������� ������
 unsigned short  CRC9;

 enum DB_FIELDS
 {
  DEV_CONFIG	= 0,			// ������������ ����������
  MODBUS_SET	= 1,			// ��������� ���������� ModBus
  INPUT_TYPE	= 2,			// ��� �����
  CROSS_BAR	= 3,			// �������������
  CHANNEL_TYPE	= 4,			// ���� ������������ �� ����� 
  OUT_BASE	= 5,			// �� ������� ������
  RELAY		= 6,			// �������� ����, �������������� ���������� ������
  INPUT_LIST	= 7,			// ������� ����� ������
  RECEIPT_LIST	= 8,			// ������� ������������
  UNSET_LIST	= 9			// ������� ������
 };
};
//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
// �� �������� ������� - �������� 64 ����
struct TDataBaseNEW
{
 TDevConfig	DevConfig;		// ������������ ����������
 unsigned short CRC0;

 TMBusSetting   MBSet;			// ��������� ���������� ModBus
 unsigned short CRC1;

 unsigned char  InputType[2*8];		// ��� ����� (2 * 64 ��� / 8 = 16 ����)
 unsigned short CRC2;

 unsigned char  CrossBar[2*64];		// �������������
 unsigned short CRC3;

 unsigned char  ChannelType[2*32];	// ���� ������������ �� ����� 
 unsigned short CRC4;

 TOutputData    OutConfig;		// �� ������� ������
 unsigned short CRC5;

 unsigned char  Relay[2*64];		// �������� ����, �������������� ���������� ������
 unsigned short CRC6;

 TMBMRequestList InputList;		// ������ �������� ����� ������
 unsigned short  CRC7;

 TMBMRequestList ReceiptList;		// ������ �������� ������������
 unsigned short  CRC8;

 TMBMRequestList UnsetList;		// ������ �������� ������
 unsigned short  CRC9;

 enum DB_FIELDS
 {
  DEV_CONFIG	= 0,			// ������������ ����������
  MODBUS_SET	= 1,			// ��������� ���������� ModBus
  INPUT_TYPE	= 2,			// ��� �����
  CROSS_BAR	= 3,			// �������������
  CHANNEL_TYPE	= 4,			// ���� ������������ �� ����� 
  OUT_BASE	= 5,			// �� ������� ������
  RELAY		= 6,			// �������� ����, �������������� ���������� ������
  INPUT_LIST	= 7,			// ������� ����� ������
  RECEIPT_LIST	= 8,			// ������� ������������
  UNSET_LIST	= 9			// ������� ������
 };
};
//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
#endif //__DATA_TYPES_H
//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
