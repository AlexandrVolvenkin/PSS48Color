//-----------------------------------------------------------------------------------------------------
//  Source      : FileName.cpp
//  Created     : 01.06.2022
//  Author      : Alexandr Volvenkin
//  email       : aav-36@mail.ru
//  GitHub      : https://github.com/AlexandrVolvenkin
//-----------------------------------------------------------------------------------------------------
#include "Dm163.h"
//#include "Platform.h"
#include "Pss21.h"
#include "LightBoard.h"

////-----------------------------------------------------------------------------------------------------
//CDm163::CDm163()
//{
//    //ctor
//}
//
//CDm163::~CDm163()
//{
//    //dtor
//}

//-----------------------------------------------------------------------------------------------------
void CDm163::Init(void)
{
    CPins::SetPinOutputSIN();
    CPins::ClearPinSIN();

    CPins::SetPinOutputRST_B();
    CPins::SetPinRST_B();

    CPins::SetPinOutputDCK();
    CPins::ClearPinDCK();

    CPins::SetPinOutputLAT_B();
    CPins::ClearPinLAT_B();

    CPins::SetPinOutputGCK();
    CPins::ClearPinGCK();

    CPins::SetPinOutputSELBK();
    CPins::ClearPinSELBK();

    CPins::SetPinOutputEN_B();
    CPins::SetPinEN_B();

    SetFrequencyGCK();
}

//-----------------------------------------------------------------------------------------------------
void CDm163::Reset(void)
{
    CPins::ClearPinRST_B();
    PulseDelayRST_B();
    CPins::SetPinRST_B();
}

//-----------------------------------------------------------------------------------------------------
void CDm163::SetBank0(uint8_t uiData)
{
    CPins::ClearPinSELBK();

    for (uint8_t i = 0;
            i < DM163_BANK0_BIT_QUANTITY;
            i++)
    {
        // передача начинается с 5 бита, старшим вперёд
        if (uiData & (0x20 >> i))
        {
            CPins::SetPinSIN();
        }
        else
        {
            CPins::ClearPinSIN();
        }

        CPins::SetPinDCK();
        PulseDelayDCK();
        CPins::ClearPinDCK();
    };

    CPins::ClearPinSELBK();
}

//-----------------------------------------------------------------------------------------------------
void CDm163::SetBank1(uint8_t uiData)
{
    CPins::ClearPinSELBK();

    for (uint8_t i = 0;
            i < DM163_BANK1_BIT_QUANTITY;
            i++)
    {
        // передача начинается с 8 бита, старшим вперёд
        if (uiData & (0x80 >> i))
        {
            CPins::SetPinSIN();
        }
        else
        {
            CPins::ClearPinSIN();
        }

        CPins::SetPinDCK();
        PulseDelayDCK();
        CPins::ClearPinDCK();
    };

    CPins::ClearPinSELBK();
}

