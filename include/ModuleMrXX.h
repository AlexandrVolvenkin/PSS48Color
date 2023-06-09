//-----------------------------------------------------------------------------------------------------
//  Source      : FileName.cpp
//  Created     : 01.06.2022
//  Author      : Alexandr Volvenkin
//  email       : aav-36@mail.ru
//  GitHub      : https://github.com/AlexandrVolvenkin
//-----------------------------------------------------------------------------------------------------
#ifndef CMODULEMRXX_H
#define CMODULEMRXX_H

//-----------------------------------------------------------------------------------------------------
#include <stdint.h>
#include "Driver.h"
#include "DataTypes.h"

#include "Timer.h"
//#include "Task.h"
#include "Dfa.h"

//-----------------------------------------------------------------------------------------------------
enum
{
// ��������� �������� - ������ ���� ������.
    MRXX_FSM_STATE_WRITE_DATABASE = 0x00,
// ��������� �������� - ���������� �����.
    MRXX_FSM_STATE_TRANSFER = 0x01,
};

enum
{
// ����� ��������� ������� 8 ���� ������. ������ ��53.
// ���� - 2: ��������� ������� ����, ���� ��� = 1 - �������� ���������.
    MRXX_STAT_DO_SHORT_CIRCUIT_BIT_OFFSET = 0,
// ���� - 3: ��������� ������� ����, ���� ��� = 1 - �����.
    MRXX_STAT_DO_BREACK_BIT_OFFSET = 1,
// ��� ������� - ���������-���������� ����.
    MRXX_ERROR_CODE_RELAY_ON_OFF = 0x01,
// ��� ������� - ���������-���������� ����.
    MRXX_ERROR_CODE_RELAY_SHORT_CIRCUIT = 0x04,
// ��� ������� - ���������-���������� ����.
    MRXX_ERROR_CODE_RELAY_BREACK = 0x08,
};

enum
{
// ���� ���������� �� ����� f_bou.
// ������������ � �������� ������.
    GLOBAL_KVIT_PC_BIT = 0,
// ����� � �������� ������.
    GLOBAL_RESET_PC_BIT = 1,
// ������������ � ���������� ������ ����.
    GLOBAL_KVIT_BUTTON_BIT = 2,
// ����� � ���������� ������ ����.
    GLOBAL_RESET_BUTTON_BIT = 3,
// ����� ����������.
    GLOBAL_BLOCK_BIT = 4,
};

enum
{
// ���� ��������� ���� ������ �������� ���������� ��������.
// ���� 0 (���.,���,���,���,���,���,��/��,���).
// ��� � ��� ������ 0 � ���������������, 1 � �����������
    OUTPUT_TYPE_MASK = 0x01,
// ��/��: ��� ������������ ������: 1 � � �����������, 0 � ��� ����������;
// ��� ���������������� ������: - 1 ������������ � �������� (����),
// 0 � ������������ � ��������� � ���������� �������� (����);
    BLOCK_ALARM_TYPE_MASK = 0x02,
// ��� � ����� � �������� ������: 1 � ����, 0 � ���;
    PC_RESET_MASK = 0x04,
// ��� � ������������ � �������� ������: 1 � ����, 0 � ���; (������ ��� ����. ������);
    PC_KVIT_MASK = 0x08,
// ��� � ����� � ����������� ������ ����: 1 � ����, 0 � ���;
    BUTTON_RESET_MASK = 0x10,
// ��� � ������������ � ����������� ������ ����: 1 � ����, 0 � ��� (������ ��� ����. ������);
    BUTTON_KVIT_MASK = 0x20,
};

//-----------------------------------------------------------------------------------------------------




//-----------------------------------------------------------------------------------------------------
class CModuleMrXXDriver : public CDriver, public CDfa
{
public:
    enum
    {
        DATA_EXCHANGE_COMMAND = 0x42,
        DATA_READY = 0x42,
        // ���������� ���������� ������
        DATA_EXCHANGE_OK	= 0x7E,
        DATA_EXCHANGE_OK_LENGTH = 1,
        // ������ �� ������
        DATA_NOT_READY	= 0x24,
        // �� ����������
        DBASE_ERR	= 0x24,
        // ������ �� �������� ���� ������ �� ��� � �� - $81.
        MRXX_SET_DATA_BASE_COMMAND = 0x81,
        // ���������� ���� � ������ ������ �������, �����, CRC - 2 ����� � ����� �������������.
        MRXX_GET_DISCRETE_OUTPUT_DATA_COMMAND_ANSWER_LENGTH = 5,
        // ���������� ���� � ������ ������ ���� ������, �����, CRC - 2 ����� � ����� �������������.
        MRXX_SET_DATA_BASE_COMMAND_ANSWER_LENGTH = 8,
        // ����� ��������� ��������� �� ���������� PREAMBLE_LENGTH + TWO_BYTE_CRC_LENGTH.
        DATA_EXCHANGE_COMMAND_ANSWER_LENGTH = 5,
        PRIAMBLE_LENGTH = 2,
        // 2 - �������� �� ������ � ������� RX SPI. 0 - ������� �������, 1 - "���" ������� �������.
        DATA_BYTE_OFFSET = 2,
        // 1 - �������� �� ������� � ������� RX SPI.
        COMMAND_BYTE_OFFSET = 1,
        COMMAND_BYTE_LENGTH = 1,
    };

    CModuleMrXXDriver(uint8_t uiType);
    CModuleMrXXDriver();
    virtual ~CModuleMrXXDriver();
    void Allocate(TMemoryAllocationConext &xMemoryAllocationConext);
    uint8_t WriteDataBase(void);
    uint8_t DataExchange(void);
    uint8_t Exchange(void);

protected:
private:
    uint8_t m_uiAddress;
    uint8_t *m_puiRxBuffer;
    uint8_t *m_puiTxBuffer;
    uint8_t *m_puiDiscreteInputs;
    uint8_t *m_puiDiscreteOutputState;
    TDiscreteOutputControl *m_pxDiscreteOutputControl;
    TOutputData *m_pxDiscreteOutputDataBase;
    uint8_t m_uiBadAnswerCounter;
};
//-----------------------------------------------------------------------------------------------------
#endif // CMODULEMRXX_H
