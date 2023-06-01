//-----------------------------------------------------------------------------------------------------
//  Source      : FileName.cpp
//  Created     : 01.06.2022
//  Author      : Alexandr Volvenkin
//  email       : aav-36@mail.ru
//  GitHub      : https://github.com/AlexandrVolvenkin
//-----------------------------------------------------------------------------------------------------
#include "DataBase.h"
#include "Platform.h"
#include "Pss21.h"
#include "DataStore.h"
#include <cstddef>


////-----------------------------------------------------------------------------------------------------
//CDataBase::CDataBase()
//{
////    m_pxDataStore = new CDataStore();
//}
//
////-----------------------------------------------------------------------------------------------------
//CDataBase::~CDataBase()
//{
////    delete m_pxDataStore;
//}

//-----------------------------------------------------------------------------------------------------
uint8_t CDataBase::RestoreDefault(void)
{
    // ����������� �� �� ���������.
    CDataStore::Init();

    enum
    {
        IDDLE = 0,
        BLOCK_WRITE_START,
        BLOCK_WRITE_END_WAITING,
        BLOCK_ERROR,
        ALL_BLOCKS_WRITED,
    };

    uint8_t uiBlockCounter = 0;
    uint8_t uiFsmState = BLOCK_WRITE_START;

    while (1)
    {
        switch (uiFsmState)
        {
        case BLOCK_WRITE_START:
            // �������� �� ��� �����?
            if (uiBlockCounter < GetBlocksNumber())
            {
                // ��������� ������ �� ���� �� ��������� �����,
                uint16_t uiLength = CDataStore::ReadBlockFlash(CPss21::m_auiIntermediateBuff, uiBlockCounter);
                // ���� �� ��������?
                if (uiLength)
                {
                    // �������� ������ � ���������.
                    // ���� �� ������ � ������?
                    if (CDataStore::WriteBlock(CPss21::m_auiIntermediateBuff,
                                               uiLength,
                                               uiBlockCounter + CDataStore::BLOCKS_CONTROL_DATA_BLOCK_NUMBER))
                    {
                        uiFsmState = BLOCK_WRITE_END_WAITING;
                    }
                    // ��� ������ ����� �� ��������� ������?
                    else if (CDataStore::GetFsmEvent() == CDataStore::WRITE_ERROR_FSM_EVENT)
                    {
                        uiFsmState = BLOCK_ERROR;
                    }
                }
                else
                {
                    uiFsmState = BLOCK_ERROR;
                }
            }
            else
            {
                uiFsmState = ALL_BLOCKS_WRITED;
            }

            break;

        case BLOCK_WRITE_END_WAITING:
            // ���� ������� �������?
            if (CDataStore::GetFsmEvent() == CDataStore::WRITE_OK_FSM_EVENT)
            {
                uiBlockCounter++;
                uiFsmState = BLOCK_WRITE_START;
            }
            // ��� ������ ����� �� ��������� ������?
            else if (CDataStore::GetFsmEvent() == CDataStore::WRITE_ERROR_FSM_EVENT)
            {
                uiFsmState = BLOCK_ERROR;
            }
            break;

        case ALL_BLOCKS_WRITED:
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
bool CDataBase::IntegrityCheck(void)
{
    // Crc ����� ��������� � ��� Crc ���������� � ��������� �����?
    return CDataStore::CompareCurrentWithStoredCrc();
}

//-----------------------------------------------------------------------------------------------------
void CDataBase::SignatureCreate(void)
{
    // �������� Crc �� Crc ���� ������.
    CDataStore::CrcOfBlocksCrcCreate();
}

//-----------------------------------------------------------------------------------------------------
bool CDataBase::SignatureCheck(void)
{
    // Crc �� Crc ���� ������ ���������?
    return CDataStore::CrcOfBlocksCrcCheck();
}

//-----------------------------------------------------------------------------------------------------
uint8_t CDataBase::Check(void)
{
    CPss21::SetErrorCode(NO_ERROR);
    // ����� ���� ������ �� ����������?
    if (CDataStore::Check())
    {
        // ��� ����� ����������� ������� ���� ������?
        if (IntegrityCheck())
        {
            // ���� ������ ������������ �������������?
            if (SignatureCheck())
            {
                return 1;
            }
            else
            {
                CPss21::SetErrorCode(DB_ERROR);
                return 0;
            }
        }
        else
        {
            CPss21::SetErrorCode(DB_ERROR);
            // ����������� �� �� ���������.
            // �� �� ��������� ������������� �������?
            RestoreDefault();
            return 0;
        }
    }
    else
    {
        CPss21::SetErrorCode(DB_ERROR);
        // ����������� �� �� ���������.
        // �� �� ��������� ������������� �������?
        RestoreDefault();
        return 0;
    }
}

//-----------------------------------------------------------------------------------------------------
uint16_t CDataBase::ReadBlock(uint8_t *puiDestination, uint8_t uiBlock)
{
    return CDataStore::ReadBlock(puiDestination,
                                 uiBlock + (CDataStore::BLOCKS_CONTROL_DATA_BLOCK_NUMBER));
}

//-----------------------------------------------------------------------------------------------------
uint16_t CDataBase::WriteBlock(uint8_t *puiSource, uint8_t uiBlock)
{
    return CDataStore::WriteBlock(puiSource,
                                  GetBlockLength(uiBlock),
                                  uiBlock + (CDataStore::BLOCKS_CONTROL_DATA_BLOCK_NUMBER));
}

//-----------------------------------------------------------------------------------------------------
uint16_t CDataBase::GetBlockLength(uint8_t uiBlock)
{
    // ���� ����������?
    if (uiBlock < GetBlocksNumber())
    {
        return CDataStore::GetBlockLength(uiBlock + CDataStore::BLOCKS_CONTROL_DATA_BLOCK_NUMBER);
    }
    else
    {
        return 0;
    }
}

//-----------------------------------------------------------------------------------------------------
