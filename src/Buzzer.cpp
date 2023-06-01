//-----------------------------------------------------------------------------------------------------
//  Source      : FileName.cpp
//  Created     : 01.06.2022
//  Author      : Alexandr Volvenkin
//  email       : aav-36@mail.ru
//  GitHub      : https://github.com/AlexandrVolvenkin
//-----------------------------------------------------------------------------------------------------
#include "Buzzer.h"
#include "Platform.h"

//-----------------------------------------------------------------------------------------------------
CBuzzer::CBuzzer()
{

}

//-----------------------------------------------------------------------------------------------------
CBuzzer::~CBuzzer()
{

}

//-----------------------------------------------------------------------------------------------------
void CBuzzer::Init(void)
{
    CPlatform::BuzzerClockAOff();
    CPlatform::BuzzerClockASetPinOutput();
    CPlatform::BuzzerClockBOff();
    CPlatform::BuzzerClockBSetPinOutput();
}

//-----------------------------------------------------------------------------------------------------
void CBuzzer::LowOn(void)
{
    // ������� ����������������� ������������(760��).
    CPlatform::BuzzerTimerFrequencySet(760);
    CPlatform::BuzzerClockAOn();
    CPlatform::BuzzerClockBOn();
}

//-----------------------------------------------------------------------------------------------------
void CBuzzer::HighOn(void)
{
    // ������� ��������� ������������(3210��).
    CPlatform::BuzzerTimerFrequencySet(3210);
    CPlatform::BuzzerClockAOn();
    CPlatform::BuzzerClockBOn();
}

//-----------------------------------------------------------------------------------------------------
void CBuzzer::Off(void)
{
    CPlatform::BuzzerClockAOff();
    CPlatform::BuzzerClockBOff();
    CPlatform::BuzzerTimerStop();
}

