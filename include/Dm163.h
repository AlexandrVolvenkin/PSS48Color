#ifndef CDM163_H
#define CDM163_H
//-----------------------------------------------------------------------------------------------------
//  Source      : FileName.cpp
//  Created     : 01.06.2022
//  Author      : Alexandr Volvenkin
//  email       : aav-36@mail.ru
//  GitHub      : https://github.com/AlexandrVolvenkin
//-----------------------------------------------------------------------------------------------------

#include <stdint.h>

#include "Platform.h"
#include "BoardLamp.h"

//-----------------------------------------------------------------------------------------------------
class CDm163
{
public:
    enum
    {
        DM163_BANK0_BIT_QUANTITY = 6,
        DM163_BANK1_BIT_QUANTITY = 8,
    };

//        CDm163();
//        virtual ~CDm163();

////-----------------------------------------------------------------------------------------------------
//    void SetPinOutputSIN(void)
//    {
//        DDRF |=  BIT(DDF0);
//    };
//
//    void SetPinSIN(void)
//    {
//        PORTF |=  BIT(PORTF0);
//    };
//
//    void ClearPinSIN(void)
//    {
//        PORTF &=  ~BIT(PORTF0);
//    };
////-----------------------------------------------------------------------------------------------------
//
//
//
////-----------------------------------------------------------------------------------------------------
//    void SetPinOutputRST_B(void)
//    {
//        DDRF |=  BIT(DDF6);
//    };
//
//    void SetPinRST_B(void)
//    {
//        PORTF |=  BIT(PORTF6);
//    };
//
//    void ClearPinRST_B(void)
//    {
//        PORTF &=  ~BIT(PORTF6);
//    };
////-----------------------------------------------------------------------------------------------------
//
//
//
////-----------------------------------------------------------------------------------------------------
//    void SetPinOutputDCK(void)
//    {
//        DDRF |=  BIT(DDF1);
//    };
//
//    void SetPinDCK(void)
//    {
//        PORTF |=  BIT(PORTF1);
//    };
//
//    void ClearPinDCK(void)
//    {
//        PORTF &=  ~BIT(PORTF1);
//    };
////-----------------------------------------------------------------------------------------------------
//
//
//
////-----------------------------------------------------------------------------------------------------
//    void SetPinOutputLAT_B(void)
//    {
//        DDRF |=  BIT(DDF2);
//    };
//
//    void SetPinLAT_B(void)
//    {
//        PORTF |=  BIT(PORTF2);
//    };
//
//    void ClearPinLAT_B(void)
//    {
//        PORTF &=  ~BIT(PORTF2);
//    };
////-----------------------------------------------------------------------------------------------------
//
//
//
////-----------------------------------------------------------------------------------------------------
//    void SetPinOutputGCK(void)
//    {
//        DDRE |=  BIT(DDE3);
//    };
//
//    void SetPinGCK(void)
//    {
//        PORTE |=  BIT(PORTE3);
//    };
//
//    void ClearPinGCK(void)
//    {
//        PORTE &=  ~BIT(PORTE3);
//    };
////-----------------------------------------------------------------------------------------------------
//
//
//
////-----------------------------------------------------------------------------------------------------
//    void SetPinOutputSELBK(void)
//    {
//        DDRF |=  BIT(DDF4);
//    };
//
//    void SetPinSELBK(void)
//    {
//        PORTF |=  BIT(PORTF4);
//    };
//
//    void ClearPinSELBK(void)
//    {
//        PORTF &=  ~BIT(PORTF4);
//    };
////-----------------------------------------------------------------------------------------------------
//
//
//
////-----------------------------------------------------------------------------------------------------
//    void SetPinOutputEN_B(void)
//    {
//        DDRF |=  BIT(DDF5);
//    };
//
//    void SetPinEN_B(void)
//    {
//        PORTF |=  BIT(PORTF5);
//    };
//
//    void ClearPinEN_B(void)
//    {
//        PORTF &=  ~BIT(PORTF5);
//    };
////-----------------------------------------------------------------------------------------------------



//-----------------------------------------------------------------------------------------------------
#define TIMER_0_PRESCALLER 1
#define DM163_CONTROL_GCK_FREQUENCY_TIME ((F_CPU / (1000000 * 2 * TIMER_0_PRESCALLER)) - 1)

    static void SetFrequencyGCK(void)
    {
        CPins::SetPinOutputGCK();
        OCR0 = DM163_CONTROL_GCK_FREQUENCY_TIME;
        // Toggle OC0 on Compare Match, clkI/O/ (No prescaling), CTC
        TCCR0 = (1 << WGM01) | (1 << COM00) | (1 << CS00);
//        TCCR0 = (1 << WGM01) | (1 << COM00) | (1 << CS00) | (1 << CS01) | (1 << CS02);
    };

    static void StartFrequencyGCK(void)
    {
        OCR0 = DM163_CONTROL_GCK_FREQUENCY_TIME;
        // Toggle OC0 on Compare Match, clkI/O/ (No prescaling), CTC
        TCCR0 = (1 << WGM01) | (1 << COM00) | (1 << CS00);
    };

    static void StopFrequencyGCK(void)
    {
        TCCR0 = 0;
        CPins::ClearPinGCK();
    };
//
//    void SetFrequencyGCK(void)
//    {
//        CPins::SetPinOutputGCK();
//        TCCR3A = (1 << COM3A0);
//        TCCR3B = (1 << WGM32) | (1 << CS30);
//        OCR3A = DM163_CONTROL_GCK_FREQUENCY_TIME;
//        OCR3B = DM163_CONTROL_GCK_FREQUENCY_TIME;
//    };
//
//    void StartFrequencyGCK(void)
//    {
//        TCCR3A = (1 << COM3A0);
//        TCCR3B = (1 << WGM32) | (1 << CS30);
//    };
//
//    void StopFrequencyGCK(void)
//    {
//        TCCR3A = (0 << COM3A0);
//        TCCR3B = (0 << WGM32) | (0 << CS30);
//        CPins::ClearPinGCK();
//    };
//-----------------------------------------------------------------------------------------------------



//-----------------------------------------------------------------------------------------------------
    static void PulseDelayRST_B(void)
    {
        _NOP();
        _NOP();
    };

    static void PulseDelayDCK(void)
    {
        _NOP();
    };

    static void PulseDelayGCK(void)
    {
        _NOP();
    };

    static void PulseDelayLAT_B(void)
    {
        _NOP();
    };
//-----------------------------------------------------------------------------------------------------

    static void Init(void);
    static void Reset(void);
    static void SetBank0(uint8_t uiData);
    static void SetBank1(uint8_t uiData);

protected:

private:
};

#endif // CDM163_H
