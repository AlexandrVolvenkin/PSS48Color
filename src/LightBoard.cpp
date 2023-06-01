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
// ������ ���������� ���������� ����������� �� ������.
uint8_t CLightBoard::m_auiBoardLampsControl[m_uiBoardLedNumber];
// ����� ������������ ������� ���� �� ������ � ������� ��������� ���������� � ������� ����������.
__flash uint8_t CLightBoard::m_auiLedMap[] =
{
    // ����� �����.    // ������ �����.
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
//    // ������� ������.
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
//        // ���� ������������ �������?
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
//    // ����� �������� ����������.
//    CDM134::Set(m_auiBoardLampsControl, m_uiBoardLedNumber);
//}

//-----------------------------------------------------------------------------------------------------
void CLightBoard::Set(CAlarmWindow* pxAlarmWindowControl)
{
    CDm163::Reset();
//    // ��������� ������ ��������� - ����0
//    for (uint8_t i = 0;
//            i < BOARD_LED_NUMBER;
//            i++)
//    {
//        CPss48BoardPixel::SetCorrection(&m_xPixelColorCorrectionData);
//    };

    for (uint8_t i = 0;
            i < BOARD_LED_NUMBER;
            i++)
    {
        // ���� ������������ �������?
//        if (pxAlarmWindowControl[m_auiLedMap[i]].GetActivityState())
        if (pxAlarmWindowControl[i].GetActivityState())
        {
        CPss21::m_aucRtuHoldingRegistersArray[i + 4] = i + 1;
        CPss21::m_aucRtuHoldingRegistersArray[i + 1] = pxAlarmWindowControl[i].GetAlarmType();
            switch (pxAlarmWindowControl[i].GetAlarmType())
//            switch (pxAlarmWindowControl[m_auiLedMap[i]].GetAlarmType())
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
//                CPss48BoardPixel::SetData(&m_xPixelColorBlack);
                CPss48BoardPixel::SetData(&m_xPixelColorYellow);
                break;
            }

        }
        else
        {
            CPss48BoardPixel::SetData(&m_xPixelColorBlack);
        }

////        CPss48BoardPixel::SetCorrection(&m_xPixelColorCorrectionData);
//            CPss48BoardPixel::SetData(&m_xPixelColorWhite);
    };


    CPins::SetPinLAT_B();
    CDm163::PulseDelayLAT_B();
    CPins::ClearPinLAT_B();
    CPins::ClearPinEN_B();

//    SET_PIN_DM163_CONTROL_LAT_B();
//    LAT_B_PULSE_DURATION_DELAY();
//    CLEAR_PIN_DM163_CONTROL_LAT_B();
//    CLEAR_PIN_DM163_CONTROL_EN_B();
}


//        CPss21::m_aucRtuHoldingRegistersArray[0] = pxAlarmWindowControl[i].GetActivityState();
