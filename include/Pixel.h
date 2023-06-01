#ifndef CPIXEL_H
#define CPIXEL_H
//-----------------------------------------------------------------------------------------------------
//  Source      : FileName.cpp
//  Created     : 01.06.2022
//  Author      : Alexandr Volvenkin
//  email       : aav-36@mail.ru
//  GitHub      : https://github.com/AlexandrVolvenkin
//-----------------------------------------------------------------------------------------------------

#include <stdint.h>

#include "Platform.h"

//-----------------------------------------------------------------------------------------------------
struct TPixelColorData
{
    uint8_t uiRedData;
    uint8_t uiGreenData;
    uint8_t uiBlueData;
};

////-----------------------------------------------------------------------------------------------------
//class CPixel
//{
//    public:
////        CPixel();
////        virtual ~CPixel();
//
//    protected:
//
//    private:
//        uint8_t m_uiRedData;
//        uint8_t m_uiGreenData;
//        uint8_t m_uiBlueData;
//};
//
////-----------------------------------------------------------------------------------------------------




//-----------------------------------------------------------------------------------------------------
class CPss48BoardPixel
{
public:
//        CPss48BoardPixel();
//        virtual ~CPss48BoardPixel();
    static void SetCorrection(TPixelColorData __farflash* pxPixelData);
    static void SetData(TPixelColorData __farflash* pxPixelData);

protected:

private:
//    static uint8_t m_uiRedData;
//    static uint8_t m_uiGreenData;
//    static uint8_t m_uiBlueData;
};

//-----------------------------------------------------------------------------------------------------
#endif // CPIXEL_H
