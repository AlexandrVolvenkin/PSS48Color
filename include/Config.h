//-----------------------------------------------------------------------------------------------------
//  Sourse      : FileName.cpp
//  Created     : 01.06.2022
//  Author      : Alexandr Volvenkin
//  email       : aav-36@mail.ru
//  GitHub      : https://github.com/AlexandrVolvenkin
//-----------------------------------------------------------------------------------------------------
#ifndef CONFIG_H_INCLUDED
#define CONFIG_H_INCLUDED


//-----------------------------------------------------------------------------------------------------
// ������� ��������� �������. ���������� ������� ���� ��������.
#define MAX_HANDLED_DISCRETE_INPUT 48
#define MAX_HANDLED_DISCRETE_OUTPUT 64
#define ALARM_GROUP_QUANTITY 16
// ���������� ������� ���������� ����.
#define MRXX_DISCRETE_OUTPUT_QUANTITY 8
// HMI ������.
#define DISPLAY_MODULE_TYPE 1
#define DISPLAY_MODULE_TYPE_12_CELLS_LCD_4x20 0x00
#define DISPLAY_MODULE_TYPE_24_CELLS_LCD_4x20 0x01
#define DISPLAY_MODULE_TYPE_LCD_320x240 0x02
#define DISPLAY_MODULE_TYPE_6_CELLS_LCD_4x20 0x03
#define DISPLAY_MODULE_TYPE_LCD_4x20 0x04

#define INTERNAL_MODULE_QUANTITY 4
#define EXTERNAL_MODULE_FUNCTION_1234_QUANTITY 16  // ������������ ������ ������ �������������.
#define EXTERNAL_MODULE_FUNCTION_15_QUANTITY 2
// �� 30 ������� modbus function 1,2,3,4  � �� 2 ������� modbus function 15.
#define EXTERNAL_MODULE_QUANTITY (EXTERNAL_MODULE_FUNCTION_1234_QUANTITY + EXTERNAL_MODULE_FUNCTION_15_QUANTITY)
#define MAX_MODULE_QUANTITY (1)

#define PLC_DATA_BASE_BLOCK_QUANTITY 100
#define PLC_DATA_BASE_BLOCK_LENGTH 256
#define DATA_BASE_BLOCK_NUMBER_DATA_LENGTH 1
#define CURRENT_TIME_BYTE_QUANTITY 7
#define MODIFICATION_CODE_BYTE_QUANTITY 23

// ���������� ���������� ������ � ����� ������.
#define DISCRETE_MODULE_INPUT_QUANTITY 12
#define ANALOGUE_INPUT_DI_VALUE_QUANTITY 4 // ���������� ���������� �������� ����������� ����� ���������� ������.
#define DISCRETE_OUTPUT_MODULE_RELAY_OUTPUT_QUANTITY 64 // ���������� ���������� ������� ���������� ����.
// ���������� ���������� ������ � ����� ������.
#define ANALOG_MODULE_INPUT_QUANTITY 6
// ���������� ���������� ������� � ����� ������.
#define ANALOG_OUTPUT_MODULE_REGULATORS_QUANTITY 6
#define ONLINE_DATA_READ_ONE_INPUT_BYTE_QUANTITY 8 // 7 ���� ASCII ������� ���������������� �� float AiValue, 1 ���� StatAi
#define ONLINE_DATA_READ_ONE_INPUT_ASCII_BYTE_QUANTITY 7 // 7 ���� ASCII ������� ���������������� �� float AiValue
#define ONLINE_DATA_READ_ONE_INPUT_STAT_AI_BYTE_QUANTITY 1 // 1 ���� StatAi - ��������� ���������� ������������� �������
#define CALIBRATION_ANALOG_MODULE_NUMBER_MASK 0x00F0 // �����. ����� Modbus function 5 - 0x00X0, X - ����� ������.
#define CALIBRATION_INPUT_NUMBER_MASK 0x000F // �����. ����� Modbus function 5 - 0x000Y, Y - ����� ������������ �����.
// ������� Modbus 71 - _FC_ONLINE_DATA_READ modbus_reply() modbus.cpp.
// ���� � ������� �������� ������ Modbus ���7 = 0, �� ������������� �������� ����� - (���0 - ���6) - ����� ����������� �����.
#define ANALOGUE_INPUT_ADDRESS_MASK 0x7F
// ���� � ������� �������� ������ Modbus ���7 = 1, �� ������������� ��� � (���0 - ���2) - ������������� ����� ������ ����3.
#define ANALOGUE_MODULE_ADDRESS_MASK 0x07

#define DEFAULT_SERVER_ID 1
#define SERVER_ID 17
#define HMI_SLAVE_ADDRESS 1
#define INVALID_SERVER_ID 18
#define ttyO0 USART0
#define ttyO1 USART1
#define MODBUS_RTU_BAUD_RATE          9600//57600//38400//19200//
#define DEBUG_PORT_BAUD_RATE          9600
#define MODBUS_RTU_ENABLE
//#define MODBUS_TCP_ENABLE
//#define MODBUS_HMI_ENABLE

// �������������� ��������� - �������� ��������� ��������� Modbus.
#define MODBUS_MESSAGE_Sourse_TCP 1
#define MODBUS_MESSAGE_Sourse_RTU_HIGH_LEVEL 2
#define MODBUS_MESSAGE_Sourse_RTU_HMI 3
#define MODBUS_MESSAGE_Sourse_RTU_MASTER 4

//-----------------------------------------------------------------------------------------------------
#define PERIPHERY_SCAN_TIME 10 // ������ ������������ ��������� - 10mc.
#define PERIPHERY_SCAN_TIMER_ID 0
#define MAIN_CYCLE_TIMER_ID 1
#define MAIN_CYCLE_TIME 100 // ����� �������� ����� - 100mc.
#define MODBUS_RTU_35_TIMEOUT_TIMER_ID 2

#define ONE_SECOND_TIME 10 // ���������� �������� �������� ����� - MAIN_CYCLE_TIME.
#define TWO_SECOND_TIME 20
#define THREE_SECOND_TIME 30
#define TEN_SECOND_TIME 100

#define LED_TIMER_ID 3
#define LED_CYCLE_TIME 1000

#define DATA_EXGANGE_TIMER_ID 4
#define DATA_EXGANGE_CYCLE_TIME 1

#define HD44780_LCD_LIGHT_CYCLE_TIME (180000 / MAIN_CYCLE_TIME)


#define BITS_IN_BYTE_QUANTITY 8

#define ANALOGUE_INPUT_ERROR 0xFE00
#define ONE_BYTE_CRC_LENGTH 1
#define TWO_BYTE_CRC_LENGTH 2
// 2 - �������� �� ��� ������ � ������� RX SPI. 0 - ������� �������, 1 - "���" ������� �������.
#define MODULE_TYPE_OFFSET 2
#define MODULE_COMMAND_LENGTH 1
// ���������� ���� � ������ �� ������ ���� ������.
#define MODULE_COMMAND_ANSWER_LENGTH 1
// ���������� ���� � "������" ������ ������. 0x7E - OK, 0xE7 - error.
#define TAIL_ANSWER_LENGTH 1
#define DATA_EXCHANGE_OK 0x7E
#define DATA_EXCHANGE_ERROR 0xE7

#define PERCENT_100 100 // ��� ���������.

#define ANALOGUE_INPUT_SET_POINT_VIOLATION_L 0x03 // 3 � ��������� ������� L.
#define ANALOGUE_INPUT_SET_POINT_VIOLATION_LL_L 0x04 // 4 � ��������� ������� LL+L.
#define ANALOGUE_INPUT_SET_POINT_VIOLATION_H 0x05 // 5 � ��������� ������� H.
#define ANALOGUE_INPUT_SET_POINT_VIOLATION_HH_H 0x06 // 6 � ��������� ������� �+��.
#define ANALOGUE_INPUT_SET_POINT_VIOLATION_MASK 0x07
#define ANALOGUE_INPUT_CHANNEL_CALIBRATION 0x20 // ����� ��������� � ������ ����������.
#define ANALOGUE_INPUT_LINE_BREAK 0x40 // ����� ����� ������.
#define ANALOGUE_INPUT_CALCULATION_OVERFLOW 0x80 // ������������ ��� ��������.


#define DATA_BASE_FLOW_CONTROL_OK 1
#define DATA_BASE_FLOW_CONTROL_ERROR 2

// ��������� - ������� ������ ������������.
#define CONFIGURATION_ERROR 0xE0

#define CONFIGURATION_FLOW_CONTROL_EXIT 0
#define CONFIGURATION_FLOW_CONTROL_BUS_CHECK 1
#define CONFIGURATION_FLOW_CONTROL_SEQUENCE_CHECK 2
#define CONFIGURATION_FLOW_CONTROL_SERVICE_DATA_CALCULATE 3
#define CONFIGURATION_FLOW_CONTROL_CONFIGURATION_CHECK 4
#define CONFIGURATION_FLOW_CONTROL_NORMAL_FLOW 5
#define CONFIGURATION_FLOW_CONTROL_ERROR_FLOW_STOP 6

#define GET_MODULE_TYPE_COMMAND 0x91 // ������� - ������ ���� ������.
#define GET_MODULE_TYPE_COMMAND_LENGTH 1
#define GET_MODULE_TYPE_COMMAND_ANSWER_LENGTH 1
// �������� � ������� RX SPI. 0 - ������� �������, 1 - "���" ������� �������.
#define GET_MODULE_TYPE_COMMAND_OFFSET 1

// ������ ������� aucCoilsArray[].
#define COILS_FULL_ARRAY_LENGTH 4
#define COILS_WORK_ARRAY_LENGTH 4

//-----------------------------------------------------------------------------------------------------
// ��� ������������, ������� ������ ���������� �� ����������� �������� ������.
#pragma pack(push)
#pragma pack(1)
struct TConfigDataPackOne // ������ ��� ������� ���� ����������, ������� modbus - 17.
{
    // Modbus ����� �������.
    uint8_t ucModbusSlaveAddress;
    // ����������� ���������� �������.
    uint8_t ucModulesQuantity;
    uint8_t aucModuleType[INTERNAL_MODULE_QUANTITY]; // ���� ������� ����� (������) �� ������� SPI.
    uint8_t ucLastDiscreteInputModuleAddressPlusOne; // ����� ���������� ������ ����������� ����� +1
    uint8_t ucLastAnalogueInputModuleAddressPlusOne; // ����� ���������� ������ ����������� ����� +1
    uint8_t ucDiscreteOutputQuantity;
    uint16_t ui16DiscreteInputQuantity;
    uint8_t ucAnalogueInputQuantity;
    uint8_t ucDisplayType;
};
#pragma pack(pop)

struct TConfigData // ������ ��� ������� ���� ����������, ������� modbus - 17.
{

    // Modbus ����� �������.
    uint8_t ucModbusSlaveAddress;
    // ����������� ���������� �������.
    uint8_t ucModulesQuantity;
    uint8_t aucModuleType[INTERNAL_MODULE_QUANTITY]; // ���� ������� ����� (������) �� ������� SPI.
    uint8_t ucLastDiscreteInputModuleAddressPlusOne; // ����� ���������� ������ ����������� ����� +1
    uint8_t ucLastAnalogueInputModuleAddressPlusOne; // ����� ���������� ������ ����������� ����� +1
    uint8_t ucDiscreteOutputQuantity;
    uint16_t ui16DiscreteInputQuantity;
    uint8_t ucAnalogueInputQuantity;
    uint8_t ucDisplayType;
};
typedef struct TConfigData TConfigData;

//-----------------------------------------------------------------------------------------------------
// ��������� ������������ ���������� ����������� ������������ �� HMI.
struct TAlarmHmi
{
    // � ������ � ����� ����������� ������������.
    uint8_t ucGroupNumber;
    // 0-��� ������������, 1-���� ������������.
    bool fbAlarmIsSet;
    // 1 - ������������� ������, 0 - �� ������������� (��������) ������.
    bool fbEventIsKvit;
    // 1 - ������ ������ �����, 0- �� ������.
    bool fbResetIsSet;
};

//-----------------------------------------------------------------------------------------------------
// ����� ������ ������� aucDiscreteInputsArray[] modbus, function 2.

// 00-251 (0x00 - 0xfb)  ��������� ������� ���������� ��������: 0-OFF, 1-ON,
// ��� ����� ������������ �� 252 ���������� ��������;
#define DI_VALUE_BIT_ARRAY_LENGTH (MAX_HANDLED_DISCRETE_INPUT + MAX_HANDLED_FUNCTION_BLOCK)
// ������ ������� aucDiscreteInputsArray[].
#define DISCRETE_INPUTS_ARRAY_LENGTH 8
//#define DISCRETE_INPUTS_ARRAY_LENGTH DI_VALUE_BIT_ARRAY_LENGTH
// �������� � ������� aucDiscreteInputsArray[].
// ��������� ����� Modbus.
#define DISCRETE_INPUTS_ARRAY_MODBUS_BEGIN_ADDRESS 0x00
// 00-251 (0x00 - 0xfb)  ��������� ������� ���������� ��������: 0-OFF, 1-ON,
// ��� ����� ������������ �� 252 ���������� ��������;
#define DI_VALUE_BIT_ARRAY_OFFSET 0

//-----------------------------------------------------------------------------------------------------
// ����� ������ ������� aucHoldingRegistersArray[] modbus, function 3.
// 7.3 ������� 3 � ������ ������ � ������� float IEEE 754 (�� 2 2-� ������� ����
// ���� �� 1 ��������).
// 7.3.1 ���������� ���������.
// 00-94 (0x00-0x5�) ���������� �������� ������� ���������� �������� 1 � 48,
// ��� ����� ������������ �� 96 2-� ������� ��������� (48 ���������� ��������);
#define AI_VALUE_FLOAT_ARRAY_LENGTH MAX_HANDLED_ANALOGUE_INPUT
#define AI_VALUE_BYTE_ARRAY_LENGTH (AI_VALUE_FLOAT_ARRAY_LENGTH * sizeof(float))
// 7.3.2 ���� ������ ���������� ������������� �������
#define AI_VALUE_DATA_BASE_FLOAT_ARRAY_LENGTH ((MAX_HANDLED_ANALOGUE_INPUT * sizeof(struct TAnalogueInputDescriptionData)) / sizeof(float))//288
#define AI_VALUE_DATA_BASE_BYTE_ARRAY_LENGTH (MAX_HANDLED_ANALOGUE_INPUT * sizeof(struct TAnalogueInputDescriptionData))
// 7.3.7 ����� � ����
// 1000 � 1006  ������� ����� � ����: 7 2-� ������� ���������, ����� �����:
// ���, ���, ���, ���� ���, ��� ,���, ���� ������ (��� ������:1 - �����������),
// ������������ ��� ������ �16.
#define CURRENT_TIME_HOLDING_BYTE_ARRAY_LENGTH (CURRENT_TIME_BYTE_QUANTITY * sizeof(int16_t))
// ������ ������� aucHoldingRegistersArray[].
#define HOLDING_REGISTERS_ARRAY_LENGTH 8
//#define HOLDING_REGISTERS_ARRAY_LENGTH  AI_VALUE_DATA_BASE_BYTE_ARRAY_LENGTH + \
CURRENT_TIME_HOLDING_BYTE_ARRAY_LENGTH \
// �������� � ������� aucHoldingRegistersArray[].
// ��������� ����� Modbus.
#define HOLDING_REGISTERS_ARRAY_MODBUS_BEGIN_ADDRESS 0x00

// ������� ���� LittleEndian, ��� ���������� ������.

// ���������� �������� ������� ���������� �������� 1 � 48,
// ��� ����� ������������ �� 96 2-� ������� ��������� (48 ���������� ��������);
#define AI_VALUE_BYTE_ARRAY_OFFSET 0
// 7.3.2 ���� ������ ���������� ������������� �������
#define AI_VALUE_DATA_BASE_BYTE_ARRAY_OFFSET 0//AI_VALUE_BYTE_ARRAY_OFFSET + AI_VALUE_BYTE_ARRAY_LENGTH
// 7.3.7 ����� � ����
// 1000 � 1006  ������� ����� � ����: 7 2-� ������� ���������, ����� �����:
// ���, ���, ���, ���� ���, ��� ,���, ���� ������ (��� ������:1 - �����������),
// ������������ ��� ������ �16.
#define CURRENT_TIME_HOLDING_BYTE_ARRAY_OFFSET (AI_VALUE_DATA_BASE_BYTE_ARRAY_OFFSET + AI_VALUE_DATA_BASE_BYTE_ARRAY_LENGTH)

//-----------------------------------------------------------------------------------------------------
// ����� ������ ������� aucInputRegistersArray[] modbus, function 4.
// 7.4 ������� 4 � ������ ��������� ���������� ���������� �������� � �������
// �������������� ����������������� ���� 0-16383 � ������ ������ (�� 1 2-� ��������
// �������� �� 1 ��������).

// 7.4.3 ����� �������� �������
#define CURRENT_TIME_BYTE_ARRAY_LENGTH (CMind::CURRENT_TIME_BYTE_QUANTITY * sizeof(int16_t))
// 7.4.4 ���� ����������� �������
#define MODIFICATION_CODE_BYTE_ARRAY_LENGTH (MODIFICATION_CODE_BYTE_QUANTITY * sizeof(int16_t))
// ������ ������� aucInputRegistersArray[].
#define INPUT_REGISTERS_ARRAY_LENGTH 8
//#define INPUT_REGISTERS_ARRAY_LENGTH (CURRENT_TIME_BYTE_ARRAY_LENGTH + \
//                                     MODIFICATION_CODE_BYTE_ARRAY_LENGTH) \

// �������� � ������� aucInputRegistersArray[].
// ��������� ����� Modbus.
// 7.4.3 ����� �������� �������
// 1000-1006  7 ��������� �������� �������: �������, ������, ���, ���� ��-
// ����, �����, ��� (0-99), ���� ������ (1 � 7);
#define CURRENT_TIME_BYTE_ARRAY_OFFSET 0
//// ������ ��� �������� ���� - int16_t.
//#define CURRENT_TIME_SECOND_OFFSET 0
//#define CURRENT_TIME_MINUTE_OFFSET 1
//#define CURRENT_TIME_HOUR_OFFSET 2
//#define CURRENT_TIME_MONTH_DAY_OFFSET 3
//#define CURRENT_TIME_MONTH_OFFSET 4
//#define CURRENT_TIME_YEAR_OFFSET 5
//#define CURRENT_TIME_WEEK_DAY_OFFSET 6
// 7.4.4 ���� ����������� �������
// 1007-1030 - ������ ����� ����������� ������� (24 ��������)
// ���������� ��������� � ������:
// - ������� 1007 - ���������� ������� �����-������ (1 � 16);
// - �������� 1008 / 1023 - ���� ������� ����� (������) �� ������� (�������
// 1008 � ��� ������ ����� (������) � ������� �0�, ������� 1023 � ��� ������ ���-
// �� (������) � ������� �15�);
// - ������� 1024 - ����� ���������� ������ ����������� ����� +1;
// (BIaddr_max+1), ��� 0, ���� ��� ������� ����� ���������� ��������;
// - ������� 1025 - ����� ���������� ������ ����������� ����� +1
// (�Iaddr_max+1), ��� 0 ���� ��� ������� ����� ���������� ��������;
// - ������� 1026 - ���������� ���������� ������� (BOut);
// - ������� 1027 - ���������� �������� ������� �������� (BInp);
// - ������� 1028 - ���������� ���������� ������� �������� (�Inp);
// - ������� 1029� ��� ������ ��������� : 0 � 12 ����� �������� ������������
// + ��� (4�20), 1 � 24 ������ �������� ������������ +��� (4�20), 2 - ��� �����-
// ������,  3 � 6 ����� �������� ������������ +��� (4�20), 4 � ��� ����������� +
// ��� (4�20);
#define MODIFICATION_CODE_BYTE_ARRAY_OFFSET (CURRENT_TIME_BYTE_ARRAY_OFFSET + CURRENT_TIME_BYTE_ARRAY_LENGTH)

//-----------------------------------------------------------------------------------------------------
//extern char acDisplayTempString[22];
//extern const char accModbusRTU[];
//extern const char accGreen[];
extern const char acctty0[];
//extern const char acc10MsTmr[];
////extern const char accFloatFormatArgs[];
//extern __flash const char accInitTime[];

//extern TConfigData xConfigData;

//-----------------------------------------------------------------------------------------------------
// �������� modbus.
// ������� �������, ��������� ������.
//extern modbus_mapping_t *pxCommonModbusMapping;
//extern modbus_t xMasterRtuCtx;
//extern modbus_t *pxMasterRtuCtx;

#ifdef MODBUS_TCP_ENABLE
//extern modbus_t xSlaveTcpCtx;
//extern modbus_t *pxSlaveTcpCtx;
//extern modbus_mapping_t xTcpModbusMapping;
//extern modbus_mapping_t *pxTcpModbusMapping;
extern uint8_t aucTcpCoilsArray[COILS_WORK_ARRAY_LENGTH];
extern uint8_t aucTcpDiscreteInputsArray[DISCRETE_INPUTS_ARRAY_LENGTH];
extern uint8_t aucTcpHoldingRegistersArray[HOLDING_REGISTERS_ARRAY_LENGTH];
extern uint8_t aucTcpInputRegistersArray[INPUT_REGISTERS_ARRAY_LENGTH];
extern uint8_t aucTcpAuxiliaryArray[];
#endif // MODBUS_TCP_ENABLE

#ifdef MODBUS_RTU_ENABLE
//extern modbus_t xSlaveRtuCtx;
//extern modbus_t *pxSlaveRtuCtx;
//extern modbus_mapping_t xRtuModbusMapping;
//extern modbus_mapping_t *pxRtuModbusMapping;
extern uint8_t aucRtuCoilsArray[COILS_WORK_ARRAY_LENGTH];
extern uint8_t aucRtuDiscreteInputsArray[DISCRETE_INPUTS_ARRAY_LENGTH];
extern uint16_t aucRtuHoldingRegistersArray[HOLDING_REGISTERS_ARRAY_LENGTH];
extern uint16_t aucRtuInputRegistersArray[INPUT_REGISTERS_ARRAY_LENGTH];
extern uint8_t aucRtuAuxiliaryArray[];
extern uint8_t aui8ReceiveMessageBuff[];
extern uint8_t aui8TransmitMessageBuff[];
#endif // MODBUS_RTU_ENABLE

#ifdef MODBUS_HMI_ENABLE
//extern modbus_t xSlaveHmiRtuCtx;
//extern modbus_t *pxSlaveHmiRtuCtx;
//extern modbus_mapping_t xHmiRtuModbusMapping;
//extern modbus_mapping_t *pxHmiRtuModbusMapping;
extern uint8_t aucHmiRtuCoilsArray[COILS_WORK_ARRAY_LENGTH];
extern uint8_t aucHmiRtuDiscreteInputsArray[DISCRETE_INPUTS_ARRAY_LENGTH];
extern uint8_t aucHmiRtuHoldingRegistersArray[HOLDING_REGISTERS_ARRAY_LENGTH];
extern uint8_t aucHmiRtuInputRegistersArray[INPUT_REGISTERS_ARRAY_LENGTH];
extern uint8_t aucHmiAuxiliaryArray[];
#endif // MODBUS_HMI_ENABLE
#define MEMCPY_PGM memcpy_P

#endif // CONFIG_H_INCLUDED
