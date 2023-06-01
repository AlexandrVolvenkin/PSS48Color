//-----------------------------------------------------------------------------------------------------
//  Source      : FileName.cpp
//  Created     : 01.06.2022
//  Author      : Alexandr Volvenkin
//  email       : aav-36@mail.ru
//  GitHub      : https://github.com/AlexandrVolvenkin
//-----------------------------------------------------------------------------------------------------
#include "Pss21.h"
#include "Platform.h"
#include "InputDevice.h"
#include "Mvsn21.h"
#include "ModuleMrXX.h"
#include "Crc.h"
#include "ModbusRTU.h"
#include "DataBase.h"
#include "DataTypes.h"
#include "Alarm.h"
#include "NotifyerControl.h"
#include "Buzzer.h"
#include "Event.h"
#include "Dfa.h"


//uint8_t CPss21::m_aucRtuCoilsArray[];
//uint8_t CPss21::m_aucRtuDiscreteInputsArray[];
uint16_t CPss21::m_aucRtuHoldingRegistersArray[];
uint16_t CPss21::m_aucRtuInputRegistersArray[];
uint8_t CPss21::m_aucRtuDiscreteDataArray[];
uint8_t CPss21::m_auiReceiveMessageBuff[];
//uint8_t CPss21::m_auiTransmitMessageBuff[];
uint8_t CPss21::m_auiIntermediateBuff[];
uint8_t CPss21::m_auiSpiRxBuffer[];
uint8_t CPss21::m_auiSpiTxBuffer[];

uint8_t CPss21::m_uiFsmState;
uint8_t CPss21::m_uiType;
//CDriver* CPss21::m_pxDriver;
//CDevice* CPss21::m_apxDevices[];
CDriver* CPss21::m_apxDrivers[];

CUart CPss21::m_xUart1(&UBRR1H, &UBRR1L, &UCSR1A, &UCSR1B, &UCSR1C, &UDR1, &DDRC, DDC7, &PORTC, PC7);
CModbusRTU CPss21::m_xModbusRtuOne;

//CDataBase CPss21::m_xDataBase;
//CDataBase* CPss21::m_pxDataBase;
//CDataStore CPss21::m_xDataStore;

CMultiFunctionKey CPss21::m_xReceiptKey(CPlatform::KeyOneState);
CMultiFunctionKey CPss21::m_xResetKey(CPlatform::KeyTwoState);
CMultiFunctionKey CPss21::m_xCheckKey(CPlatform::KeyThreeState);
CMultiFunctionKey CPss21::m_xExternalKey(CPlatform::ExternalKeyState);

//CIsReceiptButtonPressed CPss21::m_xIsReceiptButtonPressed;

uint8_t CPss21::m_uiPanelReceipt;
uint8_t CPss21::m_uiPanelReset;
uint8_t CPss21::m_uiExternalReceipt;
uint8_t CPss21::m_uiExternalReset;
uint8_t CPss21::m_uiModbusReceipt;
uint8_t CPss21::m_uiModbusReset;
uint8_t CPss21::m_uiDiscreteSignalsReceipt;
uint8_t CPss21::m_uiDiscreteSignalsReset;
uint8_t CPss21::m_uiErrorCode;
uint8_t CPss21::m_uiModuleIndex;
CEventUniteLogicAnd CPss21::m_xEventUniteLogicAndOne(500);
CTimer CPss21::m_xMainCycleTimer(MAIN_CYCLE_PERIOD_TIME);
//CTimer CPss21::m_xPeripheryTimer(PERIPHERY_SCAN_TIME);

CPreventiveAlarmWindowNotifyerControl CPss21::m_xPreventiveAlarmWindowNotifyerControl;
CEmergencyAlarmWindowNotifyerControl CPss21::m_xEmergencyAlarmWindowNotifyerControl;
CStatusLedNotifyerControl CPss21::m_xStatusLedNotifyerControl;
CBuzzerNotifyerControl CPss21::m_xBuzzerNotifyerControl;
CModbusRtuLinkControl CPss21::m_xModbusRtuLinkControl;

uint8_t CPss21::m_uiCommonAlarmType;
uint8_t CPss21::m_uiCommonAlarmTypePrevious;
//CLightBoard CPss21::m_xLightBoard;
TConfigDataPackOne CPss21::m_xDeviceConfigSearch;
TDevConfig CPss21::m_xDeviceConfiguration;
TDeviceState CPss21::m_xDeviceState;
TMBusSetting CPss21::m_xModbusSettings;
CAlarmDfa *CPss21::m_apxAlarmDfa[];

CErrorAlarmDfa *CPss21::m_apxErrorAlarmDfa[];
uint8_t CPss21::m_auiErrorAlarmDataArray[];

TDiscreteOutputControl CPss21::m_axDiscreteOutputControl[];
TOutputData CPss21::m_xDiscreteOutputDataBase;
CAlarmWindow CPss21::m_axAlarmWindowControl[];

__flash uint8_t CPss21::m_auiErrorLedOn[] =
{
    1, 1, 1, 1,    1, 1, 1, 1,
    1, 0, 0, 0,    0, 0, 0, 1,
    1, 0, 0, 0,    0, 0, 0, 1,
    1, 0, 0, 0,    0, 0, 0, 1,
    1, 0, 0, 0,    0, 0, 0, 1,
    1, 1, 1, 1,    1, 1, 1, 1,
};

////-----------------------------------------------------------------------------------------------------
//CPss21::CPss21(uint8_t uiType, CDriver* pxDriver)// :
////    m_uiType(uiType),
////    m_pxDriver(pxDriver)
//{
//
//}
//
////-----------------------------------------------------------------------------------------------------
//CPss21::~CPss21()
//{
//
//}

//-----------------------------------------------------------------------------------------------------
void CPss21::ModbusInit(void)
{
    CPlatform::m_pxUart1 = &m_xUart1;
    m_xModbusRtuOne.Init(CPlatform::m_pxUart1,
                         static_cast<uint32_t>((F_CPU / ((CPss21::m_xModbusSettings.BaudRate) + 1)) / 16UL),
                         'N',
                         8,
                         1,
                         CPss21::m_auiReceiveMessageBuff,
                         CPss21::m_auiReceiveMessageBuff,//reinterpret_cast<uint8_t*>(CPss21::m_aucRtuHoldingRegistersArray),//m_auiTransmitMessageBuff,//
                         CPss21::m_aucRtuDiscreteDataArray,//CPss21::m_aucRtuCoilsArray,
                         CPss21::m_aucRtuDiscreteDataArray,//CPss21::m_aucRtuDiscreteInputsArray,
                         CPss21::m_aucRtuHoldingRegistersArray,
                         CPss21::m_aucRtuInputRegistersArray,
                         DISCRETE_DATA_ARRAY_LENGTH,
                         DISCRETE_DATA_ARRAY_LENGTH,
                         HOLDING_REGISTERS_ARRAY_LENGTH,
                         INPUT_REGISTERS_ARRAY_LENGTH);
    m_xModbusRtuOne.SlaveSet(CPss21::m_xModbusSettings.NetAddress);
    m_xModbusRtuOne.SetFsmState(CModbus::START_REQUEST);
}

//-----------------------------------------------------------------------------------------------------
void CPss21::ModbusDefaultInit(void)
{
    CPss21::m_xModbusSettings.NetAddress = 1;
    // 9600 baud.
    CPss21::m_xModbusSettings.BaudRate = 47;
    // ��� ���� ����.
    CPss21::m_xModbusSettings.LinkParam = 0x08;

    ModbusInit();
}

//-----------------------------------------------------------------------------------------------------
void CPss21::Init(void)
{
    CLightBoard::Init(m_axAlarmWindowControl);
    m_uiCommonAlarmType = NORMAL;
    m_uiCommonAlarmTypePrevious = NORMAL;
    SetErrorCode(0);
    // ������� ��� ������.
    memset(CPss21::m_auiErrorAlarmDataArray, 0, HANDLED_ERROR_NUMBER);
    CPss21::AllAlarmWindowOff();
}

//-----------------------------------------------------------------------------------------------------
void CPss21::AddressBusInit(void)
{
    CPlatform::SpiSlaveSelectAddress0SetPinOutput();
    CPlatform::SpiSlaveSelectAddress1SetPinOutput();
    CPlatform::SpiSlaveSelectAddress2SetPinOutput();
    CPlatform::SpiSlaveSelectAddress3SetPinOutput();
    CPlatform::SpiSlaveSelectEnableSetPinOutput();
    CPlatform::SpiSlaveSelectAddress0On();
    CPlatform::SpiSlaveSelectAddress1On();
    CPlatform::SpiSlaveSelectAddress2On();
    CPlatform::SpiSlaveSelectAddress3On();
    CPlatform::SpiSlaveSelectEnableOff();
};

//-----------------------------------------------------------------------------------------------------
void CPss21::ConnectDevice(uint8_t uiAddress)
{
    uiAddress &= 0x0F;

    if (uiAddress &  BIT(0))
    {
        CPlatform::SpiSlaveSelectAddress0Off();
    }
    else
    {
        CPlatform::SpiSlaveSelectAddress0On();
    }
    if (uiAddress &  BIT(1))
    {
        CPlatform::SpiSlaveSelectAddress1Off();
    }
    else
    {
        CPlatform::SpiSlaveSelectAddress1On();
    }
    if (uiAddress &  BIT(2))
    {
        CPlatform::SpiSlaveSelectAddress2Off();
    }
    else
    {
        CPlatform::SpiSlaveSelectAddress2On();
    }
    if (uiAddress &  BIT(3))
    {
        CPlatform::SpiSlaveSelectAddress3Off();
    }
    else
    {
        CPlatform::SpiSlaveSelectAddress3On();
    }

    CPlatform::SpiSlaveSelectEnableOn();
    CSpi::Enable();
};

//-----------------------------------------------------------------------------------------------------
void CPss21::DisconnectDevice(void)
{
    CSpi::Disable();
    CPlatform::SpiSlaveSelectEnableOff();
    CPlatform::SpiSlaveSelectAddress0Off();
    CPlatform::SpiSlaveSelectAddress1Off();
    CPlatform::SpiSlaveSelectAddress2Off();
    CPlatform::SpiSlaveSelectAddress3Off();
};

//-----------------------------------------------------------------------------------------------------
void CPss21::SearchModules(void)
{
    delay_ms(500);
    CPlatform::WatchdogReset();

    memset(reinterpret_cast<uint8_t*>(&m_xDeviceConfigSearch),
           0,
           sizeof(struct TConfigDataPackOne));

    // ������� ��������� SPI. ����� ������ ������������?
    for (uint8_t i = 0; i < SPI_MAX_BUS_ADDRESS; i++)
    {
        // ���� ������ �� ������� MODULE_REQUEST_QUANTITY ���,
        // �� ��� ��� ��� �� ����������.
        for (uint8_t j = 0; j < MODULE_REQUEST_QUANTITY; j++)
        {
            CPlatform::WatchdogReset();
            delay_ms(5);

            m_auiSpiTxBuffer[0] = GET_MODULE_TYPE_COMMAND;
            ConnectDevice(i);
            delay_us(500);
            CSpi::Exchange(m_auiSpiRxBuffer,
                           m_auiSpiTxBuffer,
                           GET_MODULE_TYPE_REQUEST_LENGTH);
            DisconnectDevice();

            // ������ ��������?
            if (m_auiSpiRxBuffer[SPI_COMMAND_BYTE_OFFSET] ==
                    GET_MODULE_TYPE_COMMAND)
            {
                // ����� ������� ������ ��� ������?
                if (iCrcSummOneByteCompare(&m_auiSpiRxBuffer[SPI_COMMAND_BYTE_OFFSET], 2))
                {
                    // �������� ��� ������ � ������� ��� �������������� ���������� ������� ��� ������ �� SPI.
                    m_xDeviceConfigSearch.axModulesContext[m_xDeviceConfigSearch.uiModulesQuantity].uiType =
                        m_auiSpiRxBuffer[MODULE_TYPE_OFFSET];
                    // �������� ����� ������ � ������� ��� �������������� ���������� ������� ��� ������ �� SPI.
                    m_xDeviceConfigSearch.axModulesContext[m_xDeviceConfigSearch.uiModulesQuantity].uiAddress = i;
                    // ������ ������������. �������� �������� ���������� -
                    // ����������� ���������� ������� � �������.
                    (m_xDeviceConfigSearch.uiModulesQuantity)++;

                    if (m_xDeviceConfigSearch.uiModulesQuantity >=
                            INTERNAL_MODULE_QUANTITY)
                    {
                        return;
                    }
                    // ������� � ������ ���������� ������.
                    break;
                }
            }
        }
    }

    if(m_xDeviceConfiguration.FixConfig)
    {
        // ���������� ���������� � ������� ������������ �� ���������?
        if((m_xDeviceConfiguration.ICount != m_xDeviceConfigSearch.uiDiscreteInputQuantity) ||
                (m_xDeviceConfiguration.OCount != m_xDeviceConfigSearch.uiDiscreteOutputQuantity))
        {
            SetErrorCode(CFG_ERROR);
        }
    }
};

//-----------------------------------------------------------------------------------------------------
void CPss21::InitAllocationContext(TMemoryAllocationConext &xMemoryAllocationConext)
{
    // ���������� � �������� ������� ������ �������� ������� ������������ ���������� ���������.

    xMemoryAllocationConext.puiRxBuffer = m_auiSpiRxBuffer;
    xMemoryAllocationConext.puiTxBuffer = m_auiSpiTxBuffer;
    xMemoryAllocationConext.puiErrorCode = &m_uiErrorCode;

    // ������� ����� ����� ���������� ������.
    xMemoryAllocationConext.uiUsedDiscreteInputs = 0;
    // ��������� ����� ��� �������� ��������� ���������� ������.
    xMemoryAllocationConext.puiDiscreteInputs =
        &m_aucRtuDiscreteDataArray[DISCRETE_INPUTS_BIT_ARRAY_OFFSET];

    // ������� ����� ����� ���������� ������.
    xMemoryAllocationConext.uiUsedDiscreteOutputState = 0;
    // ��������� ����� ��� �������� ��������� ���������� �������.
    xMemoryAllocationConext.puiDiscreteOutputState =
        &m_aucRtuDiscreteDataArray[DISCRETE_OUTPUT_BIT_ARRAY_OFFSET];

    // ������� ����� ����� ���������� ������.
    xMemoryAllocationConext.uiUsedDiscreteOutputs = 0;
    // ��������� ����� ��� ���������� ����������� ��������.
    xMemoryAllocationConext.pxDiscreteOutputControl = m_axDiscreteOutputControl;

    // ������� ����� ����� ���������� ������.
    xMemoryAllocationConext.uiUsedDiscreteOutputDataBase = 0;
    // ��������� ����� � ����� ������ ������� ���������� �������.
    xMemoryAllocationConext.pxDiscreteOutputDataBase = &m_xDiscreteOutputDataBase;

    // ������� ����� ����� ���������� ������.
    xMemoryAllocationConext.uiUsedErrorAlarmDataArray = 0;
    // ��������� ����� ��� �������� ��������� ������.
    xMemoryAllocationConext.puiErrorAlarmDataArray = m_auiErrorAlarmDataArray;
};

//-----------------------------------------------------------------------------------------------------
void CPss21::CreateDevices(void)
{
    TMemoryAllocationConext xMemoryAllocationConext;
    InitAllocationContext(xMemoryAllocationConext);

    for (uint8_t i = 0;
            i < (m_xDeviceConfigSearch.uiModulesQuantity);
            i++)
    {
        CPlatform::WatchdogReset();

        uint8_t uiType = m_xDeviceConfigSearch.axModulesContext[i].uiType;
        switch (uiType)
        {
        case MODULE_TYPE_MVSN:
//        case MODULE_TYPE_MVDI:
//        case MODULE_TYPE_MVDS9:
            if (m_xDeviceConfigSearch.uiDiscreteInputQuantity <
                    DISCRETE_INPUT_MODULE_MAX_NUMBER)
            {
//            m_apxDevices[i] = new CMvsn21;
//            m_apxDevices[i] -> m_pxDriver = new CMvsn21Driver(uiType);
                m_apxDrivers[i] = new CMvsn21Driver(uiType);
                xMemoryAllocationConext.uiAddress = m_xDeviceConfigSearch.axModulesContext[i].uiAddress;
//            m_apxDevices[i] -> m_pxDriver -> Allocate(xMemoryAllocationConext);
                m_apxDrivers[i] -> Allocate(xMemoryAllocationConext);
                m_xDeviceConfigSearch.uiDiscreteInputQuantity += 1;
            }
            break;

        case MODULE_TYPE_MVDO5:
            break;

        case MODULE_TYPE_MR53:
        case MODULE_TYPE_MR54:
            if (m_xDeviceConfigSearch.uiDiscreteOutputQuantity <
                    DISCRETE_OUTPUT_MODULE_MAX_NUMBER)
            {
//            m_apxDevices[i] = new CModuleMrXX;
//            m_apxDevices[i] -> m_pxDriver = new CModuleMrXXDriver(uiType);
                m_apxDrivers[i] = new CModuleMrXXDriver(uiType);
                xMemoryAllocationConext.uiAddress = m_xDeviceConfigSearch.axModulesContext[i].uiAddress;
//            m_apxDevices[i] -> m_pxDriver -> Allocate(xMemoryAllocationConext);
                m_apxDrivers[i] -> Allocate(xMemoryAllocationConext);
                m_xDeviceConfigSearch.uiDiscreteOutputQuantity += 1;
            }
            break;

        case MODULE_TYPE_MVI:
            break;

        default:
            break;
        }
    }
};

//-----------------------------------------------------------------------------------------------------
void CPss21::DestroyDevices(uint8_t uiLength)
{
    for (uint16_t i = 0; i < uiLength; i++)
    {
//        delete m_apxDevices[i];
//        delete m_apxDrivers[i];
    }
};

//-----------------------------------------------------------------------------------------------------
uint16_t CPss21::ReportType(uint8_t *puiDestination, uint16_t uiLength)
{
    memset(m_auiSpiTxBuffer, 0, 6);
    uint8_t uiLengthLocal = 0;
    m_auiSpiTxBuffer[uiLengthLocal++] = GET_MODULE_TYPE_COMMAND;
    m_auiSpiTxBuffer[uiLengthLocal++] = 0;
    m_auiSpiTxBuffer[uiLengthLocal++] = 0;
    m_auiSpiTxBuffer[uiLengthLocal++] = 0;
    m_auiSpiTxBuffer[uiLengthLocal++] = 0;

    ConnectDevice(0);
    delay_us(500);
    CSpi::Exchange(m_auiSpiRxBuffer,
                   m_auiSpiTxBuffer,
                   uiLengthLocal);
    DisconnectDevice();

    return 1;
}

//-----------------------------------------------------------------------------------------------------
void CPss21::SetBytesFromBits(uint8_t *uiDestination,
                              uint8_t *uiSource,
                              uint16_t uiLength)
{
    int16_t shift = 0;

    for (int16_t i = 0; i < uiLength; i++)
    {
        uiDestination[i] = uiSource[(i) / 8] & (1 << shift) ? 1 : 0;
        /* gcc doesn't like: shift = (++shift) % 8; */
        shift++;
        shift %= 8;
    }
}

//-----------------------------------------------------------------------------------------------------
void CPss21::ModbusDeviceControl(uint8_t* puiData, uint16_t uiLength, uint16_t uiAddress)
{
    // ������� - "������������"?
    if (uiAddress == MODBUS_RECEIPT_ADDRESS)
    {
        if (*puiData)
        {
            CPss21::SetModbusReceipt(1);
            SetDiscreteSignalsReceipt(1);
            SendToDevicesModbusReceipt();
        }
    }
    // ������� - "�����"?
    else if (uiAddress == MODBUS_RESET_ADDRESS)
    {
        if (*puiData)
        {
            CPss21::SetModbusReset(1);
            SetDiscreteSignalsReset(1);
            SendToDevicesModbusReset();
        }
    }
    // ��������� ��������� ���� ������������?
    else if ((uiAddress >= ALARM_WINDOWS_BIT_ARRAY_OFFSET) &&
             (uiAddress < (ALARM_WINDOWS_BIT_ARRAY_OFFSET +
                           ALARM_WINDOWS_ARRAY_LENGTH)))
    {
        SetBytesFromBits(&m_aucRtuDiscreteDataArray[uiAddress],
                         puiData,
                         uiLength);
        // ������� �������� � ������������ ������ �����.
        m_xModbusRtuLinkControl.SetFsmEvent(CModbusRtuLinkControl::LINK_SESSION_HAPPENED_FSM_EVENT);
    }
////    // ��������� ��������� ���� ������������?
//    else if ((uiAddress >= DISCRETE_INPUTS_BIT_ARRAY_OFFSET) &&//debag//
//             (uiAddress < (ALARM_WINDOWS_BIT_ARRAY_OFFSET +
//                           ALARM_WINDOWS_ARRAY_LENGTH)))
//    {
//        SetBytesFromBits(&m_aucRtuDiscreteDataArray[uiAddress],
//                         puiData,
//                         uiLength);
//        // ������� �������� � ������������ ������ �����.
//        m_xModbusRtuLinkControl.SetFsmEvent(CModbusRtuLinkControl::LINK_SESSION_HAPPENED_FSM_EVENT);
//    }
}

//-----------------------------------------------------------------------------------------------------
void CPss21::ModbusDeviceControl(uint8_t uiData, uint16_t uiAddress)
{
    ModbusDeviceControl(&uiData, 1, uiAddress);
}

//-----------------------------------------------------------------------------------------------------
bool CPss21::ModbusRtuLinkControlIsOn(void)
{
    if (m_xDeviceConfiguration.MBTimeout != 0)
    {
        return true;
    }
    else
    {
        return false;
    }
}

//-----------------------------------------------------------------------------------------------------
uint16_t CPss21::GetTimeoutModbusRtuLinkControl(void)
{
    return (m_xDeviceConfiguration.MBTimeout * 1000);
}

//-----------------------------------------------------------------------------------------------------
bool CPss21::ModbusRtuLinkControlInProgress(void)
{
    if (m_xDeviceState.ModbusRtuLinkControlInProgress)
    {
        return true;
    }
    else
    {
        return false;
    }
}

//-----------------------------------------------------------------------------------------------------
void CPss21::SetModeModbusRtuLinkControlInProgress(bool bData)
{
    m_xDeviceState.ModbusRtuLinkControlInProgress = bData;
    // �������� ������� ������������.
    CDataBase::WriteBlock(reinterpret_cast<uint8_t*>(&m_xDeviceState),
                          TDataBase::DEVICE_STATE);
    // ���� ������ �������� ��� ������ �����.
    // �������� �������������� �������.
    CDataBase::SignatureCreate();
}

//-----------------------------------------------------------------------------------------------------
void CPss21::SendToDevicesReceipt(void)
{
    for (uint8_t i = 0;
            i < (m_xDeviceConfigSearch.uiModulesQuantity);
            i++)
    {
        m_apxDrivers[i] -> SetReceipt(1);
    }

    // ������� �������� � ������������ ������������.
    m_xModbusRtuLinkControl.SetFsmEvent(CModbusRtuLinkControl::RECEIPT_HAPPENED_FSM_EVENT);
}

//-----------------------------------------------------------------------------------------------------
void CPss21::SendToDevicesReset(void)
{
    for (uint8_t i = 0;
            i < (m_xDeviceConfigSearch.uiModulesQuantity);
            i++)
    {
        m_apxDrivers[i] -> SetReset(1);
    }
}

//-----------------------------------------------------------------------------------------------------
void CPss21::SendToDevicesModbusReceipt(void)
{
    for (uint8_t i = 0;
            i < (m_xDeviceConfigSearch.uiModulesQuantity);
            i++)
    {
        m_apxDrivers[i] -> SetModbusReceipt(1);
    }

    // ������� �������� � ������������ ������������.
    m_xModbusRtuLinkControl.SetFsmEvent(CModbusRtuLinkControl::RECEIPT_HAPPENED_FSM_EVENT);
}

//-----------------------------------------------------------------------------------------------------
void CPss21::SendToDevicesModbusReset(void)
{
    for (uint8_t i = 0;
            i < (m_xDeviceConfigSearch.uiModulesQuantity);
            i++)
    {
        m_apxDrivers[i] -> SetModbusReset(1);
    }
}

//-----------------------------------------------------------------------------------------------------
void CPss21::KeyStateProcessing(void)
{
//    m_xIsReceiptButtonPressed.Fsm();

//    m_xReceiptKey.Fsm();
//    m_xResetKey.Fsm();
//    m_xCheckKey.Fsm();
//    m_xExternalKey.Fsm();

//    if (m_xIsReceiptButtonPressed.IsSolved())
//    {
//        SetPanelReceipt(1);
//        SetDiscreteSignalsReceipt(1);
//        SendToDevicesReceipt();
//        m_xBuzzerNotifyerControl.AlarmSet(BEEP_SIGNAL);
//    }
    if (m_xReceiptKey.KeyEventHappened(CMultiFunctionKey::KEY_EVENT_PRESSED_PUSH))
    {
        SetPanelReceipt(1);
        SetDiscreteSignalsReceipt(1);
        SendToDevicesReceipt();
        m_xBuzzerNotifyerControl.AlarmSet(BEEP_SIGNAL);
    }

    if (m_xResetKey.KeyEventHappened(CMultiFunctionKey::KEY_EVENT_PRESSED_PUSH))
    {
        SetPanelReset(1);
        SetDiscreteSignalsReset(1);
        SendToDevicesReset();
        m_xBuzzerNotifyerControl.AlarmSet(BEEP_SIGNAL);
    }

    if (m_xCheckKey.KeyEventHappened(CMultiFunctionKey::KEY_EVENT_PRESSED_HOLD))
    {
        // ������� ����� �������� ����� ������������ ����.
        SetFsmState(MAP_CREATE_START);
        m_xBuzzerNotifyerControl.AlarmSet(BEEP_SIGNAL);
    }

    if (m_xCheckKey.KeyEventHappened(CMultiFunctionKey::KEY_EVENT_PRESSED_PUSH))
    {
        if (GetFsmState() == MAP_CREATE_ON)
        {
            // ��������� ����.
            SetFsmState(MAP_CREATE_NEXT_WINDOW);
            m_xBuzzerNotifyerControl.AlarmSet(BEEP_SIGNAL);
        }
    }

//    if (m_xCheckKey.KeyEventHappened(CMultiFunctionKey::KEY_EVENT_PRESSED_PUSH))
//    {
//        // ������� ����� ������������.
//        SetFsmState(TEST_START);
//        m_xBuzzerNotifyerControl.AlarmSet(BEEP_SIGNAL);
//    }

    // ������� ������� ������ - ������ �����?
    if(m_xDeviceConfiguration.ExtUnsetOnly)
    {
        if (m_xExternalKey.KeyEventHappened(CMultiFunctionKey::KEY_EVENT_PRESSED_RECEIPT))
        {
            SetExternalReset(1);
            SetDiscreteSignalsReset(1);
            SendToDevicesReset();
            m_xBuzzerNotifyerControl.AlarmSet(BEEP_SIGNAL);
        }

        if (m_xExternalKey.KeyEventHappened(CMultiFunctionKey::KEY_EVENT_PRESSED_CHECK))
        {
            // ������� ����� ������������.
            SetFsmState(TEST_START);
            m_xBuzzerNotifyerControl.AlarmSet(BEEP_SIGNAL);
        }
    }
    else
    {
        if (m_xExternalKey.KeyEventHappened(CMultiFunctionKey::KEY_EVENT_PRESSED_RECEIPT))
        {
            SetExternalReceipt(1);
            SetDiscreteSignalsReceipt(1);
            SendToDevicesReceipt();
            m_xBuzzerNotifyerControl.AlarmSet(BEEP_SIGNAL);
        }

        if (m_xExternalKey.KeyEventHappened(CMultiFunctionKey::KEY_EVENT_PRESSED_RESET))
        {
            SetExternalReset(1);
            SetDiscreteSignalsReset(1);
            SendToDevicesReset();
            m_xBuzzerNotifyerControl.AlarmSet(BEEP_SIGNAL);
        }

        if (m_xExternalKey.KeyEventHappened(CMultiFunctionKey::KEY_EVENT_PRESSED_CHECK))
        {
            // ������� ����� ������������.
            SetFsmState(TEST_START);
            m_xBuzzerNotifyerControl.AlarmSet(BEEP_SIGNAL);
        }
    }

    // ������ "������������" � "��������" ������ ������ 7 ������?
    if (m_xEventUniteLogicAndOne.IsHappened((m_xReceiptKey.KeyEventHappened(CMultiFunctionKey::KEY_EVENT_PRESSED_SET_DEFAULT)),
                                            (m_xCheckKey.KeyEventHappened(CMultiFunctionKey::KEY_EVENT_PRESSED_SET_DEFAULT))))
    {
        // ������������� ��������� Modbus �� ��������� �� ������� ������.
        CPss21::ModbusDefaultInit();
        m_xBuzzerNotifyerControl.AlarmSet(BEEP_SIGNAL);
    }
}

//-----------------------------------------------------------------------------------------------------
void CPss21::ReceiptResetGlobalFlagsClear(void)
{
    SetPanelReceipt(0);
    SetExternalReceipt(0);
    SetModbusReceipt(0);
    SetPanelReset(0);
    SetExternalReset(0);
    SetModbusReset(0);
}

//-----------------------------------------------------------------------------------------------------
uint8_t CPss21::ModulesInteraction(uint8_t uiModuleIndex)
{
    if (uiModuleIndex < (m_xDeviceConfigSearch.uiModulesQuantity))
    {
        m_apxDrivers[uiModuleIndex] -> DataExchange();
        return 0;
    }
    else
    {
        return 1;
    }
};

//-----------------------------------------------------------------------------------------------------
void CPss21::ConfigurationInit(void)
{
    uint8_t* puiTempBuffer = m_auiIntermediateBuff;
    uint8_t* puiAlarmDfaInit = m_auiReceiveMessageBuff;

    memset(puiAlarmDfaInit, 0, DISCRETE_SIGNALS_NUMBER);

    // ��������� ������������ ����������.
    // ������� �� ��������� ����� ���� �� - "������������ ����������".
    CDataBase::ReadBlock(puiTempBuffer, TDataBase::DEV_CONFIG);
    memcpy(reinterpret_cast<uint8_t*>(&m_xDeviceConfiguration),
           puiTempBuffer,
           sizeof(struct TDevConfig));

    // ��������� ��������� ���������� ModBus.
    // ������� �� ��������� ����� ���� �� - "��������� ���������� ModBus".
    CDataBase::ReadBlock(puiTempBuffer, TDataBase::MODBUS_SET);
    memcpy(reinterpret_cast<uint8_t*>(&m_xModbusSettings),
           puiTempBuffer,
           sizeof(struct TMBusSetting));

    // ������� �� ��������� ����� ���� �� - "�� ������� ������".
    CDataBase::ReadBlock(puiTempBuffer, TDataBase::OUT_BASE);
    memcpy(reinterpret_cast<uint8_t*>(&m_xDiscreteOutputDataBase),
           puiTempBuffer,
           sizeof(struct TOutputData));

    // ��������� ������� ����������� ������� ���������������� ��� ��������.
    // ������� �� ��������� ����� ���� �� - "������� ����������� ������� ���������������� ��� ��������".
    CDataBase::ReadBlock(puiTempBuffer, TDataBase::ACTIVITY_LEVEL);
    for (uint8_t i = 0;
            i < DISCRETE_INPUTS_NUMBER;
        )
    {
        uint8_t uiData = *puiTempBuffer;
        for (uint8_t j = 0;
                j < 8;
                j++)
        {
            // ��������� ������� �������� ����������� ������� ���������������� ��� ��������.
            if (((uiData >> j) & 0x01))
            {
                puiAlarmDfaInit[i] = 0x80;
            }
            else
            {
                puiAlarmDfaInit[i] = 0;
            }
            i++;
        }
        puiTempBuffer++;
    }

    puiTempBuffer = m_auiIntermediateBuff;
    // �������� ���� ������������ � ���������� ��������.
    // ������� �� ��������� ����� ���� �� - "������ ���� ������������ ������� ��������� � �������� ����������� �������".
    CDataBase::ReadBlock(puiTempBuffer, TDataBase::ALARM_WINDOW_INDEX);
    for (uint8_t i = 0;
            i < DISCRETE_SIGNALS_NUMBER;
            i++)
    {
        // �������� ���� ������������ � �������� ����������� �������.
        puiAlarmDfaInit[i] |= (puiTempBuffer[i] & ~(0x80));
    }

    // ������� �� ��������� ����� ���� �� - "��� ������������ ����������� � ����".
    CDataBase::ReadBlock(puiTempBuffer, TDataBase::ALARM_TYPE);
    for (uint8_t i = 0;
            i < DISCRETE_SIGNALS_NUMBER;
            i++)
    {
        uint8_t uiWindowIndex = (puiAlarmDfaInit[i] & ~(0x80));
        // ��� �������� ����������� ������� ����������������� ������������?
        if (uiWindowIndex < ALARM_WINDOWS_NUMBER)
        {
            // ������� ������ ������������ ����, ������ �� �����.
            // ������� ������� ����������� ������� ���������������� ��� ��������.
            puiAlarmDfaInit[i] &= 0x80;
            // ��������� ��� ������������ �� ������������ ����.
            puiAlarmDfaInit[i] |= puiTempBuffer[uiWindowIndex];
        }
        else
        {
            // ������� ������ ������������ ����, ������ �� �����.
            // ������� ������� ����������� ������� ���������������� ��� ��������.
            puiAlarmDfaInit[i] &= 0x80;
        }
    }

    // �������� ������� ��������� ��������� ������������ ���������� ��������.
    for (uint8_t i = 0;
            i < DISCRETE_SIGNALS_NUMBER;
            i++)
    {
        // ����� ��� ������������ ������ � ���������� ��������?
        switch (puiAlarmDfaInit[i] & 0x03)
        {
        case NORMAL:
            m_apxAlarmDfa[i] = new CNormalAlarmDfa;
            break;

        case INDICATION:
            // ������� ����������� ������� ���������������� ��� �������� - �������?
            if (puiAlarmDfaInit[i] & 0x80)
            {
                m_apxAlarmDfa[i] = new CIndicationAlarmHighLevelDfa;
            }
            else
            {
                m_apxAlarmDfa[i] = new CIndicationAlarmLowLevelDfa;
            }
            break;

        case PREVENTIVE:
            // ������� ����������� ������� ���������������� ��� �������� - �������?
            if (puiAlarmDfaInit[i] & 0x80)
            {
                m_apxAlarmDfa[i] = new CPreventiveAlarmHighLevelDfa;
            }
            else
            {
                m_apxAlarmDfa[i] = new CPreventiveAlarmLowLevelDfa;
            }
            break;

        case EMERGENCY:
            // ������� ����������� ������� ���������������� ��� �������� - �������?
            if (puiAlarmDfaInit[i] & 0x80)
            {
                m_apxAlarmDfa[i] = new CEmergencyAlarmHighLevelDfa;
            }
            else
            {
                m_apxAlarmDfa[i] = new CEmergencyAlarmLowLevelDfa;
            }
            break;

        case IND_PREVENTIVE:
            m_apxAlarmDfa[i] = new CNormalAlarmDfa;
            break;

        case IND_EMERGENCY:
            m_apxAlarmDfa[i] = new CNormalAlarmDfa;
            break;

        default:
            m_apxAlarmDfa[i] = new CNormalAlarmDfa;
            break;
        }
    }

    // �������� ���� ������������ � ��������� �������� ������������ ���������� ��������.
    // ������� �� ��������� ����� ���� �� - "������ ���� ������������ ������� ��������� � �������� ����������� �������".
    CDataBase::ReadBlock(puiTempBuffer, TDataBase::ALARM_WINDOW_INDEX);
    for (uint8_t i = 0;
            i < DISCRETE_SIGNALS_NUMBER;
            i++)
    {
        // ��� �������� ����������� ������� ����������������� ������������?
        if (puiTempBuffer[i] < ALARM_WINDOWS_NUMBER)
        {
            // �������� ���� ������������ � �������� ����������� �������.
            m_apxAlarmDfa[i] ->
            SetAlarmWindowIndex(puiTempBuffer[i]);
        }
        else
        {
            m_apxAlarmDfa[i] ->
            SetAlarmWindowIndex(0);
        }
    }

    // ������� �� ��������� ����� ���� �� - "�������� ����, �������������� ���������� ������".
    CDataBase::ReadBlock(puiTempBuffer, TDataBase::RELAY);
    // ������� ��������� ������ �� ��.
    for (uint8_t i = 0;
            i < DISCRETE_SIGNALS_NUMBER;
            i++)
    {
        *(m_apxAlarmDfa[i] -> GetLinkedDiscreteOutputsPointer()) = puiTempBuffer[i];
    };

    // ������� ������� ���������� ��������� ���������� �������� - ���������� ����� �������.
    for (uint8_t i = 0;
            i < DISCRETE_INPUTS_NUMBER;
            i++)
    {
        m_apxAlarmDfa[i] -> SetDiscreteStateIndex(i);
    };
    // ������� ������� ���������� ��������� ���������� �������� - ��������� �����(coils Modbus).
    for (uint8_t i = 0;
            i < ALARM_WINDOWS_NUMBER;
            i++)
    {
        m_apxAlarmDfa[DISCRETE_INPUTS_NUMBER + i] -> SetDiscreteStateIndex(DISCRETE_INPUTS_NUMBER + i);
//        m_apxAlarmDfa[DISCRETE_INPUTS_NUMBER + i] -> SetDiscreteStateIndex(i);
    };

    // �����������  ��������� ����������.
    // ������� �� ��������� ����� ���� �� - "��������� ���������� �������� � EEPROM".
    CDataBase::ReadBlock(puiTempBuffer, TDataBase::DEVICE_STATE);
    memcpy(reinterpret_cast<uint8_t*>(&m_xDeviceState),
           puiTempBuffer,
           sizeof(struct TDeviceState));

//    // �������� �������� ��������� ������.
//    uint8_t uiIndex = 0;
//
//    m_apxErrorAlarmDfa[uiIndex] = new CErrorAlarmDfa;
//    // �������� ���� ������������ � ���������� ������� ������������ ������.
//    m_apxErrorAlarmDfa[uiIndex] -> SetAlarmWindowIndex(IMD_ERROR);
//    // ��������� ������ ��������� ��������� ������.
//    m_apxErrorAlarmDfa[uiIndex] -> SetDiscreteStateIndex(IMD_ERROR);
//    uiIndex++;
//
//    m_apxErrorAlarmDfa[uiIndex] = new CErrorAlarmDfa;
//    // �������� ���� ������������ � ���������� ������� ������������ ������.
//    m_apxErrorAlarmDfa[uiIndex] -> SetAlarmWindowIndex(OMD_ERROR);
//    // ��������� ������ ��������� ��������� ������.
//    m_apxErrorAlarmDfa[uiIndex] -> SetDiscreteStateIndex(OMD_ERROR);
//    uiIndex++;
//
//    m_apxErrorAlarmDfa[uiIndex] = new CErrorAlarmDfa;
//    // �������� ���� ������������ � ���������� ������� ������������ ������.
//    m_apxErrorAlarmDfa[uiIndex] -> SetAlarmWindowIndex(CFG_ERROR);
//    // ��������� ������ ��������� ��������� ������.
//    m_apxErrorAlarmDfa[uiIndex] -> SetDiscreteStateIndex(CFG_ERROR);
//    uiIndex++;
//
//    m_apxErrorAlarmDfa[uiIndex] = new CErrorAlarmDfa;
//    // �������� ���� ������������ � ���������� ������� ������������ ������.
//    m_apxErrorAlarmDfa[uiIndex] -> SetAlarmWindowIndex(DB_ERROR);
//    // ��������� ������ ��������� ��������� ������.
//    m_apxErrorAlarmDfa[uiIndex] -> SetDiscreteStateIndex(DB_ERROR);
//    uiIndex++;
//
//    m_apxErrorAlarmDfa[uiIndex] = new CErrorAlarmDfa;
//    // �������� ���� ������������ � ���������� ������� ������������ ������.
//    m_apxErrorAlarmDfa[uiIndex] -> SetAlarmWindowIndex(MBS_ERROR);
//    // ��������� ������ ��������� ��������� ������.
//    m_apxErrorAlarmDfa[uiIndex] -> SetDiscreteStateIndex(MBS_ERROR);
//    uiIndex++;

    // �������� �������� ��������� ������.
    for (uint8_t i = 0;
            i < HANDLED_ERROR_NUMBER;
            i++)
    {
        m_apxErrorAlarmDfa[i] = new CErrorAlarmDfa;
        // �������� ���� ������������ � ���������� ������� ������������ ������.
        m_apxErrorAlarmDfa[i] -> SetAlarmWindowIndex(i);
        // ��������� ������ ��������� ��������� ������.
        m_apxErrorAlarmDfa[i] -> SetDiscreteStateIndex(i);
    };
}

//-----------------------------------------------------------------------------------------------------
void CPss21::DiscreteOutputControlClear(void)
{
    // ���������� ������� ���������� �������� ���������� �������(����) � �������� ���������.
    for (int j = 0;
            j < DISCRETE_OUTPUT_MODULE_MAX_NUMBER;
            j++)
    {
        // ���� ������ ����.
        for (int k = 0;
                k < MR_DISCRETE_OUTPUT_NUMBER;
                k++)
        {
            // ������� ���� - ����� ���������.
            m_axDiscreteOutputControl[(j * MR_DISCRETE_OUTPUT_NUMBER) + k].uiNewActivation = 0;
            // ������� ���� - ���������� ��������� ����.
            m_axDiscreteOutputControl[(j * MR_DISCRETE_OUTPUT_NUMBER) + k].uiRelayActivationRequest = 0;
        }
    }
}

//-----------------------------------------------------------------------------------------------------
void CPss21::DiscreteOutputsSet(uint8_t *puiLinkedDiscreteOutputs, uint8_t uiNewViolation)
{
    // ���������� ������ ��� ������� �� ���������� ����?
    if (!uiNewViolation)
    {
        // �� ����� ���������.
        // ��������� ����� - ���������� ���������, ��� ������������������� ����,
        // � ������ ������� ���������� ��������� ���� - ����������.
        // ���� ������ ������.
        for (int j = 0;
                j < DISCRETE_OUTPUT_MODULE_MAX_NUMBER;
                j++)
        {
            // ���� ������ ����.
            for (int k = 0;
                    k < MR_DISCRETE_OUTPUT_NUMBER;
                    k++)
            {
                // ������� - (j, k) ���������� �����(����) ��������?
                if (puiLinkedDiscreteOutputs[j] & (0x01 << k))
                {
                    // ��������� ���� - ���������� ��������� ����.
                    m_axDiscreteOutputControl[(j * MR_DISCRETE_OUTPUT_NUMBER) + k].uiRelayActivationRequest = 1;
                }
            }
        }
    }
    else
    {
        // ����� ���������.
        // ��������� ����� - ����� ���������, ��� ������������������� ����
        // � ������ ������� ���������� ���� - ����� ���������.
        // ��������� ����� - ���������� ���������, ��� ������������������� ����,
        // � ������ ������� ���������� ��������� ���� - ����������.
        // ���� ������ ������.
        for (int j = 0;
                j < DISCRETE_OUTPUT_MODULE_MAX_NUMBER;
                j++)
        {
            // ���� ������ ����.
            for (int k = 0;
                    k < MR_DISCRETE_OUTPUT_NUMBER;
                    k++)
            {
                // ������� - (j, k) ���������� �����(����) ��������?
                if (puiLinkedDiscreteOutputs[j] & (0x01 << k))
                {
                    // ��������� ���� - ����� ���������.
                    m_axDiscreteOutputControl[(j * MR_DISCRETE_OUTPUT_NUMBER) + k].uiNewActivation = 1;
                    // ��������� ���� - ���������� ��������� ����.
                    m_axDiscreteOutputControl[(j * MR_DISCRETE_OUTPUT_NUMBER) + k].uiRelayActivationRequest = 1;
                }
            }
        }
    }
}

//-----------------------------------------------------------------------------------------------------
void CPss21::AlarmsProcessing(void)
{
    // ���������� ��� ���������� �������.
    for (uint8_t i = 0;
            i < DISCRETE_SIGNALS_NUMBER;
            i++)
    {
        m_apxAlarmDfa[i] -> Fsm();
    }
}

//-----------------------------------------------------------------------------------------------------
void CPss21::ErrorAlarmsProcessing(void)
{
    // ���������� ��� ���������� �������.
    for (uint8_t i = 0;
            i < HANDLED_ERROR_NUMBER;
            i++)
    {
        m_apxErrorAlarmDfa[i] -> Fsm();
    }
}

//-----------------------------------------------------------------------------------------------------
uint8_t CPss21::GetDiscreteInputState(uint8_t uiIndex)
{
//    // �� ����� �� ������� ������� ���������� ��������?
//    if ((uiIndex >= 0) &&
//            (uiIndex < (DISCRETE_INPUTS_NUMBER + ALARM_WINDOWS_NUMBER)))
//    {
//        // ������� ��������� ����������� ������� - ���������� ����� �������.
//        return m_aucRtuDiscreteDataArray[DISCRETE_INPUTS_BIT_ARRAY_OFFSET + uiIndex];
//    }
//    else
//    {
//        // ������� ��������� ����������� ������� - ���������� ����� �������, ���� 0.
//        return m_aucRtuDiscreteDataArray[DISCRETE_INPUTS_BIT_ARRAY_OFFSET];
//    }

    if (uiIndex < DISCRETE_INPUTS_NUMBER)
    {
        // ������� ��������� ����������� ������� - ���������� ����� �������.
        return m_aucRtuDiscreteDataArray[DISCRETE_INPUTS_BIT_ARRAY_OFFSET + uiIndex];
    }
    else
    {
        // ������� ��������� ����������� ������� - ��������� �����(coils Modbus).
        return m_aucRtuDiscreteDataArray[ALARM_WINDOWS_BIT_ARRAY_OFFSET + (uiIndex - DISCRETE_INPUTS_NUMBER)];
    }
}

//-----------------------------------------------------------------------------------------------------
void CPss21::SetErrorAlarmState(uint8_t uiIndex, uint8_t uiData)
{
    // ��������� ��������� ��������� ������.
    m_auiErrorAlarmDataArray[uiIndex] = uiData;
}

//-----------------------------------------------------------------------------------------------------
uint8_t CPss21::GetErrorAlarmState(uint8_t uiIndex)
{
    // ������� ��������� ��������� ������.
    return m_auiErrorAlarmDataArray[uiIndex];
}

//-----------------------------------------------------------------------------------------------------
void CPss21::AlarmTypeSet(uint8_t uiAlarmType)
{
    m_xPreventiveAlarmWindowNotifyerControl.AlarmSet(uiAlarmType);
    m_xEmergencyAlarmWindowNotifyerControl.AlarmSet(uiAlarmType);
    m_xStatusLedNotifyerControl.AlarmSet(uiAlarmType);
    m_xBuzzerNotifyerControl.AlarmSet(uiAlarmType);
}

//-----------------------------------------------------------------------------------------------------
void CPss21::AlarmTypeChange(void)
{
    if (m_uiCommonAlarmType != m_uiCommonAlarmTypePrevious)
    {
        m_uiCommonAlarmTypePrevious = m_uiCommonAlarmType;
        AlarmTypeSet(m_uiCommonAlarmType);
    }
}

//-----------------------------------------------------------------------------------------------------
void CPss21::AlarmTypeErrorChange(void)
{
    if (m_uiCommonAlarmType != m_uiCommonAlarmTypePrevious)
    {
        m_uiCommonAlarmTypePrevious = m_uiCommonAlarmType;

        m_xPreventiveAlarmWindowNotifyerControl.AlarmSet(INDICATION);
        m_xEmergencyAlarmWindowNotifyerControl.AlarmSet(INDICATION);
        m_xStatusLedNotifyerControl.AlarmSet(ERROR);
        m_xBuzzerNotifyerControl.AlarmSet(ERROR);
    }
}

//-----------------------------------------------------------------------------------------------------
void CPss21::AlarmTypeReceipt(void)
{
    m_uiCommonAlarmType = INDICATION;
    m_uiCommonAlarmTypePrevious = INDICATION;
    AlarmTypeSet(INDICATION);
}

//-----------------------------------------------------------------------------------------------------
void CPss21::AlarmTypeReset(void)
{
    m_uiCommonAlarmType = NORMAL;
    m_uiCommonAlarmTypePrevious = NORMAL;
    AlarmTypeSet(NORMAL);
}

//-----------------------------------------------------------------------------------------------------
void CPss21::NotifyersControlProcessing(void)
{
    m_xPreventiveAlarmWindowNotifyerControl.Fsm();
    m_xEmergencyAlarmWindowNotifyerControl.Fsm();
    m_xStatusLedNotifyerControl.Fsm();
    m_xBuzzerNotifyerControl.Fsm();
}

//-----------------------------------------------------------------------------------------------------
void CPss21::SetAlarmWindowType(uint8_t uiAlarmWindowIndex, uint8_t uiAlarmType)
{
    GetAlarmWindowControlPointer(uiAlarmWindowIndex) -> SetAlarmType(uiAlarmType);
}

//-----------------------------------------------------------------------------------------------------
void CPss21::SaveContextNotifyerControl(void)
{
//    // �������� ������� ��������� ��������� ���������� ������������.
//    m_xPreventiveAlarmWindowNotifyerControl.SetSavedFsmState(m_xPreventiveAlarmWindowNotifyerControl.GetFsmState());
//    m_xEmergencyAlarmWindowNotifyerControl.SetSavedFsmState(m_xEmergencyAlarmWindowNotifyerControl.GetFsmState());
//    m_xStatusLedNotifyerControl.SetSavedFsmState(m_xStatusLedNotifyerControl.GetFsmState());
//    m_xBuzzerNotifyerControl.SetSavedFsmState(m_xBuzzerNotifyerControl.GetFsmState());

    // �������� ������� ��������� ����������� - ���� ������������.
    for (uint8_t i = 0;
            i < ALARM_WINDOWS_NUMBER;
            i++)
    {
        m_axAlarmWindowControl[i].SetSavedAlarmType(m_axAlarmWindowControl[i].GetAlarmType());
        m_axAlarmWindowControl[i].SetSavedActivityState(m_axAlarmWindowControl[i].GetActivityState());
    };
}

//-----------------------------------------------------------------------------------------------------
void CPss21::RestoreContextNotifyerControl(void)
{
//    // ����������� ���������� ��������� ��������� ���������� ������������.
//    m_xPreventiveAlarmWindowNotifyerControl.SetFsmState(m_xPreventiveAlarmWindowNotifyerControl.GetSavedFsmState());
//    m_xEmergencyAlarmWindowNotifyerControl.SetFsmState(m_xEmergencyAlarmWindowNotifyerControl.GetSavedFsmState());
//    m_xStatusLedNotifyerControl.SetFsmState(m_xStatusLedNotifyerControl.GetSavedFsmState());
//    m_xBuzzerNotifyerControl.SetFsmState(m_xBuzzerNotifyerControl.GetSavedFsmState());
    AlarmTypeSet(GetCommonAlarmType());

    // ����������� ���������� ��������� ����������� - ���� ������������.
    for (uint8_t i = 0;
            i < ALARM_WINDOWS_NUMBER;
            i++)
    {
        m_axAlarmWindowControl[i].SetAlarmType(m_axAlarmWindowControl[i].GetSavedAlarmType());
        m_axAlarmWindowControl[i].SetActivityState(m_axAlarmWindowControl[i].GetSavedActivityState());
    };
}

//-----------------------------------------------------------------------------------------------------
void CPss21::DiscreteSignalsProcessing(void)
{
    DiscreteOutputControlClear();
    AlarmsProcessing();
    ErrorAlarmsProcessing();
    SetDiscreteSignalsReceipt(0);
    SetDiscreteSignalsReset(0);
}

//-----------------------------------------------------------------------------------------------------
void CPss21::BoardWindowsUpdate(void)
{
    CLightBoard::Set(m_axAlarmWindowControl);
}

//-----------------------------------------------------------------------------------------------------
void CPss21::ActiveAlarmWindowOn(uint8_t uiAlarmType)
{
    for (uint8_t i = 0;
            i < ALARM_WINDOWS_NUMBER;
            i++)
    {
        // ��� ������������ ���� � ��� ������������ ���������� ���������?
        if (m_axAlarmWindowControl[i].GetAlarmType() == uiAlarmType)
        {
            // ������������ ���� ������������, ��� ����������� �����������.
            m_axAlarmWindowControl[i].SetActivityState(1);
        }

//        CPss21::m_aucRtuHoldingRegistersArray[i] = m_axAlarmWindowControl[i].GetActivityState();
    };

    BoardWindowsUpdate();
}

//-----------------------------------------------------------------------------------------------------
void CPss21::ActiveAlarmWindowOff(uint8_t uiAlarmType)
{
    for (uint8_t i = 0;
            i < ALARM_WINDOWS_NUMBER;
            i++)
    {
        // ��� ������������ ���� � ��� ������������ ���������� ���������?
        if (m_axAlarmWindowControl[i].GetAlarmType() == uiAlarmType)
        {
            // ������������ ���� ������������, ��� ����������� ����������� �����������.
            m_axAlarmWindowControl[i].SetActivityState(0);
        }
        else if (m_axAlarmWindowControl[i].GetAlarmType() == NORMAL)
        {
            // ������������ ���� ������������, ��� ����������� ����������� �����������.
            m_axAlarmWindowControl[i].SetActivityState(0);
        }

//        CPss21::m_aucRtuHoldingRegistersArray[i] = m_axAlarmWindowControl[i].GetActivityState();
    };

    BoardWindowsUpdate();
}

//-----------------------------------------------------------------------------------------------------
void CPss21::AllAlarmWindowOn(uint8_t uiAlarmType)
{
//    CAlarmWindow axAlarmWindowControl[ALARM_WINDOWS_NUMBER];

    for (uint8_t i = 0;
            i < ALARM_WINDOWS_NUMBER;
            i++)
    {
        // ������������ ���� ������������, ��� ����������� �����������.
        m_axAlarmWindowControl[i].SetAlarmType(uiAlarmType);
        // ������������ ���� ������������, ��� ����������� �����������.
        m_axAlarmWindowControl[i].SetActivityState(1);
    };

    BoardWindowsUpdate();
}

//-----------------------------------------------------------------------------------------------------
void CPss21::ErrorWindowOn(uint8_t uiIndex)
{
    // ���������� ���� ������������ - ������.
    for (uint8_t i = 0;
            i < ALARM_WINDOWS_NUMBER;
            i++)
    {
        if (m_auiErrorLedOn[i])
        {
            // ������������ ���� ������������, ��� ����������� �����������.
            m_axAlarmWindowControl[i].SetAlarmType(INDICATION);
            m_axAlarmWindowControl[i].SetActivityState(1);
        }
    };

    if (uiIndex < ALARM_WINDOWS_NUMBER)
    {
        m_axAlarmWindowControl[uiIndex].SetAlarmType(NORMAL);
        m_axAlarmWindowControl[uiIndex].SetActivityState(0);
    }

    BoardWindowsUpdate();
}

//-----------------------------------------------------------------------------------------------------
void CPss21::AllAlarmWindowOff(void)
{
    for (uint8_t i = 0;
            i < ALARM_WINDOWS_NUMBER;
            i++)
    {
        // ������������ ���� ������������, ��� ����������� ����������� �����������.
        m_axAlarmWindowControl[i].SetAlarmType(NORMAL);
        // ������������ ���� ������������, ��� ����������� ����������� �����������.
        m_axAlarmWindowControl[i].SetActivityState(0);
    };

    BoardWindowsUpdate();
}

//-----------------------------------------------------------------------------------------------------
void CPss21::TestMainCycle(void)
{
    CPss21::m_xModbusRtuOne.Fsm();

    CPss21::KeyStateProcessing();

    NotifyersControlProcessing();

    CPss21::ReceiptResetGlobalFlagsClear();

    CPlatform::WatchdogReset();
}

//-----------------------------------------------------------------------------------------------------
void CPss21::ErrorMainCycle(void)
{
    CPss21::m_xModbusRtuOne.Fsm();

    CPss21::KeyStateProcessing();

    // ���� ������� - "�����������"?
    if ((GetPanelReceipt()) ||
            (GetExternalReceipt()) ||
            (GetModbusReceipt()))
    {
//        m_xPreventiveAlarmWindowNotifyerControl.AlarmSet(INDICATION);
//        m_xEmergencyAlarmWindowNotifyerControl.AlarmSet(INDICATION);
        m_xStatusLedNotifyerControl.AlarmSet(NORMAL);
        m_xBuzzerNotifyerControl.AlarmSet(NORMAL);
    }

    NotifyersControlProcessing();

    CPss21::ReceiptResetGlobalFlagsClear();

    CPlatform::WatchdogReset();
}

//-----------------------------------------------------------------------------------------------------
void CPss21::LinkControlErrorMainCycle(void)
{
    CPss21::m_xModbusRtuOne.Fsm();
    CPss21::m_xModbusRtuLinkControl.Fsm();

    CPss21::KeyStateProcessing();

    NotifyersControlProcessing();

    CPss21::ReceiptResetGlobalFlagsClear();

    CDataStore::Fsm();

    CPlatform::WatchdogReset();
}
uint8_t uiMapTestCounter = 0;
//-----------------------------------------------------------------------------------------------------
void CPss21::MainFsm(void)
{
    switch (GetFsmState())
    {
    case START:
        CPlatform::WatchdogEnable();
        CPlatform::Init();
        CPss21::Init();
        CBuzzer::Init();
        if (!(CDataBase::Check()))
        {
            SetFsmState(ERROR_START);
        }
        else
        {
            CPss21::ConfigurationInit();
            CPss21::ModbusInit();
            CSpi::Init();
            CPss21::AddressBusInit();
            CPss21::SearchModules();
            CPss21::CreateDevices();
            m_xBuzzerNotifyerControl.AlarmSet(BEEP_SIGNAL);
            SetFsmState(MAIN_CYCLE_START_WAITING);
        }
        break;

    case RESTART:
        break;

    case MAIN_CYCLE_START_WAITING:
        CPss21::m_xModbusRtuOne.Fsm();

        CPss21::KeyStateProcessing();

        NotifyersControlProcessing();

        CDataStore::Fsm();

        if (CPss21::m_xMainCycleTimer.IsOverflow())
        {
            CPss21::m_xMainCycleTimer.Reset();
            CPlatform::WatchdogReset();
            SetModuleIndex(0);
            SetFsmState(MAIN_CYCLE_MODULES_INTERACTION);
        }

        break;

    case MAIN_CYCLE_MODULES_INTERACTION:
        CPss21::m_xModbusRtuOne.Fsm();
        CPss21::m_xModbusRtuLinkControl.Fsm();

        CPss21::KeyStateProcessing();

        NotifyersControlProcessing();

        CDataStore::Fsm();

        if (CPss21::ModulesInteraction(GetModuleIndex()))
        {
            SetFsmState(MAIN_CYCLE_DISCRETE_SIGNALS_PROCESSING);
        }
        else
        {
            SetModuleIndex(GetModuleIndex() + 1);
        }

        switch (GetErrorCode())
        {
//        case IMD_ERROR:
//        case OMD_ERROR:
        case CFG_ERROR:
        case DB_ERROR:
            SetFsmState(ERROR_START);
            break;

        case MBS_ERROR:
            SetFsmState(LINK_CONTROL_ERROR_START);
            break;

        default:
            break;
        }
        break;

    case MAIN_CYCLE_DISCRETE_SIGNALS_PROCESSING:
        CPss21::DiscreteSignalsProcessing();
        SetFsmState(MAIN_CYCLE_END);
        break;

    case MAIN_CYCLE_END:
        CPss21::ReceiptResetGlobalFlagsClear();
        SetFsmState(MAIN_CYCLE_START_WAITING);
        break;

//-----------------------------------------------------------------------------------------------------
    case MAP_CREATE_START:
        uiMapTestCounter = 0;
        SetFsmState(MAP_CREATE_WINDOW_ON);

        CPlatform::WatchdogReset();
        break;

    case MAP_CREATE_WINDOW_ON:
        CPss21::m_aucRtuHoldingRegistersArray[0] = uiMapTestCounter + 1;
        if (uiMapTestCounter != 48)
        {
            CPss21::AllAlarmWindowOff();
            // ������������ ���� ������������, ��� ����������� �����������.
            m_axAlarmWindowControl[uiMapTestCounter].SetAlarmType(INDICATION);
            // ������������ ���� ������������, ��� ����������� �����������.
            m_axAlarmWindowControl[uiMapTestCounter].SetActivityState(1);
            BoardWindowsUpdate();
            SetFsmState(MAP_CREATE_ON);
        }
        else
        {
            SetFsmState(MAP_CREATE_START);
        }

        CPlatform::WatchdogReset();
        break;

    case MAP_CREATE_ON:
        TestMainCycle();
        break;

    case MAP_CREATE_NEXT_WINDOW:
        uiMapTestCounter++;
        SetFsmState(MAP_CREATE_WINDOW_ON);

        CPlatform::WatchdogReset();
        break;


//-----------------------------------------------------------------------------------------------------
    case TEST_START:
        SaveContextNotifyerControl();
        AlarmTypeSet(PREVENTIVE);
        CPss21::AllAlarmWindowOn(PREVENTIVE);
        CPss21::m_xMainCycleTimer.Set(TEST_ON_TIME());
        SetFsmState(TEST_ON);

        CPlatform::WatchdogReset();
        break;

    case TEST_ON:
        if (CPss21::m_xMainCycleTimer.IsOverflow())
        {
            CPss21::AllAlarmWindowOff();
            RestoreContextNotifyerControl();
            CPss21::m_xMainCycleTimer.Set(MAIN_CYCLE_PERIOD_TIME);
            SetFsmState(MAIN_CYCLE_START_WAITING);
        }

        TestMainCycle();
        break;

//-----------------------------------------------------------------------------------------------------
    case ERROR_START:
        // ������������� ��������� Modbus �� ��������� �� ������� ������.
        ModbusDefaultInit();
        ErrorWindowOn(GetErrorCode() - 1);
        SetErrorCode(0);

        m_xPreventiveAlarmWindowNotifyerControl.AlarmSet(INDICATION);
        m_xEmergencyAlarmWindowNotifyerControl.AlarmSet(INDICATION);
        m_xStatusLedNotifyerControl.AlarmSet(ERROR);
        m_xBuzzerNotifyerControl.AlarmSet(ERROR);

        SetFsmState(ERROR_ON);

        CPlatform::WatchdogReset();
        break;

    case ERROR_ON:
        ErrorMainCycle();
        break;

//-----------------------------------------------------------------------------------------------------
    case LINK_CONTROL_ERROR_START:
        SaveContextNotifyerControl();
        ErrorWindowOn(GetErrorCode() - 1);
        SetErrorCode(0);

        m_xPreventiveAlarmWindowNotifyerControl.AlarmSet(INDICATION);
        m_xEmergencyAlarmWindowNotifyerControl.AlarmSet(INDICATION);
        m_xStatusLedNotifyerControl.AlarmSet(ERROR);
        m_xBuzzerNotifyerControl.AlarmSet(ERROR);

        CPlatform::WatchdogReset();
        SetFsmState(LINK_CONTROL_ERROR_ON);
        break;

    case LINK_CONTROL_ERROR_ON:
        LinkControlErrorMainCycle();
        break;

    case LINK_CONTROL_ERROR_OFF:
        CPss21::AllAlarmWindowOff();
        RestoreContextNotifyerControl();
        CPss21::m_xMainCycleTimer.Set(MAIN_CYCLE_PERIOD_TIME);

        CPlatform::WatchdogReset();
        SetFsmState(MAIN_CYCLE_START_WAITING);
        break;

//-----------------------------------------------------------------------------------------------------
    case PROGRAMMING_START:
        // ����� ����� ���������� ����� ���� ������ ��������� ������������
        // ����� "PROGRAMMING_TIME + WDTO_2S", ����� ��������� ��������� ������������.
        CPss21::m_xMainCycleTimer.Set(PROGRAMMING_TIME);
        SetFsmState(PROGRAMMING_ON);

        CPlatform::WatchdogReset();
        break;

    case PROGRAMMING_ON:
        CPss21::m_xModbusRtuOne.Fsm();

        CDataStore::Fsm();

        if (CPss21::m_xMainCycleTimer.IsOverflow())
        {
            SetFsmState(REBOOT);
        }

        CPlatform::WatchdogReset();
        break;

    case REBOOT:
        // ����� "WDTO_2S" - ������������, ����� ��������� ��������� ������������.
        CPss21::m_xModbusRtuOne.Fsm();

        CDataStore::Fsm();
        break;

//-----------------------------------------------------------------------------------------------------
    default:
        break;
    }
}

