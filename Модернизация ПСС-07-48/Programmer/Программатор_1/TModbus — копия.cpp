

// Конструктор класса TModbus
TModbus::TModbus(void):Handle(INVALID_HANDLE_VALUE),Timeout(1000),ERRCode(0),
    TotalTimeout(1500),Buffer(NULL) { }


//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
// Запись блока данных ПСС-07 (WIDE / 2022)
bool TModbus::WriteBlock_NEW(int NetAddr,int Block,void *Data)
{
    int Readed,Lenght;
    /*
     int SizeBlock[16] =				// Размеры блоков данных ПСС-07 (WIDE 2022)
     {
      20,  7,  16, 128,
      64, 24, 128,  81,
      81, 81,   1,
      1,1,1,1,1
     };
      */
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
        FMain->L4->Caption = "Сбой посылки";
        return false;
    }

// Sleep(1);		// Sleep(10);
// переменная - Timeout инициализируется в конструкторе на время отсутствия события поступления
// данных в порт, равное 1 секунде
    if(!Read(Buffer,5,Readed, Timeout))
    {
        // еслим в течении 1 секунды данные не пришли, то ошибка и всё в соответствии с этим.
        FMain->L4->Caption = "Сбой приема";
        return false;
    }
// данные пришли, но мы не знаем конец это или нет.
// сохраним во временный счётчик количество принятых байт
    int ReadedCounter = Readed;
// ограничим цикл количеством ожидаемых данных
    while (ReadedCounter < 5)
    {
// изменим время таймаута отсутствия данных с 1s на 50mc.
// если за это время данные не придут, то это по любому конец модбас пакета.
// изменим смещение в буфере на количество принятых байт и уменьшим количество ожидаемого максимума
// принятых байт.
        if(!Read(Buffer[ReadedCounter],5 - ReadedCounter,Readed, 50))
        {
            // еслим в течении 50mc данные не пришли, то это по любому конец модбас пакета.
            // проверим его контрольную сумму.
            break;
        }
        else
        {
            // данные пришли меньше чем через 50mc, следовательно предыдущие данные это не вся посылка
            // продолжим приём
            ReadedCounter += Readed;
        }
    };

    if((CRC = CRC16Fast(Buffer,Readed)) != 0x0000)
    {
        Beep();
        FMain->L4->Caption = "CRC: "+IntToStr(CRC);
        return false;
    }

    return true;

};
