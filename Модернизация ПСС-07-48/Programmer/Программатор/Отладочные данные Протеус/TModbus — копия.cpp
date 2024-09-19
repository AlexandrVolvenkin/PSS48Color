

// ����������� ������ TModbus
TModbus::TModbus(void):Handle(INVALID_HANDLE_VALUE),Timeout(1000),ERRCode(0),
    TotalTimeout(1500),Buffer(NULL) { }


//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
// ������ ����� ������ ���-07 (WIDE / 2022)
bool TModbus::WriteBlock_NEW(int NetAddr,int Block,void *Data)
{
    int Readed,Lenght;
    /*
     int SizeBlock[16] =				// ������� ������ ������ ���-07 (WIDE 2022)
     {
      20,  7,  16, 128,
      64, 24, 128,  81,
      81, 81,   1,
      1,1,1,1,1
     };
      */
    int SizeBlock[11] =
    {
        20,  7, 16, 128, 64, 24, 128,					// ������� ������ ������ ���-07 (WIDE 2022)
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
        FMain->L4->Caption = "���� �������";
        return false;
    }

// Sleep(1);		// Sleep(10);
// ���������� - Timeout ���������������� � ������������ �� ����� ���������� ������� �����������
// ������ � ����, ������ 1 �������
    if(!Read(Buffer,5,Readed, Timeout))
    {
        // ����� � ������� 1 ������� ������ �� ������, �� ������ � �� � ������������ � ����.
        FMain->L4->Caption = "���� ������";
        return false;
    }
// ������ ������, �� �� �� ����� ����� ��� ��� ���.
// �������� �� ��������� ������� ���������� �������� ����
    int ReadedCounter = Readed;
// ��������� ���� ����������� ��������� ������
    while (ReadedCounter < 5)
    {
// ������� ����� �������� ���������� ������ � 1s �� 50mc.
// ���� �� ��� ����� ������ �� ������, �� ��� �� ������ ����� ������ ������.
// ������� �������� � ������ �� ���������� �������� ���� � �������� ���������� ���������� ���������
// �������� ����.
        if(!Read(Buffer[ReadedCounter],5 - ReadedCounter,Readed, 50))
        {
            // ����� � ������� 50mc ������ �� ������, �� ��� �� ������ ����� ������ ������.
            // �������� ��� ����������� �����.
            break;
        }
        else
        {
            // ������ ������ ������ ��� ����� 50mc, ������������� ���������� ������ ��� �� ��� �������
            // ��������� ����
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
