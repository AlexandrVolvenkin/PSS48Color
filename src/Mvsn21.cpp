//-----------------------------------------------------------------------------------------------------
//  Source      : FileName.cpp
//  Created     : 01.06.2022
//  Author      : Alexandr Volvenkin
//  email       : aav-36@mail.ru
//  GitHub      : https://github.com/AlexandrVolvenkin
//-----------------------------------------------------------------------------------------------------
#include "Mvsn21.h"
#include "Platform.h"
//#include "Config.h"
#include "Pss21.h"
#include "Crc.h"

//-----------------------------------------------------------------------------------------------------
CMvsn21Driver::CMvsn21Driver(uint8_t uiType)
{
    m_uiType = uiType;
}

//-----------------------------------------------------------------------------------------------------
CMvsn21Driver::~CMvsn21Driver()
{

}

//-----------------------------------------------------------------------------------------------------
void CMvsn21Driver::Allocate(TMemoryAllocationContext &xMemoryAllocationContext)
{
    m_uiAddress = xMemoryAllocationContext.uiAddress;
    m_puiRxBuffer = xMemoryAllocationContext.puiRxBuffer;
    m_puiTxBuffer = xMemoryAllocationContext.puiTxBuffer;
    m_puiErrorCode = xMemoryAllocationContext.puiErrorCode;
    // ������� ��������� �� ����� � ������� ���������� ������ ��� �������� ������.
    m_puiDiscreteInputs =
        &xMemoryAllocationContext.
        puiDiscreteInputs[xMemoryAllocationContext.uiUsedDiscreteInputs];
    // �������� ����� ����� ���������� ������.
    xMemoryAllocationContext.
    uiUsedDiscreteInputs +=
        MVSN21_DISCRETE_INPUTS_NUMBER;

    // ������� ��������� �� ����� � ������� ������������� ���������� ������ ��� �������� ������.
    m_puiDiscreteInputsBadState =
        &xMemoryAllocationContext.
        puiDiscreteInputsBadState[xMemoryAllocationContext.uiUsedDiscreteInputsBadState];
    // �������� ����� ����� ���������� ������.
    xMemoryAllocationContext.
    uiUsedDiscreteInputsBadState +=
        MVSN21_DISCRETE_INPUTS_NUMBER;

    // ������� ��������� �� ����� � ������� ��������� ������ ��� �������� ������.
    m_puiErrorAlarmDataArray =
        &xMemoryAllocationContext.
        puiErrorAlarmDataArray[DISCRETE_INPUT_MODULE_FAILURE];

    m_uiBadAnswerCounter = 0;
}

//-----------------------------------------------------------------------------------------------------
uint8_t CMvsn21Driver::DataExchange(void)
{
    m_puiTxBuffer[0] = DATA_EXCHANGE_COMMAND;

    CPss21::ConnectDevice(m_uiAddress);
    delay_us(500);
    CSpi::Exchange(m_puiRxBuffer,
                   m_puiTxBuffer,
                   (PRIAMBLE_LENGTH +
                    DATA_EXCHANGE_COMMAND_ANSWER_LENGTH +
                    ONE_BYTE_CRC_LENGTH));
    CPss21::DisconnectDevice();

    uint8_t *puiDestination = m_puiDiscreteInputs;
    uint8_t *puiDestinationBadState = m_puiDiscreteInputsBadState;

    // ��� ������� ������?
    switch(m_puiRxBuffer[COMMAND_BYTE_OFFSET])
    {
    case DATA_READY:
        // ������ �� ����������?
        if ((iCrcSummOneByteCompare(&m_puiRxBuffer[COMMAND_BYTE_OFFSET],
                                    (COMMAND_BYTE_LENGTH +
                                     DATA_EXCHANGE_COMMAND_ANSWER_LENGTH)) > 0))
        {
            m_uiBadAnswerCounter = 0;
            SetErrorAlarmData(0);
            // �������� ���������� �� ������ ������ � ������� ������ �������.

            for (uint8_t i = 0; i < DISCRETE_INPUT_BYTE_QUANTITY; i++)
            {
                uint8_t uiData = (m_puiRxBuffer[DATA_BYTE_OFFSET + i]);

                for (uint8_t j = 0; j < DISCRETE_INPUTS_IN_BYTE_QUANTITY; j++)
                {
//  ���� ��������� ������ ����� ������������ � ���� ����� ������� ����� ���������� �������� Mvsn21
//    enum INPUT_STATE_CODE
//    {
//        WIRE_BREAK	= 0x02,			// ����� �����
//        ON		= 0x01,			// ������������ �������
//        OFF		= 0x00,			// ���������� �������
//        SHORT_CIRCUIT	= 0x03			// �������� ���������
//    };
                    uint8_t uiResult = (uiData & 0x03);
                    uiData >>= 2;
                    *puiDestinationBadState++ = uiResult;
                }
            }
            return 1;
        }
        break;

    case DATA_NOT_READY:
        for (uint8_t i = 0;
                i < (DISCRETE_INPUT_BYTE_QUANTITY * DISCRETE_INPUTS_IN_BYTE_QUANTITY);
                i++)
        {
            *puiDestinationBadState++ = INPUT_IS_INVALID;
        }
        break;

    default:
        for (uint8_t i = 0;
                i < (DISCRETE_INPUT_BYTE_QUANTITY * DISCRETE_INPUTS_IN_BYTE_QUANTITY);
                i++)
        {
            *puiDestinationBadState++ = INPUT_IS_INVALID;
        }
        break;
    };

    // ������ �� ������� �����������?
    if (m_uiBadAnswerCounter < BAD_MODULE_CYCLE_COUNT_DEFAULT)
    {
        // �������� �������� �������� - "���������� ������� ����� � ������� ��� ������".
        m_uiBadAnswerCounter++;
        return 0;
    }
    else
    {
        for (uint8_t i = 0;
                i < (DISCRETE_INPUT_BYTE_QUANTITY * DISCRETE_INPUTS_IN_BYTE_QUANTITY);
                i++)
        {
            *puiDestinationBadState++ = INPUT_IS_INVALID;
        }
        // ������ ������� �����������.
        *m_puiErrorCode = IMD_ERROR;
        SetErrorAlarmData(1);
        return 0;
    }
}
