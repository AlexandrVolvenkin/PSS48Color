//-----------------------------------------------------------------------------------------------------
//  Source      : FileName.cpp
//  Created     : 01.06.2022
//  Author      : Alexandr Volvenkin
//  email       : aav-36@mail.ru
//  GitHub      : https://github.com/AlexandrVolvenkin
//-----------------------------------------------------------------------------------------------------
#ifndef CDM134_H
#define CDM134_H

//-----------------------------------------------------------------------------------------------------
#include <stdint.h>

#include "BoardLamp.h"

//-----------------------------------------------------------------------------------------------------
class CDM134
{
public:
//    CDM134();
//    virtual ~CDM134();
    static void Init(void);
    static void Set(uint8_t* puiBoardLampsControl, uint8_t uiLampNumber);

protected:
private:
};

#endif // CDM134_H

//-----------------------------------------------------------------------------------------------------
#include <stdint.h>

//-----------------------------------------------------------------------------------------------------
