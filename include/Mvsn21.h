//-----------------------------------------------------------------------------------------------------
//  Source      : FileName.cpp
//  Created     : 01.06.2022
//  Author      : Alexandr Volvenkin
//  email       : aav-36@mail.ru
//  GitHub      : https://github.com/AlexandrVolvenkin
//-----------------------------------------------------------------------------------------------------
#ifndef CMVDS9_H
#define CMVDS9_H

#include <stdint.h>
#include "Driver.h"
#include "DataTypes.h"
#include "Dfa.h"

//-----------------------------------------------------------------------------------------------------
// ����� ������� ������ ����� ���������� �������� ����9 � ���
// * ������ �� ����������� �����:$42
// �����:$42-������ ������(���), $24-������ �� ������, ����� �����
// * �� ������:����.������ - 4�, ���.���� - 1� -> � sec1 - sec1+4
// ���� 1:D0,D1-����.��.1, D2,D3-����.��.2,D4,D5-����.��.3,D6,D7-����.��.4
// ���� 2:D0,D1-����.��.5, D2,D3-����.��.6,D4,D5-����.��.7,D6,D7-����.��.8
// ���� 3:D0,D1-����.��.9, D2,D3-����.��.10,D4,D5-����.��.11,D6,D7-����.��.12
// ���� 4:D0,D1-����.��.13, D2,D3-����.��.14,D4,D5-����.��.15,D6,D7-����.��.16
// ���� 4:D0,D1-����.��.17, D2,D3-����.��.18,D4,D5-����.��.19,D6,D7-����.��.20
// ���� 6:D0,D1-����.��.21, D2,D3-����.��.22,D4,D5-����.��.23,D6,D7-����.��.24
// ���� 7: � ���-05 �� ������������
// ���� 8: ���. ����
// ���� 9: �� ������ 1-9

//  ��������� ������ � ������ 1-3:
//  ������ ���� (D0,D2,D4,D6) =0 - ���������� (OFF), =1 - �������� (ON)
//  �������� ���� (D1,D3,D5,D7) =0 - ����������, =1 - ������������

class CMvsn21Driver : public CDriver, public CDfa
{
public:

    enum
    {
        DATA_EXCHANGE_COMMAND = 0x42,
        DATA_READY = 0x42,
        DATA_EXCHANGE_OK	= 0x7E,				// ���������� ���������� ������
        DATA_NOT_READY	= 0x24,				// ������ �� ������
        DBASE_ERR	= 0x25,				// �� ����������
        // ����� ��������� ��������� �� ���������� SPI_PREAMBLE_LENGTH + ONE_BYTE_CRC_LENGTH.
        DATA_EXCHANGE_COMMAND_ANSWER_LENGTH = 6,
        DISCRETE_INPUT_BYTE_QUANTITY = 6,
        DISCRETE_INPUT_BITS_IN_BYTE_QUANTITY = 8,
        DISCRETE_INPUTS_IN_BYTE_QUANTITY = 4,
        PRIAMBLE_LENGTH = 2,
        // 2 - �������� �� ������ � ������� RX SPI. 0 - ������� �������, 1 - "���" ������� �������.
        DATA_BYTE_OFFSET = 2,
        // 1 - �������� �� ������� � ������� RX SPI.
        COMMAND_BYTE_OFFSET = 1,
        COMMAND_BYTE_LENGTH = 1,
    };

    CMvsn21Driver(uint8_t uiType);
    virtual ~CMvsn21Driver();
    void Allocate(TMemoryAllocationConext &xMemoryAllocationConext);
    uint8_t DataExchange(void);

protected:
private:
    uint8_t m_uiAddress;
    uint8_t* m_puiRxBuffer;
    uint8_t* m_puiTxBuffer;
    uint8_t* m_puiDiscreteInputs;
    uint8_t m_uiBadAnswerCounter;
};

#endif // CMVDS9_H

