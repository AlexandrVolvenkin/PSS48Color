//-----------------------------------------------------------------------------------------------------
//  Source      : FileName.cpp
//  Created     : 01.06.2022
//  Author      : Alexandr Volvenkin
//  email       : aav-36@mail.ru
//  GitHub      : https://github.com/AlexandrVolvenkin
//-----------------------------------------------------------------------------------------------------
#ifndef CLIGHTBOARD_H
#define CLIGHTBOARD_H

#include <stdint.h>
#include "AlarmWindow.h"
#include "DM134.h"
#include "Pixel.h"

// class CPss48BoardPixel;

//-----------------------------------------------------------------------------------------------------
class CLightBoard
{
public:
    enum
    {
        BOARD_LED_NUMBER = 48,
    };
//    CLightBoard();
//    virtual ~CLightBoard();
    static void Init(CAlarmWindow* pxAlarmWindowControl);
    static void Set(CAlarmWindow* pxAlarmWindowControl);

protected:
private:
    // Количество светодиодов управляемых одной микросхемой-драйвером.
    static const uint8_t m_uiLedDriverLedNumber = 16;
    // Количество микросхем-драйверов светодиодов на панели прибора.
    static const uint8_t m_uiLedDriversNumber = 4;
    static const uint8_t m_uiBoardLedNumber = m_uiLedDriverLedNumber * m_uiLedDriversNumber;

    // Массив управления драйверами светодиодов на панели.
    static uint8_t m_auiBoardLampsControl[m_uiBoardLedNumber];
    // Карта соответствия индекса окна на панели и индекса состояния светодиода в массиве управления.
    static __flash uint8_t m_auiLedMap[];

    static __flash TPixelColorData m_xPixelColorCorrectionData;
    static __flash TPixelColorData m_xPixelColorBlack;
    static __flash TPixelColorData m_xPixelColorRed;
    static __flash TPixelColorData m_xPixelColorGreen;
    static __flash TPixelColorData m_xPixelColorBlue;
    static __flash TPixelColorData m_xPixelColorYellow;
    static __flash TPixelColorData m_xPixelColorWhite;
};

#endif // CLIGHTBOARD_H
