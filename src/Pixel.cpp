//-----------------------------------------------------------------------------------------------------
//  Source      : FileName.cpp
//  Created     : 01.06.2022
//  Author      : Alexandr Volvenkin
//  email       : aav-36@mail.ru
//  GitHub      : https://github.com/AlexandrVolvenkin
//-----------------------------------------------------------------------------------------------------
#include "Pixel.h"
#include "Dm163.h"

////-----------------------------------------------------------------------------------------------------
//CPixel::CPixel()
//{
//    //ctor
//}
//
////-----------------------------------------------------------------------------------------------------
//CPixel::~CPixel()
//{
//    //dtor
//}

//-----------------------------------------------------------------------------------------------------




////-----------------------------------------------------------------------------------------------------
//CPss48BoardPixel::CPss48BoardPixel()
//{
//    //ctor
//}
//
////-----------------------------------------------------------------------------------------------------
//CPss48BoardPixel::~CPss48BoardPixel()
//{
//    //dtor
//}

//-----------------------------------------------------------------------------------------------------
void CPss48BoardPixel::SetCorrection(TPixelColorData __farflash* pxPixelData)
{
    CDm163::SetBank0(pxPixelData -> uiRedData);
    CDm163::SetBank0(pxPixelData -> uiGreenData);
    CDm163::SetBank0(pxPixelData -> uiBlueData);
}

//-----------------------------------------------------------------------------------------------------
void CPss48BoardPixel::SetData(TPixelColorData __farflash* pxPixelData)
{
//    CDm163::SetBank1(pxPixelData -> uiRedData);
//    CDm163::SetBank1(pxPixelData -> uiGreenData);
//    CDm163::SetBank1(pxPixelData -> uiBlueData);

//    CDm163::SetBank0(63);
//    CDm163::SetBank1(255);
//
//    CDm163::SetBank0(63);
//    CDm163::SetBank1(255);
//
//    CDm163::SetBank0(63);
//    CDm163::SetBank1(255);

    CDm163::SetBank0(63);
    CDm163::SetBank1(pxPixelData -> uiBlueData);

    CDm163::SetBank0(63);
    CDm163::SetBank1(pxPixelData -> uiRedData);

    CDm163::SetBank0(63);
    CDm163::SetBank1(pxPixelData -> uiGreenData);
}

