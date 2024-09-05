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
//    // Левая часть.    // Правая часть.
//    0, 0, 0, 0,    0, 0, 0, 0,
//    1, 1, 1, 1,    1, 1, 1, 1,
//    1, 1, 1, 1,    1, 1, 1, 1,
//    1, 1, 1, 1,    1, 1, 1, 1,
//    1, 1, 1, 1,    1, 1, 1, 1,
//    1, 1, 1, 1,    1, 1, 1, 1,

//    // Левая часть.    // Правая часть.
//    47, 33, 40, 32,    23, 9, 16, 8,
//    48, 34, 39, 31,    24, 10, 15, 7,
//    41, 35, 38, 39,    17, 11, 14, 6,
//    42, 36, 37, 29,    18, 12, 13, 5,
//    43, 46, 25, 28,    19, 22, 1, 4,
//    44, 45, 26, 27,    20, 21, 2, 3,

//    // Левая часть.    // Правая часть.
//    0,  0,  0,  0,     0,  0,  0,  0,
//    0,  0,  0,  0,     0,  0,  0,  0,
//    0,  0,  0,  0,     0,  0,  0,  0,
//    0,  0,  0,  0,     0,  0,  0,  0,
//    0,  0,  0,  0,     0,  0,  0,  0,
//    0,  0,  0,  0,     0,  0,  0,  0,

    // Левая часть.    // Правая часть.
    38,  46,  47,  39,    31,  23,  15,  7,
    5,   13,  21,  29,    30,  22,  14,  6,
    20,  28,  36,  44,    45,  37,  4,  12,
    34,  42,  43,  35,    27,  19,  11,  3,
    1,    9,  17,  25,    26,  18,  10,  2,
    16,  24,  32,  40,    41,  33,  0,  8,

//-----------------------------------------------------------------------------------------------------
//карта индекс в массиве - позиция лампы: 1 - 75 - xy x - 7, y - 5.
// 1 - первый элемент в массиве, адрес - 0.
//1 - 75
//2 -76
//3 86
//4-85
//5-84
//6-83
//7-82
//8-81
//
//9-61
//10-62
//11-63
//12-64
//13-74
//14-73
//15-72
//16-71
//
//17- 53
//18-54
//19-55
//20- 56
//21-66
//22-65
//23-51
//24-52
//
//25-35
//26-36
//27-46
//28-45
//29-44
//30- 43
//31--42
//32-41
//
//33-21
//34-22
//35-23
//36-24
//37-34
//38-33
//39-32
//40-31
//
//41-13
//42-14
//43-15
//44-16
//45-26
//46-25
//47-11
//48-12
////-----------------------------------------------------------------------------------------------------
//
//41-13
//42-14
//43-15
//44-16
//45-26
//46-25
//47-11
//48-12
//    // Левая часть.    // Правая часть.
//    38,  46,  47,  39,    31,  23,  15,  7,
//    5,   13,  21,  29,    30,  22,  14,  6,
//    20,  28,  36,  44,    45,  37,  4,  12,
//    34,  42,  43,  35,    27,  19,  11,  3,
//    1,    9,  17,  25,    26,  18,  10,  2,
//    16,  24,  32,  40,    41,  33,  0,  8,
//
//     1  | 2  | 3  | 4     5  | 6  | 7  | 8  |
//---------------------------------------------
//1  | 0  | 1  | 2  | 3     4  | 5  | 6  | 7  |
//---------------------------------------------
//2  | 8  | 9  | 10 | 11    12 | 13 | 14 | 15 |
//---------------------------------------------
//3  | 16 | 17 | 18 | 19    20 | 21 | 22 | 23 |
//---------------------------------------------
//4  | 24 | 25 | 26 | 27    28 | 29 | 30 | 31 |
//---------------------------------------------
//5  | 32 | 33 | 34 | 35    36 | 37 | 38 | 39 |
//---------------------------------------------
//6  | 40 | 41 | 42 | 43    44 | 45 | 46 | 47 |
//---------------------------------------------
};

// разведено на плате:
// b
// r
// g

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
    0,
    255,
    0,
};

__flash TPixelColorData CLightBoard::m_xPixelColorGreen =
{
    0,
    0,
    255,
};

__flash TPixelColorData CLightBoard::m_xPixelColorBlue =
{
    255,
    0,
    0,
};

__flash TPixelColorData CLightBoard::m_xPixelColorYellow =
{
    0,
    255,
    255,
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
//    return;
    CDm163::Reset();
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
//        if (pxAlarmWindowControl[i].GetActivityState())
        {
//        CPss21::m_aucRtuHoldingRegistersArray[1] = i;
//        CPss21::m_aucRtuHoldingRegistersArray[2] = m_auiLedMap[i];
//        CPss21::m_aucRtuHoldingRegistersArray[3] = pxAlarmWindowControl[i].GetAlarmType();
//            switch (pxAlarmWindowControl[i].GetAlarmType())
//            switch (pxAlarmWindowControl[m_auiLedMap[i]].GetAlarmType())
            switch (pxAlarmWindowControl[m_auiLedMap[i]].GetAlarmColor())
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

            case ERROR:
                CPss48BoardPixel::SetData(&m_xPixelColorWhite);
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

    CPins::SetPinLAT_B();
    CDm163::PulseDelayLAT_B();
    CPins::ClearPinLAT_B();
    CPins::ClearPinEN_B();
}


//        CPss21::m_aucRtuHoldingRegistersArray[0] = pxAlarmWindowControl[i].GetActivityState();
