//-----------------------------------------------------------------------------------------------------
//  Sourse      : FileName.cpp
//  Created     : 01.06.2022
//  Author      : Alexandr Volvenkin
//  email       : aav-36@mail.ru
//  GitHub      : https://github.com/AlexandrVolvenkin
//-----------------------------------------------------------------------------------------------------
// SerialMT - serial multi trans.
//-----------------------------------------------------------------------------------------------------
#include <cstring>

#include "Platform.h"
#include "UartMT.h"
#include "SerialMT.h"
#include "Crc.h"
#include "SpiMT.h"

//-----------------------------------------------------------------------------------------------------
CSpiMT::CSpiMT()
{
    //ctor
}

//-----------------------------------------------------------------------------------------------------
CSpiMT::~CSpiMT()
{
    //dtor
}

//-----------------------------------------------------------------------------------------------------
void CSpiMT::Init(void)
{

}

//-----------------------------------------------------------------------------------------------------
void CSpiMT::Reset(void)
{
    m_pxDevice -> Reset();
    m_uiMessageLength = 0;
}

//-----------------------------------------------------------------------------------------------------
int8_t CSpiMT::ReceiveEnable(void)
{
    m_pxDevice -> Enable();
}

//-----------------------------------------------------------------------------------------------------
int8_t CSpiMT::ReceiveDisable(void)
{
    m_pxDevice -> Disable();
}

//-----------------------------------------------------------------------------------------------------
int16_t CSpiMT::Tail(uint8_t *puiMessage, uint16_t uiLength)
{
    uint16_t uiCrc = usCrc16(puiMessage, uiLength);
    puiMessage[uiLength++] = uiCrc & 0x00FF;
    puiMessage[uiLength++] = uiCrc >> 8;

    return uiLength;
}

//-----------------------------------------------------------------------------------------------------
int16_t CSpiMT::Send(uint8_t *puiMessage, uint16_t uiLength)
{
    return m_pxDevice -> Write(puiMessage, uiLength);
}

//-----------------------------------------------------------------------------------------------------
uint8_t CSpiMT::MessageIsSended(void)
{
    return m_pxDevice -> FrameIsSended();
}

//-----------------------------------------------------------------------------------------------------
int16_t CSpiMT::Receive(void)
{
    return m_pxDevice -> Read();
}

//-----------------------------------------------------------------------------------------------------
uint8_t CSpiMT::Select(void)
{
    return m_pxDevice -> ByteIsReceived();
}

//-----------------------------------------------------------------------------------------------------
int16_t CSpiMT::Receive(uint8_t *puiDestination, uint16_t uiLength)
{
    return m_pxDevice -> Read(puiDestination, uiLength);
}

//-----------------------------------------------------------------------------------------------------
uint8_t CSpiMT::FrameIsReceived(void)
{
    if ((CPlatform::GetSystemTick() - m_uiLastSystemTime)  >=
            m_uiGuardTimeout)
    {
        return 1;
    }
    else
    {
        return 0;
    }
};

//-----------------------------------------------------------------------------------------------------
uint8_t CSpiMT::ReceiveTimeIsOver(void)
{
    if ((CPlatform::GetSystemTick() - m_uiLastSystemTime)  >=
            m_uiReceiveTimeout)
    {
        return 1;
    }
    else
    {
        return 0;
    }
};

//-----------------------------------------------------------------------------------------------------
uint16_t CSpiMT::GetFrameLength(void)
{
    return m_pxDevice -> GetFrameLength();
}

//-----------------------------------------------------------------------------------------------------
void CSpiMT::GetSystemTime(void)
{
    GetSystemTime();
}

//-----------------------------------------------------------------------------------------------------
int8_t CSpiMT::FrameCheck(void)
{
    if (m_pxDevice -> m_nuiRxBuffByteCounter < MIN_MESSAGE_LENGTH)
    {
        return 0;
    }

    uint16_t uiCrc = ((static_cast<uint16_t>(m_puiRxBuffer[m_pxDevice -> m_nuiRxBuffByteCounter - 1]) << 8) |
                      (static_cast<uint16_t>(m_puiRxBuffer[m_pxDevice -> m_nuiRxBuffByteCounter - 2])));

    if (usCrc16(m_puiRxBuffer,
                (m_pxDevice -> m_nuiRxBuffByteCounter - CRC_LENGTH)) == uiCrc)
    {
        return 1;
    }
    else
    {
        return 0;
    }
}

//-----------------------------------------------------------------------------------------------------
int8_t CSpiMT::FrameCheck(uint8_t *puiSourse, uint16_t uiLength)
{
    if (uiLength < MIN_MESSAGE_LENGTH)
    {
        return 0;
    }

    uint16_t uiCrc = ((static_cast<uint16_t>(puiSourse[uiLength - 1]) << 8) |
                      (static_cast<uint16_t>(puiSourse[uiLength - 2])));
    uint16_t uiCrcTemp = usCrc16(puiSourse,
                                 (uiLength - CRC_LENGTH));
    if (usCrc16(puiSourse,
                (uiLength - CRC_LENGTH)) == uiCrc)
    {
        return 1;
    }
    else
    {
        return 0;
    }
}

//-----------------------------------------------------------------------------------------------------
void CSpiMT::Execution(void)
{
    int16_t iReceivedCounter;

    switch (m_uiFlowControl)
    {
    case IDDLE:
        break;

    case START:
        Reset();
        m_uiFlowControl = FSM_WAITING_MESSAGE;
        break;

    case WAITING_MESSAGE:
        if (Select())
        {
            iReceivedCounter =
                Exchange(CSpi::m_puiRxBuffer + m_uiMessageLength,
                         CSpi::m_puiTxBuffer + m_uiMessageLength,
                         MASTER_DATA_EXCHANGE_MAX_MESSAGE_LENGTH - m_uiMessageLength);

            if (iReceivedCounter > 0)
            {
                m_uiMessageLength += iReceivedCounter;

                if (SpiReply(CSpi::m_puiRxBuffer, CSpi::m_puiTxBuffer, m_uiMessageLength))
                {
                    m_uiFlowControl = FSM_DATA_EXCHANGE_END_WAITING;
                }
                else
                {
                    m_uiFlowControl = FSM_IDDLE;
                }
            }
            else if (iReceivedCounter == -1)
            {
                m_uiFlowControl = FSM_IDDLE;
            }


        }

        break;

    case DATA_EXCHANGE_END_WAITING:

        break;

    default:
        break;
    }




    int16_t iReceivedCounter;

    switch (m_uiFlowControl)
    {
    case IDDLE:
        break;

    case START:
        GetSystemTime();
        Reset();
        m_uiFlowControl = WAITING_MESSAGE;
        break;

    case WAITING_MESSAGE:
        if (Select())
        {
            iReceivedCounter = Receive(m_puiRxBuffer + m_uiMessageLength, MESSAGE_BAG_MAX_LENGTH - m_uiMessageLength);

            if (iReceivedCounter > 0)
            {
                m_uiMessageLength += iReceivedCounter;
                GetSystemTime();
                m_uiFlowControl = RECEIVE_MESSAGE;
            }
            else if (iReceivedCounter == -1)
            {
                m_uiFlowControl = START;
            }
        }

        if (ReceiveTimeIsOver())
        {
            m_uiFlowControl = START;
        }

        break;

    case RECEIVE_MESSAGE:
        if (Select())
        {
            iReceivedCounter = Receive(m_puiRxBuffer + m_uiMessageLength, MESSAGE_BAG_MAX_LENGTH - m_uiMessageLength);

            if (iReceivedCounter > 0)
            {
                m_uiMessageLength += iReceivedCounter;
                GetSystemTime();
            }
            else if (iReceivedCounter == -1)
            {
                m_uiFlowControl = START;
            }
        }

        if (FrameIsReceived())
        {
            if (FrameCheck(m_puiRxBuffer, m_uiMessageLength))
            {
                m_uiFlowControl = REPLY;
            }
            else
            {
                m_uiFlowControl = START;
            }
        }

        break;

    case REPLY:
        if (Reply(m_puiRxBuffer, m_puiTxBuffer, m_uiMessageLength))
        {
            CPlatform::TxLedOn();
            m_uiFlowControl = FRAME_TRANSMIT;
        }
        else
        {
            CPlatform::TxLedOff();
            m_uiFlowControl = START;
        }
        break;

    case FRAME_TRANSMIT:
        if (MessageIsSended())
        {
            CPlatform::TxLedOff();
            m_uiFlowControl = START;
        }

        break;

    default:
        break;
    }

}
