//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
#include <vcl.h>                        // Для отладки
#pragma hdrstop

#include "TModbus.h"
#include "MainForm.h"                   // Для отладки
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
// Конструктор класса TModbus
TModbus::TModbus(void):Handle(INVALID_HANDLE_VALUE),Timeout(1000),ERRCode(0),
    TotalTimeout(1500),Buffer(NULL) { }
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
// Деструктор класса TModbus
TModbus::~TModbus()
{
    TModbus::Close();
}
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
// Открытие порта и настройка параметров связи
HANDLE TModbus::Open(const char *Port,int BaudRate,int StopBits,int Parity,bool UseRTS,int BuffSize)
{
// Закрываем предыдущий открытый коммуникационный порт
    if(Handle != INVALID_HANDLE_VALUE) Close();

// Открытие коммуникационного порта
    Handle = CreateFile(Port,GENERIC_READ|GENERIC_WRITE,0,NULL, //FILE_SHARE_READ|FILE_SHARE_WRITE,
                        OPEN_EXISTING,FILE_ATTRIBUTE_NORMAL|FILE_FLAG_OVERLAPPED,NULL);

    if(Handle == INVALID_HANDLE_VALUE) return Handle;

// Настройка параметров коммуникационного порта
    DCB    Param = {0};

    GetCommState(Handle,&Param);

    Param.DCBlength   = sizeof(DCB);
    Param.BaudRate    = BaudRate;
    Param.StopBits    = StopBits;
    Param.Parity      = Parity;
    Param.fParity     = (Parity != NOPARITY)? true : false;

    Param.ByteSize    = 8;
    Param.fBinary     = true;
    Param.fDtrControl = DTR_CONTROL_DISABLE;
    Param.fRtsControl = RTS_CONTROL_DISABLE;// (UseRTS)? RTS_CONTROL_TOGGLE : RTS_CONTROL_DISABLE;

    Param.fAbortOnError = false;

    if(!SetCommState(Handle,&Param))
    {
        CloseHandle(Handle);
        return INVALID_HANDLE_VALUE;
    }

// Настройка таймаутов коммуникационного порта
    COMMTIMEOUTS Timeouts = {0};

    memset(&Timeouts, 0x00, sizeof(Timeouts));

    int   CharBits     = 10 + ((Param.fParity)? 1:0) + ((StopBits == TWOSTOPBITS)? 1:0);
    float TimeChar     = 1000.0 * CharBits / BaudRate;
// float TimeInterval = (3.5+1)*TimeChar;

    Timeouts.ReadIntervalTimeout        = 0;	// 10*TimeInterval;
    Timeouts.ReadTotalTimeoutMultiplier = 10;	//TimeChar;
    Timeouts.ReadTotalTimeoutConstant   = TotalTimeout;

    if(!SetCommTimeouts(Handle,&Timeouts))
    {
        CloseHandle(Handle);
        return INVALID_HANDLE_VALUE;
    }

// Сброс и очистка коммуникационного порта
    PurgeComm(Handle,PURGE_TXABORT|PURGE_RXABORT|PURGE_TXCLEAR|PURGE_RXCLEAR);

// Выделение памяти под буфер I/0
    Buffer = new char[BuffSize];

    FMain->L4->Caption = Port;
    return Handle;
}
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
// Закрытие коммуникационного порта
void TModbus::Close(void)
{
    PurgeComm(Handle,PURGE_TXABORT|PURGE_RXABORT|PURGE_TXCLEAR|PURGE_RXCLEAR);
    CloseHandle(Handle);
    Handle = INVALID_HANDLE_VALUE;

    delete [] Buffer;
    Buffer = NULL;
}
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
// Вычисление CRC-16, зеркальный алгоритм, полином 0xA001, табличная реализация
unsigned short TModbus::CRC16Fast(const char *Data,int Lenght)
{
    union for_reorder
    {
        unsigned char  reorder[2];
        unsigned short accumulator;
    } un_order,order;

    int index;
    un_order.accumulator = 0xFFFF;

    for(int i = 0; i < Lenght; i++)
    {
        un_order.reorder[0] ^= Data[i];
        index = un_order.reorder[0];

        un_order.reorder[0] = un_order.reorder[1];
        un_order.reorder[1] = 0;

        un_order.accumulator ^= TModbus::CRCTable[index];
    }

    order.reorder[0]=un_order.reorder[1];
    order.reorder[1]=un_order.reorder[0];
    return order.accumulator;
}
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
// Посылка пакета данных
bool TModbus::Write(const void *Data,int Lenght)
{
    OVERLAPPED Overlapped = {0};
    unsigned long Written;
    bool Result;

    int    nn = ERROR_IO_PENDING;

    nn = WAIT_OBJECT_0;
    nn = ERROR_IO_INCOMPLETE;
    nn = 0;

    PurgeComm(Handle,PURGE_TXABORT|PURGE_RXABORT|PURGE_TXCLEAR|PURGE_RXCLEAR);

    Overlapped.hEvent = CreateEvent(NULL,false,false,NULL);
    Result = WriteFile(Handle,Data,Lenght,&Written,&Overlapped);

    if(!Result)
    {
        if((nn = GetLastError()) == ERROR_IO_PENDING)
            if((nn = WaitForSingleObject(Overlapped.hEvent,INFINITE)) == WAIT_OBJECT_0)
                if((nn = GetOverlappedResult(Handle,&Overlapped,&Written,false))) Result = true;
    }
    else
    {
        Result = true;
    }

// nn = GetLastError();
    if((int)Written != Lenght)
    {
        FMain->BLabel->Caption = "Write(...): Written != Lenght";
        Result = false;
    };

    CloseHandle(Overlapped.hEvent);

    FMain->L4->Caption = Written;
    return Result;
}
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
// Прием пакета данных
bool TModbus::Read(void *Data,int Lenght,int &Readed,int Timeout)
{
    OVERLAPPED Overlapped = {0};
    unsigned long Read;
    bool Result;

    Overlapped.hEvent = CreateEvent(NULL,false,false,NULL);
    Result = ReadFile(Handle,Data,Lenght,&Read,&Overlapped);

    if(!Result)
    {
        if(GetLastError() == ERROR_IO_PENDING)
            if(WaitForSingleObject(Overlapped.hEvent,Timeout) == WAIT_OBJECT_0)
                if(GetOverlappedResult(Handle,&Overlapped,&Read,false)) Result = true;

//  GetOverlappedResult(Handle,&Overlapped,&Read,false);
    }
    else
    {
        Result = true;
    }

    CloseHandle(Overlapped.hEvent);
    Readed = Read;

    FMain->L4->Caption = Read;
    return Result;
}
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
// Смена мест старшего и младшего байт в слове
unsigned short TModbus::SwapBytes(unsigned short Data)
{
    return (Data<<8)|(Data>>8);
}
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
// Заполнение буфера заданным символом
void TModbus::FillBuff(char Fill,int Lenght,int Begin)
{
    for(int i = Begin; i < Lenght; i++) Buffer[i] = Fill;
}
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
// Установка пользовательского размера буфера I/O
char *TModbus::SetBuffer(int BuffSize)
{
    if(Buffer != NULL) delete [] Buffer;
    return  (Buffer = new char[BuffSize]);
}
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
// Запрос на изменение состояния логической ячейки (Запрос 05/0x05)
bool TModbus::ForceMultipleCoil(int NetAddr,short Address,bool Value)
{
    int  Readed,Lenght = 6;

    Buffer[0] = NetAddr&0x00FF;
    Buffer[1] = 0x05;

    Buffer[2] = Address>>8;
    Buffer[3] = Address;

    if(Value)
    {
        Buffer[4]=0xFF;
        Buffer[5]=0x00;
    }
    else
    {
        Buffer[4] = Buffer[5]= 0x00;
    }

// Передача запроса
    unsigned short CRC = CRC16Fast(Buffer,Lenght);
    Buffer[Lenght]     = CRC>>8;
    Buffer[Lenght+1]   = CRC;

    if(!Write(Buffer,Lenght+2))
    {
        FMain->L4->Caption = "Сбой посылки";
        return false;
    }

    if(!Read(Buffer,Lenght = 8,Readed,Timeout))
    {
        FMain->L4->Caption = "Сбой приема";
        return false;
    }
    else
    {
        FMain->L4->Caption = "CRC:"+IntToStr((int)CRC16Fast(Buffer,Readed))+"  Readed:"+IntToStr(Readed);
    }
    return true;
}
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
// Запрос на обновление Firmware (Запрос 100/0x64)
bool TModbus::FirmwareUpdate(int NetAddr,short Count,void *Data)
{
    int  i,Readed,Lenght = Count+4;

    Buffer[0] = NetAddr&0x00FF;
    Buffer[1] = 0x64;

    Buffer[2] = Count>>8;
    Buffer[3] = Count;

    for(i=0; i<Count; i++) Buffer[4+i] = ((char *)Data)[i];

// Передача запроса
    unsigned short CRC = CRC16Fast(Buffer,Lenght);
    Buffer[Lenght]     = CRC>>8;
    Buffer[Lenght+1]   = CRC;

// Сброс и очистка коммуникационного порта
//PurgeComm(Handle,PURGE_TXABORT|PURGE_RXABORT|PURGE_TXCLEAR|PURGE_RXCLEAR);

    if(!Write(Buffer,Lenght+2))
    {
        FMain->L4->Caption = "Сбой посылки";
        return false;
    }


    if(Count == 0) return true;    // При Count == 0 запрос без ответа

    // Принимаем ответ фиксированной длинны
    if(!Read(Buffer,Lenght = 10,Readed,Timeout))
    {
        FMain->L4->Caption = "Сбой приема";
        return false;
    }

//  Lenght = Buff[3]+2;                   // Вычисляем истинную длинну запроса
//  if(!Read(Buff+4,Lenght,Readed,Timeout)){ FMain->L4->Caption = "Сбой приема"; return false; }


    CRC = CRC16Fast(Buffer,Readed); //+4);
    FMain->L4->Caption = "CRC: "+IntToStr(CRC)+" / Len: "+IntToStr(Lenght)+" / Read: "+IntToStr(Readed)+
                         " / Cod: "+IntToHex((Buffer[4]&0xFF),2) + " / Buff[3]: "+IntToStr((int)Buffer[3]);

    Application->ProcessMessages();

    if(CRC != 0) return false;
    else return true;
}
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
// Запрос на чтение идентификационной информации (Запрос 43/0x2B)
bool TModbus::ReadDeviceIdentification(int NetAddr,short ReadDevID,short ObjectID,int ReadLen)
{
    int  i,Readed,Lenght = 5,All,NumberObject;
    unsigned char Buffer[1024] = {0};


    Buffer[0] = NetAddr&0x00FF;
    Buffer[1] = 0x2B;
    Buffer[2] = 0x0E;

    Buffer[3] = ReadDevID;
    Buffer[4] = ObjectID;

    unsigned short CRC = CRC16Fast(Buffer,Lenght);
    Buffer[Lenght]     = CRC>>8;
    Buffer[Lenght+1]   = CRC;

    if(!Write(Buffer,Lenght+2))
    {
        FMain->L4->Caption = "Сбой посылки";
        return false;
    }


// Читаем ответное сообщение
    if(!Read(Buffer,ReadLen,Readed,Timeout))
    {
        FMain->L4->Caption = "Сбой приема";
        return false;
    }

// CRC = CRC16Fast(Buffer,Readed);
// FMain->L4->Caption = "CRC: "+IntToStr(CRC);

    return true;
}
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
// Чтение типа ведомого устройства
bool   TModbus::ReportSlaveID(int NetAddr,int ReadLen)
{
    int  i,Readed,Lenght = 2;

    Buffer[0] = NetAddr&0x00FF;
    Buffer[1] = 0x11;

    unsigned short CRC = CRC16Fast(Buffer,Lenght);
    Buffer[Lenght]     = CRC>>8;
    Buffer[Lenght+1]   = CRC;

    if(!Write(Buffer,Lenght+2))
    {
        FMain->L4->Caption = "Сбой посылки";
        return false;
    }

// Читаем ответное сообщение
    if(!Read(Buffer,ReadLen,Readed,Timeout))
    {
        FMain->L4->Caption = "Сбой приема";
        return false;
    }

    CRC = CRC16Fast(Buffer,Readed);
    FMain->L4->Caption = "CRC: "+IntToStr(CRC);

    return true;

}
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
// Операции с блоком рабочих настроек ПСС
bool   TModbus::RWPPSWorkData(int NetAddr,int Func,void *Data)
{
    int  i,Readed,ReadLen,Lenght = 2;
    bool ReqRead = true;

    Buffer[0] = NetAddr&0x00FF;
    Buffer[1] = 0x46;

    if(Func == 0x02)
    {
        Buffer[2] = 0x00;
        Buffer[3] = Lenght = 33;
        Buffer[4] = Func;

        for(i=0; i<32; i++) Buffer[5+i] = ((char *)Data)[i];

        Lenght += 4;
        ReadLen = 5;
    }

    if(Func == 0x01)
    {
        Buffer[2] = 0x00;
        Buffer[3] = Lenght = 1;
        Buffer[4] = Func;

        Lenght += 4;
        ReadLen = 39;
    }

    if(Func == 0x03)
    {
        Buffer[2] = 0x00;
        Buffer[3] = Lenght = 1;
        Buffer[4] = Func;

        Lenght += 4;
//  ReadLen = 5;
        ReqRead = false;
    }


    unsigned short CRC = CRC16Fast(Buffer,Lenght);
    Buffer[Lenght]     = CRC>>8;
    Buffer[Lenght+1]   = CRC;

    if(!Write(Buffer,Lenght+2))
    {
        FMain->L4->Caption = "Сбой посылки";
        return false;
    }

    if(ReqRead)
    {
        if(!Read(Buffer,ReadLen,Readed,1000))
        {
            FMain->L4->Caption = "Сбой приема";
            return false;
        }

        CRC = CRC16Fast(Buffer,Readed);
        FMain->L4->Caption = "CRC: "+IntToStr(CRC);
    }

    return true;
}
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
// Проверка завершения операции программирования
bool   TModbus::PollProgram(int NetAddr)
{
    int  i,Readed,Lenght = 2;

    Buffer[0] = NetAddr & 0x00FF;
    Buffer[1] = 0x0E;

    unsigned short CRC = CRC16Fast(Buffer,Lenght);
    Buffer[Lenght]     = CRC>>8;
    Buffer[Lenght+1]   = CRC;

    if(!Write(Buffer,Lenght+2))
    {
        FMain->CLabel->Caption = "PollProgram Сбой посылки";
        return false;
    }

// Читаем ответное сообщение
    if(!Read(Buffer,4,Readed,Timeout))
    {
        FMain->CLabel->Caption = "PollProgram Сбой приема 1";
        return false;
    }

    if((Buffer[1]&0x80))
    {
        int AddRead;
        if(!Read(Buffer+5,1,AddRead,Timeout))
        {
            FMain->CLabel->Caption = "PollProgram Сбой приема 2";
            return false;
        }

        Readed += AddRead;
    }

    CRC = CRC16Fast(Buffer,Readed);

    if(CRC != 0)
    {
        FMain->CLabel->Caption = "PollProgram CRC Fail";
    };

    return true;
}
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
bool TModbus::ReadFWVersion(int NetAddr)
{
    int  i,Readed,Lenght = 5;

    Buffer[0] = NetAddr & 0xFF;
    Buffer[1] = 0x46;
    Buffer[2] = 0x00;
    Buffer[3] = 0x01;
    Buffer[4] = 0x00;

    unsigned short CRC = CRC16Fast(Buffer,Lenght);
    Buffer[Lenght]     = CRC >> 8;
    Buffer[Lenght+1]   = CRC;

    if(!Write(Buffer,Lenght+2))
    {
        FMain->L4->Caption = "Сбой посылки";
        return false;
    }
    if(!Read(Buffer,10,Readed,Timeout))
    {
        FMain->L4->Caption = "Сбой приема";
        return false;
    }

    if((CRC = CRC16Fast(Buffer,Readed)) != 0x0000)
    {
        Beep();
        FMain->L4->Caption = "CRC: "+IntToStr(CRC);
        return false;
    }

    return true;
};
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
bool TModbus::RestartDevice(int NetAddr)
{
    int Readed,Lenght = 5;

    Buffer[0] = NetAddr & 0xFF;
    Buffer[1] = 0x46;
    Buffer[2] = 0x00;
    Buffer[3] = 0x01;
    Buffer[4] = 0x03;

    unsigned short CRC = CRC16Fast(Buffer,Lenght);
    Buffer[Lenght]     = CRC>>8;
    Buffer[Lenght+1]   = CRC;

    if(!Write(Buffer,Lenght+2))
    {
        FMain->L4->Caption = "Сбой посылки";
        return false;
    }

    Sleep(10);

    if(!Read(Buffer,5,Readed,Timeout))
    {
        FMain->L4->Caption = "Сбой приема";
        return false;
    }

    return true;
};
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
bool TModbus::ReadBlock(int NetAddr,int Block)
{
    int Readed,Lenght = 6;

    int SizeBlock[10] =
    {
        20,7,8,64,32,24,64,81,81,81			// Размеры блоков данных ПСС-07
    };
    /*
     int SizeBlock[10] =
     {
      20,7,2*8,2*64,2*32,24,2*64,81,81,81			// Размеры блоков данных ПСС-07 WIDE
     };
      */

    Buffer[0] = NetAddr & 0xFF;
    Buffer[1] = 0x46;
    Buffer[2] = 0x00;
    Buffer[3] = 0x02;
    Buffer[4] = 0x01;
    Buffer[5] = Block & 0xFF;

    unsigned short CRC = CRC16Fast(Buffer,Lenght);
    Buffer[Lenght]     = CRC >> 8;
    Buffer[Lenght+1]   = CRC;

    if(!Write(Buffer,Lenght+2))
    {
        FMain->L4->Caption = "Сбой посылки";
        return false;
    }

    Sleep(10);

    if(!Read(Buffer,6 + 2 + SizeBlock[Block],Readed,Timeout))
    {
        FMain->L4->Caption = "Сбой приема";
        return false;
    }

    if((CRC = CRC16Fast(Buffer,Readed)) != 0x0000)
    {
        Beep();
        FMain->L4->Caption = "CRC: "+IntToStr(CRC);
        return false;
    }

    return true;
}
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
bool TModbus::WriteBlock(int NetAddr,int Block,void *Data)
{
    int Readed,Lenght;
    int SizeBlock[10] =
    {
        20,7,8,64,32,24,64,81,81,81			// Размеры блоков данных ПСС-07
    };

    Lenght = SizeBlock[Block] + 6;

    Buffer[0] = NetAddr & 0xFF;
    Buffer[1] = 0x46;
    Buffer[2] = 0x00;
    Buffer[3] = SizeBlock[Block] + 2;
    Buffer[4] = 0x02;
    Buffer[5] = Block & 0xFF;

    memmove(&Buffer[6],Data,SizeBlock[Block]);

    unsigned short CRC = CRC16Fast(Buffer,Lenght);
    Buffer[Lenght]     = CRC>>8;
    Buffer[Lenght+1]   = CRC;

    if(!Write(Buffer,Lenght+2))
    {
        FMain->L4->Caption = "Сбой посылки";
        return false;
    }

    Sleep(10);

    if(!Read(Buffer,5,Readed,Timeout))
    {
        FMain->L4->Caption = "Сбой приема";
        return false;
    }

    if((CRC = CRC16Fast(Buffer,Readed)) != 0x0000)
    {
        Beep();
        FMain->L4->Caption = "CRC: "+IntToStr(CRC);
        return false;
    }

    return true;
};
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
// Чтение блока данных ПСС-07 (WIDE / 2022)
bool TModbus::ReadBlock_NEW(int NetAddr,int Block)
{
    int Readed,Lenght = 6;

    int SizeBlock[11] =
    {
        20,  7, 16, 128, 64, 24, 128,					// Размеры блоков данных ПСС-07 (WIDE 2022)
        81, 81, 81, 8
    };

    Buffer[0] = NetAddr & 0xFF;
    Buffer[1] = 0x46;
    Buffer[2] = 0x00;
    Buffer[3] = 0x02;
    Buffer[4] = 0x01;
    Buffer[5] = Block & 0xFF;

    unsigned short CRC = CRC16Fast(Buffer,Lenght);
    Buffer[Lenght]     = CRC >> 8;
    Buffer[Lenght+1]   = CRC;

    if(!Write(Buffer,Lenght+2))
    {
        FMain->ALabel->Caption = "ReadBlock_NEW Сбой посылки";
        return false;
    }

// Sleep(1);		// Sleep(10);

    if(!Read(Buffer,6 + 2 + SizeBlock[Block],Readed,2*Timeout))
    {
        FMain->ALabel->Caption = "ReadBlock_NEW Сбой приема";
        return false;
    }

    if((CRC = CRC16Fast(Buffer,Readed)) != 0x0000)
    {
        Beep();
        FMain->ALabel->Caption = "ReadBlock_NEW CRC Fail: " + IntToStr(CRC);
        return false;
    }

    return true;
};
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
// Запись блока данных ПСС-07 (WIDE / 2022)
bool TModbus::WriteBlock_NEW(int NetAddr,int Block,void *Data)
{
    int Readed,Lenght;

    int SizeBlock[11] =
    {
        20,  7, 16, 128, 64, 24, 128,					// Размеры блоков данных ПСС-07 (WIDE 2022)
        81, 81, 81, 8
    };


    Lenght = SizeBlock[Block] + 6;

    Buffer[0] = NetAddr & 0xFF;
    Buffer[1] = 0x46;
    Buffer[2] = 0x00;
    Buffer[3] = SizeBlock[Block] + 2;
    Buffer[4] = 0x02;
    Buffer[5] = Block & 0xFF;

    memmove(&Buffer[6],Data,SizeBlock[Block]);

    unsigned short CRC = CRC16Fast(Buffer,Lenght);
    Buffer[Lenght]     = CRC>>8;
    Buffer[Lenght+1]   = CRC;

    if(!Write(Buffer,Lenght+2))
    {
        FMain->ALabel->Caption = "WriteBlock_NEW Сбой посылки";
        return false;
    }

// Sleep(1);		// Sleep(10);

    if(!Read(Buffer,5,Readed, 2 * Timeout))
    {
        FMain->ALabel->Caption = "WriteBlock_NEW Сбой приема";
        return false;
    }

    if((CRC = CRC16Fast(Buffer, Readed)) != 0x0000)
    {
        Beep();

        FMain->ALabel->Caption = "WriteBlock_NEW CRC Fail: " + IntToStr(CRC);
        return false;
    }

    return true;

};
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
bool TModbus::WriteBlock_3(int NetAddr,int Block,void *Data)
{
    int Readed,Lenght;

    int SizeBlock[16] =				// Размеры блоков данных ПСС-07 (WIDE 2022)
    {
        20,  7,  16, 128,
        64, 24, 128,  81,
        81, 81,   1,
        1,1,1,1,1
    };

    Lenght = SizeBlock[Block] + 6;

    Buffer[0] = NetAddr & 0xFF;
    Buffer[1] = 0x46;
    Buffer[2] = 0x00;
    Buffer[3] = SizeBlock[Block] + 2;
    Buffer[4] = 0x02;
    Buffer[5] = Block & 0xFF;

    memmove(&Buffer[6],Data,SizeBlock[Block]);

    unsigned short CRC = CRC16Fast(Buffer,Lenght);
    Buffer[Lenght]     = CRC>>8;
    Buffer[Lenght+1]   = CRC;

    if(!Write(Buffer,Lenght+2))
    {
        FMain->LMBStatus->Caption = "Сбой посылки";
        return false;
    };

    Sleep(10);

    if(!Read(Buffer,5,Readed,Timeout))
    {
        FMain->LMBStatus->Caption = "Сбой приема";
        return false;
    };

    if((CRC = CRC16Fast(Buffer,Readed)) != 0x0000)
    {
        Beep();

        FMain->LMBStatus->Caption = "CRC: "+IntToStr(CRC);
        return false;
    }

    return true;
};
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
#pragma package(smart_init)
