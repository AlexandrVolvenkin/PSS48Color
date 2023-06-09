//-----------------------------------------------------------------------------------------------------
//  Sourse      : FileName.cpp
//  Created     : 01.06.2022
//  Author      : Alexandr Volvenkin
//  email       : aav-36@mail.ru
//  GitHub      : https://github.com/AlexandrVolvenkin
//-----------------------------------------------------------------------------------------------------
// SerialMT - serial multi trans.
//-----------------------------------------------------------------------------------------------------
#ifndef UARTMT_H_INCLUDED
#define UARTMT_H_INCLUDED

#include "Task.h"
#include "SerialMT.h"
#include "Platform.h"

//-----------------------------------------------------------------------------------------------------
class CUartMT : virtual public CSerialMT, virtual public CTask
{
public:

    enum
    {
        IDDLE  = 0,
        START,
        WAITING_MESSAGE,
        RECEIVE_MESSAGE,
        RECEIVE_CONFIRMATION,
        REPLY,
        FRAME_TRANSMIT,
        FRAME_TRANSMIT_END_WAITING,
        RESTART,
    };

    enum
    {
        DEFAULT_BAUD_RATE = 9600UL,
    };

//-----------------------------------------------------------------------------------------------------
    CUartMT();
    virtual ~CUartMT();
    void Init(void);
    void Reset(void);
    int8_t ReceiveEnable(void);
    int8_t ReceiveDisable(void);
//    int16_t ResponseBasis(uint8_t , uint8_t , uint8_t * );
    int16_t Tail(uint8_t * , uint16_t );
    int16_t Send(uint8_t * , uint16_t );
    uint8_t MessageIsSended(void);
    int16_t Receive(void);
    uint8_t Select(void);
    int16_t Receive(uint8_t * , uint16_t );
    uint8_t FrameIsReceived(void);
    uint8_t ReceiveTimeIsOver(void);
    void GetSystemTime(void);
    uint16_t GetFrameLength(void);
    int8_t FrameCheck(void);
    int8_t FrameCheck(uint8_t * , uint16_t );

    void Execution(void);
//    void Fsm(void);

//-----------------------------------------------------------------------------------------------------
protected:
private:
    CUart* m_pxDevice;
    /* Bauds: 9600, 19200, 57600, 115200, etc */
    uint32_t m_uiBaudRate;
    /* Data bit */
    uint8_t m_uiDataBit;
    /* Stop bit */
    uint8_t m_uiStopBit;
    /* Parity: 'N', 'O', 'E' */
    char m_cParity;
    // ������� �� ���������� ���������� ����� 3.5 ����.
    const static uint8_t m_uiGuardTimeout =  ((((1000000UL / DEFAULT_BAUD_RATE) * 8UL * 4UL) / 1000UL) + 1);
    // ������� �� ���������� ���������.
    const static uint8_t m_uiReceiveTimeout = 15000;
    // ������� �� ���������� �������������.
    const static uint8_t m_uiConfirmationTimeout = 500;
};

//-----------------------------------------------------------------------------------------------------
#endif // UARTMT_H_INCLUDED
