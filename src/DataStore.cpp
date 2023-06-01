
//-----------------------------------------------------------------------------------------------------
//  Source      : FileName.cpp
//  Created     : 01.06.2022
//  Author      : Alexandr Volvenkin
//  email       : aav-36@mail.ru
//  GitHub      : https://github.com/AlexandrVolvenkin
//-----------------------------------------------------------------------------------------------------
#include "DataStore.h"
#include "Platform.h"
#include "Pss21.h"
#include "Crc.h"
#include "HammingCodes.h"


// ��������� ������ ������� ��������.
CDataStore::TBlocksControlData CDataStore::m_xBlocksControlData;
// ������ ����������� ���� ������.
uint16_t CDataStore::m_auiBlocksCurrentCrc[MAX_BLOCKS_NUMBER];
uint8_t* CDataStore::m_puiIntermediateBuff;
CTimer CDataStore::m_xTimer;
uint8_t CDataStore::m_uiFsmState;
uint8_t CDataStore::m_uiFsmEvent;

////-----------------------------------------------------------------------------------------------------
//CDataStore::CDataStore()
//{
////    m_pxStorageDevice = new CEeprom();
//    m_puiIntermediateBuff = CPss21::m_auiIntermediateBuff;
//    SetFsmState(IDDLE);
//    SetSavedFsmState(IDDLE);
//}
//
////-----------------------------------------------------------------------------------------------------
//CDataStore::~CDataStore()
//{
////    delete m_pxStorageDevice;
//}

//-----------------------------------------------------------------------------------------------------
void CDataStore::Init(void)
{
    m_puiIntermediateBuff = CPss21::m_auiIntermediateBuff;
    SetFsmState(IDDLE);

    // ������� ��������� ��������.
    memset(reinterpret_cast<uint8_t*>(&m_xBlocksControlData),
           0,
           sizeof(m_xBlocksControlData));

    // ��������� �������� �� �������������� ������.
    m_xBlocksControlData.
    axBlockPositionData[BLOCKS_CONTROL_DATA].uiOffset = BLOCKS_CONTROL_DATA_BEGIN;
    // ��������� ������ ��������� ������.
    m_xBlocksControlData.
    axBlockPositionData[BLOCKS_CONTROL_DATA].uiLength = sizeof(struct TBlocksControlData);
    // ���������� ���� ���������� ���������
    // ������ ���� �������� �������.
    // ��� ��� �� ���� ����������� ��������� ������.
    // ������������ ����������������������� ��� ��������(8,4).
    // ����������� - 1.5: ���� ���� ������������� � ������� ����� 12 ���,
    // �� ���� ���� �������� ������ ���������� ��� ����� ������������.
    // ��������� ������ �������������� ������.
    m_xBlocksControlData.
    axBlockPositionData[BLOCKS_CONTROL_DATA].uiEncodedLength =
        (CHammingCodes::CalculateEncodedDataLength(sizeof(struct TBlocksControlData) + CRC_LENGTH));
    // ��������� �������� ��� ���������� ���������.
    m_xBlocksControlData.uiFreeSpaceOffset =
        (BLOCKS_CONTROL_DATA_BEGIN +
         CHammingCodes::CalculateEncodedDataLength(sizeof(struct TBlocksControlData) + CRC_LENGTH));
    // ��������� ������� - ���� ������ �� ������������ �������������.
    m_xBlocksControlData.uiCrcOfBlocksCrc = 0;
}

//-----------------------------------------------------------------------------------------------------
uint16_t CDataStore::ReadBlock(uint8_t *puiDestination, uint8_t uiBlock)
{
    // ��������� ����� �� ������� ������?
    if (uiBlock >= MAX_BLOCKS_NUMBER)
    {
        // ��� ������.
        return 0;
    }

    uint16_t uiLength;
    uint16_t uiEncodedLength;
    uint16_t uiSourceOffset;
    uint8_t auiTempArray[256];

    // ���� ����������?
    if ((m_xBlocksControlData.
            axBlockPositionData[uiBlock].uiLength != 0) &&
            (m_xBlocksControlData.
             axBlockPositionData[uiBlock].uiEncodedLength != 0))
    {
        // ������� ����� ����� � EEPROM.
        uiSourceOffset = m_xBlocksControlData.
                         axBlockPositionData[uiBlock].uiOffset;
        // ������� ������ �����.
        uiLength = m_xBlocksControlData.
                   axBlockPositionData[uiBlock].uiLength;
        // ������� ������ ��������������� �����.
        uiEncodedLength = m_xBlocksControlData.
                          axBlockPositionData[uiBlock].uiEncodedLength;
    }
    else
    {
        // ��� ������.
        return 0;
    }

    // ��������� �������������� ������.
    CEeprom::Read(auiTempArray, uiSourceOffset, uiEncodedLength);

    // ���������� ����������� ������.
    CHammingCodes::HammingCodesToBytes(auiTempArray, auiTempArray, uiEncodedLength);

    // ������� ����������� ����� �����.
    uint16_t uiCrc = static_cast<uint16_t>(auiTempArray[uiLength]);
    uiCrc |= (static_cast<uint16_t>(auiTempArray[uiLength + 1]) << 8);
    // �������� ����������� ����� �����.
    uint16_t uiCalculatedCrc = usCrc16(auiTempArray, uiLength);

    // ���� �� ��������?
    if (uiCrc == uiCalculatedCrc)
    {
        // �������� Crc �������� �����.
        m_auiBlocksCurrentCrc[uiBlock] = uiCrc;
        memcpy(puiDestination, auiTempArray, uiLength);
        return uiLength;
    }
    else
    {
        CPss21::SetErrorCode(DB_ERROR);
        // ��� ������.
        return 0;
    }
}

//-----------------------------------------------------------------------------------------------------
uint16_t CDataStore::Write(uint8_t *puiSource, uint16_t uiLength, uint8_t uiBlock)
{
    // ��������� ����� �� ������� ������?
    if (uiBlock >= MAX_BLOCKS_NUMBER)
    {
        // ��� ������.
        return 0;
    }

    uint16_t uiEncodedByteCounter;
    uint8_t auiTempArray[256];

    memcpy(auiTempArray, puiSource, uiLength);
    // �������� ����������� ����� ����������� ������.
    uint16_t uiCrc = usCrc16(puiSource, uiLength);
    auiTempArray[uiLength] = static_cast<uint8_t>(uiCrc);
    auiTempArray[uiLength + 1] = static_cast<uint8_t>(uiCrc >> 8);

    m_xBlocksControlData.
    axBlockPositionData[uiBlock].uiCrc = uiCrc;

    uiEncodedByteCounter =
        CHammingCodes::BytesToHammingCodes(m_puiIntermediateBuff, auiTempArray, (uiLength + CRC_LENGTH));

    // ������ �������� ����� ������ - uiBlock ����������� �������?
    if (m_xBlocksControlData.
            axBlockPositionData[uiBlock].uiEncodedLength == 0)
    {
        // �������� ����� ���� ������.
        // �������� �������� �� ����.
        m_xBlocksControlData.
        axBlockPositionData[uiBlock].uiOffset = m_xBlocksControlData.uiFreeSpaceOffset;
        // ������� �������� �� ��������� �����.
        m_xBlocksControlData.uiFreeSpaceOffset += uiEncodedByteCounter;
        // �������� ������ ��������� ������.
        m_xBlocksControlData.
        axBlockPositionData[uiBlock].uiLength = uiLength;
        // �������� ������ �������������� ������.
        m_xBlocksControlData.
        axBlockPositionData[uiBlock].uiEncodedLength = uiEncodedByteCounter;
    }

    CEeprom::WriteInterrupt(m_xBlocksControlData.
                            axBlockPositionData[uiBlock].uiOffset,
                            m_puiIntermediateBuff,
                            uiEncodedByteCounter);

    // Out length.
    return uiEncodedByteCounter;
}

//-----------------------------------------------------------------------------------------------------
bool CDataStore::WriteBlock(uint8_t *puiSource, uint16_t uiLength, uint8_t uiBlock)
{
    if (GetFsmState() == IDDLE)
    {
        if (Write(puiSource,
                  uiLength,
                  uiBlock))
        {
            SetFsmEvent(WRITE_IN_PROGRESS_FSM_EVENT);
            SetFsmState(START_WRITE);
            return true;
        }
        else
        {
            SetFsmEvent(WRITE_ERROR_FSM_EVENT);
            return false;
        }
    }
    else
    {
        SetFsmEvent(STORAGE_DEVICE_BUSY_FSM_EVENT);
        return false;
    }
}

//-----------------------------------------------------------------------------------------------------
bool CDataStore::CompareCurrentWithStoredCrc(void)
{
    // ������� ������� Crc ����� � ��� Crc ���������� � ��������� ����� � ���������� ������ ������.
    for (uint16_t i = BLOCKS_CONTROL_DATA_BLOCK_NUMBER;
            i < (MAX_BLOCKS_NUMBER - BLOCKS_CONTROL_DATA_BLOCK_NUMBER);
            i++)
    {
        if (m_auiBlocksCurrentCrc[i] !=
                m_xBlocksControlData.axBlockPositionData[i].uiCrc)
        {
            return false;
        }
    }

    // ��� ����� ����������� ������� ���� ������.
    return true;
}

//-----------------------------------------------------------------------------------------------------
// ���������� ������ ���� ���� ������ ������������ �������������.
void CDataStore::CrcOfBlocksCrcCreate(void)
{
    // ����������� ����� ����������� �� ������� ����������� ���� ������, �� ������� ���������.
    // ����������� ��� ������ � ����������� ������� ����� ������ ����� ������������.
    // ���� ��� � ������������ ��������������� � ���, ��� ���� ������ ������� �� ��������,
    // � �� ������������ �������������. � ���� ������ ������ ��������� � ����� ������������ �� ������,
    // ������ ������������ ��� ������ ���� ������.
    uint16_t auiBlocksCrc[MAX_BLOCKS_NUMBER - BLOCKS_CONTROL_DATA_BLOCK_NUMBER];

    // ������� Crc ���� ������, �� ������� ���������.
    for (uint16_t i = 0;
            i < (MAX_BLOCKS_NUMBER - BLOCKS_CONTROL_DATA_BLOCK_NUMBER);
            i++)
    {
        auiBlocksCrc[i] =
            m_xBlocksControlData.axBlockPositionData[i + BLOCKS_CONTROL_DATA_BLOCK_NUMBER].uiCrc;
    }

    // �������� Crc ���� ������, �� ������� ���������.
    m_xBlocksControlData.uiCrcOfBlocksCrc =
        usCrc16(reinterpret_cast<uint8_t*>(auiBlocksCrc),
                ((MAX_BLOCKS_NUMBER - BLOCKS_CONTROL_DATA_BLOCK_NUMBER) * sizeof(uint16_t)));

}

//-----------------------------------------------------------------------------------------------------
bool CDataStore::CrcOfBlocksCrcCheck(void)
{
    // ����������� ����� ����������� �� ������� ����������� ���� ������, �� ������� ���������.
    // ����������� ��� ������ � ����������� ������� ����� ������ ����� ������������.
    // ���� ��� � ������������ ��������������� � ���, ��� ���� ������ ������� �� ��������,
    // � �� ������������ �������������. � ���� ������ ������ ��������� � ����� ������������ �� ������,
    // ������ ������������ ��� ������ ���� ������.
    uint16_t auiBlocksCrc[MAX_BLOCKS_NUMBER - BLOCKS_CONTROL_DATA_BLOCK_NUMBER];

    // ������� Crc ���� ������, �� ������� ���������.
    for (uint16_t i = 0;
            i < (MAX_BLOCKS_NUMBER - BLOCKS_CONTROL_DATA_BLOCK_NUMBER);
            i++)
    {
        auiBlocksCrc[i] =
            m_xBlocksControlData.axBlockPositionData[i + BLOCKS_CONTROL_DATA_BLOCK_NUMBER].uiCrc;
    }

    // Crc �� Crc ���� ������ ��������� � ���������� � ��������� �����?
    if (m_xBlocksControlData.uiCrcOfBlocksCrc ==
            usCrc16(reinterpret_cast<uint8_t*>(auiBlocksCrc),
                    ((MAX_BLOCKS_NUMBER - BLOCKS_CONTROL_DATA_BLOCK_NUMBER) * sizeof(uint16_t))))
    {
        return true;
    }
    else
    {
        return false;
    }
}

//-----------------------------------------------------------------------------------------------------
uint8_t CDataStore::Check(void)
{
    CDataStore::Init();

    // ��������� ���� ��������?
    if (!(ReadBlock(reinterpret_cast<uint8_t*>(&m_xBlocksControlData), BLOCKS_CONTROL_DATA)))
    {
        return 0;
    }

    enum
    {
        IDDLE = 0,
        BLOCK_CHECK_START,
        BLOCK_WRITE_START,
        BLOCK_WRITE_END_WAITING,
        NEXT_BLOCK,
        BLOCK_ERROR,
        ALL_BLOCKS_CHECKED,
    };

    uint8_t uiBlockCounter = 0;
    uint8_t uiFsmState = BLOCK_CHECK_START;
    uint16_t uiLength;

    CHammingCodes::SetErrorCode(CHammingCodes::NONE_ERROR);

    while (1)
    {
        switch (uiFsmState)
        {
        case BLOCK_CHECK_START:
            // ��������� �� ��� �����?
            if (uiBlockCounter < MAX_BLOCKS_NUMBER)
            {
                uiLength = ReadBlock(m_puiIntermediateBuff, uiBlockCounter);
                // ���� �� ��������?
                if (uiLength)
                {
                    // ���� ������������ ����� ����������� ������?
                    if (CHammingCodes::GetErrorCode() != CHammingCodes::NONE_ERROR)
                    {
                        // ������� ��������������� ���� � ���������.
                        uiFsmState = BLOCK_WRITE_START;
                    }
                    else
                    {
                        uiFsmState = NEXT_BLOCK;
                    }
                }
                else
                {
                    uiFsmState = BLOCK_ERROR;
                }
            }
            else
            {
                uiFsmState = ALL_BLOCKS_CHECKED;
            }
            break;

        case BLOCK_WRITE_START:
            // �������� ������ � ���������.
            // ���� �� ������ � ������?
            if (WriteBlock(m_puiIntermediateBuff, uiLength, uiBlockCounter))
            {
                uiFsmState = BLOCK_WRITE_END_WAITING;
            }
            // ��� ������ ����� �� ��������� ������?
            else if (CDataStore::GetFsmEvent() == CDataStore::WRITE_ERROR_FSM_EVENT)
            {
                uiFsmState = BLOCK_ERROR;
            }
            break;

        case BLOCK_WRITE_END_WAITING:
            // ���� ������� �������?
            if (CDataStore::GetFsmEvent() == CDataStore::WRITE_OK_FSM_EVENT)
            {
                uiFsmState = NEXT_BLOCK;
            }
            // ��� ������ ����� �� ��������� ������?
            else if (CDataStore::GetFsmEvent() == CDataStore::WRITE_ERROR_FSM_EVENT)
            {
                uiFsmState = BLOCK_ERROR;
            }
            break;

        case NEXT_BLOCK:
            uiBlockCounter++;
            uiFsmState = BLOCK_CHECK_START;
            break;

        case ALL_BLOCKS_CHECKED:
            // ������ �� ����������.
            return 1;
            break;

        case BLOCK_ERROR:
            return 0;
            break;

        default:
            break;
        }

        CDataStore::Fsm();

        delay_ms(10);

        CPlatform::WatchdogReset();
    }
}

//-----------------------------------------------------------------------------------------------------
void CDataStore::Fsm(void)
{
    switch (GetFsmState())
    {
    case IDDLE:
        break;

    case START_WRITE:
        // ��������� ����� �������� ���������� � ������.
        GetTimerPointer() -> Set(READY_TO_WRITE_WAITING_TIMEOUT);
        SetFsmState(READY_TO_WRITE_WAITING);
        break;

    case READY_TO_WRITE_WAITING:
        if (CEeprom::IsReadyToWrite())
        {
            // ��������� ����� �������� ��������� ������ ������.
            GetTimerPointer() -> Set(WRITE_END_WAITING_TIMEOUT);
            CEeprom::StartWrite();
            SetFsmState(WRITE_END_WAITING);
        }
        // ����� �������� ���������� � ������ �����������?
        else if (GetTimerPointer() -> IsOverflow())
        {
            SetFsmEvent(WRITE_ERROR_FSM_EVENT);
            SetFsmState(IDDLE);
        }
        break;

    case WRITE_END_WAITING:
        if (CEeprom::GetBufferIsWrited())
        {
            SetFsmState(START_WRITE_BLOCKS_CONTROL_DATA);
        }
        // ����� �������� ��������� ������ �����������?
        else if (GetTimerPointer() -> IsOverflow())
        {
            SetFsmEvent(WRITE_ERROR_FSM_EVENT);
            SetFsmState(IDDLE);
        }
        break;


    // ������ ���������� �����.
    case START_WRITE_BLOCKS_CONTROL_DATA:
        // ������ ���������.
        // �������� ��������� ��������� ��������.
        if (Write(reinterpret_cast<uint8_t*>(&m_xBlocksControlData),
                  sizeof(m_xBlocksControlData),
                  BLOCKS_CONTROL_DATA))
        {
            // ��������� ����� �������� ���������� � ������.
            GetTimerPointer() -> Set(READY_TO_WRITE_WAITING_TIMEOUT);
            SetFsmState(READY_TO_WRITE_WAITING_BLOCKS_CONTROL_DATA);
        }
        else
        {
            SetFsmEvent(WRITE_ERROR_FSM_EVENT);
            SetFsmState(IDDLE);
        }
        break;

    case READY_TO_WRITE_WAITING_BLOCKS_CONTROL_DATA:
        if (CEeprom::IsReadyToWrite())
        {
            // ��������� ����� �������� ��������� ������.
            GetTimerPointer() -> Set(WRITE_END_WAITING_TIMEOUT);
            CEeprom::StartWrite();
            SetFsmState(WRITE_END_WAITING_BLOCKS_CONTROL_DATA);
        }
        // ����� �������� ���������� � ������ �����������?
        else if (GetTimerPointer() -> IsOverflow())
        {
            SetFsmEvent(WRITE_ERROR_FSM_EVENT);
            SetFsmState(IDDLE);
        }
        break;

    case WRITE_END_WAITING_BLOCKS_CONTROL_DATA:
        if (CEeprom::GetBufferIsWrited())
        {
            SetFsmEvent(WRITE_OK_FSM_EVENT);
            SetFsmState(IDDLE);
        }
        // ����� �������� ��������� ������ �����������?
        else if (GetTimerPointer() -> IsOverflow())
        {
            SetFsmEvent(WRITE_ERROR_FSM_EVENT);
            SetFsmState(IDDLE);
        }
        break;

    default:
        break;
    }
}

//-----------------------------------------------------------------------------------------------------








//-----------------------------------------------------------------------------------------------------
uint16_t CDataStore::ReadBlockFlash(uint8_t *puiDestination, uint8_t uiBlock)
{
    return CFlash::ReadBlock(puiDestination, uiBlock);
}
