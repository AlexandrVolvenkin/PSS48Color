//-----------------------------------------------------------------------------------------------------
//  Source      : FileName.cpp
//  Created     : 01.06.2022
//  Author      : Alexandr Volvenkin
//  email       : aav-36@mail.ru
//  GitHub      : https://github.com/AlexandrVolvenkin
//-----------------------------------------------------------------------------------------------------
#include "LightBoard.h"
#include "Platform.h"
#include "Pss21.h"
#include "Dm163.h"

//-------------------------------------------------------------------------------------------------
// Массив управления драйверами светодиодов на панели.
uint8_t CLightBoard::m_auiBoardLampsControl[m_uiBoardLedNumber];
// Карта соответствия индекса окна на панели и индекса состояния светодиода в массиве управления.
__flash uint8_t CLightBoard::m_auiLedMap[] =
{
    // Левая часть.    // Правая часть.
    0, 0, 0, 0,    0, 0, 0, 0,
    1, 1, 1, 1,    1, 1, 1, 1,
    1, 1, 1, 1,    1, 1, 1, 1,
    1, 1, 1, 1,    1, 1, 1, 1,
    1, 1, 1, 1,    1, 1, 1, 1,
    1, 1, 1, 1,    1, 1, 1, 1,
};

__flash TPixelColorData CLightBoard::m_xPixelColorCorrectionData =
{
    63,
    63,
    63,
};

__flash TPixelColorData CLightBoard::m_xPixelColorBlack =
{
    0,
    0,
    0,
};

__flash TPixelColorData CLightBoard::m_xPixelColorRed =
{
    255,
    0,
    0,
};

__flash TPixelColorData CLightBoard::m_xPixelColorGreen =
{
    0,
    255,
    0,
};

__flash TPixelColorData CLightBoard::m_xPixelColorBlue =
{
    0,
    0,
    255,
};

__flash TPixelColorData CLightBoard::m_xPixelColorYellow =
{
    255,
    255,
    0,
};

__flash TPixelColorData CLightBoard::m_xPixelColorWhite =
{
    255,
    255,
    255,
};

////-----------------------------------------------------------------------------------------------------
//CLightBoard::CLightBoard()
//{
////    Init();
//}
//
////-----------------------------------------------------------------------------------------------------
//CLightBoard::~CLightBoard()
//{
//
//}

//-----------------------------------------------------------------------------------------------------
void CLightBoard::Init(CAlarmWindow* pxAlarmWindowControl)
{
//    // Включим панель.
//    CPlatform::LedDriverEnable1On();
//    CPlatform::LedDriverEnable2On();
//    CPlatform::LedDriverEnable3On();
//    CPlatform::LedDriverEnable4On();
//    CPlatform::LedDriverEnable1SetPinOutput();
//    CPlatform::LedDriverEnable2SetPinOutput();
//    CPlatform::LedDriverEnable3SetPinOutput();
//    CPlatform::LedDriverEnable4SetPinOutput();

//    CDM134::Init();
    CDm163::Init();
}

////-----------------------------------------------------------------------------------------------------
//void CLightBoard::Set(CAlarmWindow* pxAlarmWindowControl)
//{
//    for (uint8_t i = 0;
//            i < ALARM_WINDOWS_NUMBER;
//            i++)
//    {
//        // Окно сигнализации активно?
//        if (pxAlarmWindowControl[i].GetActivityState())
//        {
//            m_auiBoardLampsControl[m_auiLedMap[i]] = 1;
//        }
//        else
//        {
//            m_auiBoardLampsControl[m_auiLedMap[i]] = 0;
//        }
//
////        CPss21::m_aucRtuHoldingRegistersArray[i] = pxAlarmWindowControl[i].GetActivityState();
//    };
//
//    // Зажжём активные светодиоды.
//    CDM134::Set(m_auiBoardLampsControl, m_uiBoardLedNumber);
//}

//-----------------------------------------------------------------------------------------------------
void CLightBoard::Set(CAlarmWindow* pxAlarmWindowControl)
{
    // передадим данные коррекции - банк0
    for (uint8_t i = 0;
            i < BOARD_LED_NUMBER;
            i++)
    {
        CPss48BoardPixel::SetCorrection(&m_xPixelColorCorrectionData);
    };

    for (uint8_t i = 0;
            i < BOARD_LED_NUMBER;
            i++)
    {
        // Окно сигнализации активно?
        if (pxAlarmWindowControl[m_auiLedMap[i]].GetActivityState())
        {
            switch (pxAlarmWindowControl[m_auiLedMap[i]].GetAlarmType())
            {
            case INDICATION:
                CPss48BoardPixel::SetData(&m_xPixelColorGreen);
                break;

            case PREVENTIVE:
                CPss48BoardPixel::SetData(&m_xPixelColorYellow);
                break;

            case EMERGENCY:
                CPss48BoardPixel::SetData(&m_xPixelColorRed);
                break;

            default:
                CPss48BoardPixel::SetData(&m_xPixelColorBlack);
                break;
            }

        }
        else
        {
            CPss48BoardPixel::SetData(&m_xPixelColorBlack);
        }
    };
}


//        CPss21::m_aucRtuHoldingRegistersArray[0] = pxAlarmWindowControl[i].GetActivityState();
