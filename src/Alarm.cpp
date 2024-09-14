//-----------------------------------------------------------------------------------------------------
//  Source      : FileName.cpp
//  Created     : 01.06.2022
//  Author      : Alexandr Volvenkin
//  email       : aav-36@mail.ru
//  GitHub      : https://github.com/AlexandrVolvenkin
//-----------------------------------------------------------------------------------------------------
#include "Dfa.h"
#include "Platform.h"
#include "Pss21.h"

//-----------------------------------------------------------------------------------------------------
CAlarmDfa::CAlarmDfa()
{
    //ctor
}

//-----------------------------------------------------------------------------------------------------
CAlarmDfa::~CAlarmDfa()
{
    //dtor
}

//-----------------------------------------------------------------------------------------------------
// ������� ��������� ������������ ����������� �������.
void CAlarmDfa::Fsm(void)
{
    uint8_t uiDiscreteSignalState = DISCRETE_SIGNAL_IS_INVALID;

    // ���������� ���� ������������?
    if ((CPss21::GetDiscreteInputsBadState(GetDiscreteStateIndex()) & 0x03) == INPUT_IS_INVALID)
    {
        uiDiscreteSignalState = DISCRETE_SIGNAL_IS_INVALID;
    }
    else
    {
        // ��� �������� ����������� ������� namur?
        if (IS_NAMUR_ON())
        {
            switch (CPss21::GetDiscreteInputsBadState(GetDiscreteStateIndex()))
            {
            case OFF:
            case ON:
                // ���������� ������ �������?
                // ������� ������� ���� "NAMUR" ���������� �� ���������� ����������� �����:
//	- ��������� "0" (OFF) - "����������"
//	- ��������� "1" (ON) - "��������"
                if ((CPss21::GetDiscreteInputsBadState(GetDiscreteStateIndex()) & 0x03) == OFF)
                {
                    // ��������� �������� �������� ����������� ����� ������������ � ������������ ������
                    // ������������������ � ������ ������� ����������.
                    CPss21::SetDiscreteInputState(GetDiscreteStateIndex(), 0);

                    if (ACTIVE_LEVEL())
                    {
                        uiDiscreteSignalState = DISCRETE_SIGNAL_IS_ACTIVE;
                    }
                    else
                    {
                        uiDiscreteSignalState = DISCRETE_SIGNAL_IS_NOT_ACTIVE;
                    }
                }
                else if ((CPss21::GetDiscreteInputsBadState(GetDiscreteStateIndex()) & 0x03) == ON)
                {
                    // ��������� �������� �������� ����������� ����� ������������ � ������������ ������
                    // ������������������ � ������ ������� ����������.
                    CPss21::SetDiscreteInputState(GetDiscreteStateIndex(), 1);

                    if (ACTIVE_LEVEL())
                    {
                        uiDiscreteSignalState = DISCRETE_SIGNAL_IS_NOT_ACTIVE;
                    }
                    else
                    {
                        uiDiscreteSignalState = DISCRETE_SIGNAL_IS_ACTIVE;
                    }
                }
                break;

            case WIRE_BREAK:
            case SHORT_CIRCUIT:
                uiDiscreteSignalState = DISCRETE_SIGNAL_IS_NAMUR_ERROR;
                // ��������� �������� �������� ����������� ����� ������������ � ������������ ������
                // ������������������ � ������ ������� ����������.
                CPss21::SetDiscreteInputState(GetDiscreteStateIndex(), 0);
                break;

            default:
                break;
            }
        }
        else
        {
            // ���������� ������ �������?
            // ������� ������� ���� "����� �������" (��) ���������� �� ���������� ����������� �����:
            //        1.3. � ��������� ��������� �������� ���� "����� �������" (��) ������ ����:
//	- ��������� "0" (OFF) � "3" (��) - "��������"
//	- ��������� "1" (ON) � "2" (�����) - "����������"
            if (((CPss21::GetDiscreteInputsBadState(GetDiscreteStateIndex()) & 0x03) == WIRE_BREAK) ||
                    ((CPss21::GetDiscreteInputsBadState(GetDiscreteStateIndex()) & 0x03) == ON))
            {
                // ��������� �������� �������� ����������� ����� ������������ � ������������ ������
                // ������������������ � ������ ������� ����������.
                CPss21::SetDiscreteInputState(GetDiscreteStateIndex(), 0);

                if (ACTIVE_LEVEL())
                {
                    uiDiscreteSignalState = DISCRETE_SIGNAL_IS_ACTIVE;
                }
                else
                {
                    uiDiscreteSignalState = DISCRETE_SIGNAL_IS_NOT_ACTIVE;
                }
            }
            else if (((CPss21::GetDiscreteInputsBadState(GetDiscreteStateIndex()) & 0x03) == SHORT_CIRCUIT) ||
                     ((CPss21::GetDiscreteInputsBadState(GetDiscreteStateIndex()) & 0x03) == OFF))
            {
                // ��������� �������� �������� ����������� ����� ������������ � ������������ ������
                // ������������������ � ������ ������� ����������.
                CPss21::SetDiscreteInputState(GetDiscreteStateIndex(), 1);

                if (ACTIVE_LEVEL())
                {
                    uiDiscreteSignalState = DISCRETE_SIGNAL_IS_NOT_ACTIVE;
                }
                else
                {
                    uiDiscreteSignalState = DISCRETE_SIGNAL_IS_ACTIVE;
                }
            }
        }
    }

    switch (GetFsmState())
    {
    case START:
        break;

//    case ACTIVE_STATE_CHECK:
//        break;

    case ACTIVE_STATE_WAITING:
//        CPss21::m_aucRtuHoldingRegistersArray[0] = CPss21::m_aucDiscreteInputsBadState[0];
        // ��� �������� ����������� ������� namur?
        if (IS_NAMUR_ON())
        {
            // ���������� ������ NAMUR � ������ ��� ��?
            if (uiDiscreteSignalState == DISCRETE_SIGNAL_IS_NAMUR_ERROR)
            {
                SetFsmState(NAMUR_STATE_ON);
            }
            else
            {
                // ���������� ������ �������?
                if (uiDiscreteSignalState == DISCRETE_SIGNAL_IS_ACTIVE)
                {
                    // ��������� ��������� ���������� ������ - ����� ���������.
                    CPss21::DiscreteOutputsSet(GetLinkedDiscreteOutputsPointer(), NEW_VIOLATION);
                    // ��������� ��� ������������ ���������� ���� � ������� ���������� ������ ����������.
                    CPss21::SetAlarmWindowType(GetAlarmWindowIndex(), ALARM_TYPE());
                    CPss21::SetAlarmWindowColor(GetAlarmWindowIndex(), ALARM_TYPE());
                    CPss21::GetAlarmWindowControlPointer(GetAlarmWindowIndex()) -> SetActivityState(1);
                    // ���������������� ����� ��������������� ������ ���������������� � ���������
                    // ������������ ��� �������� ����������� ������� � ���������� ���������?
                    if (CPss21::m_xDeviceConfiguration.AutoUnset)
                    {
                        SetFsmState(RECEIPT_OR_RESET_OR_AUTOUNSET_WAITING);
                    }
                    else
                    {
                        SetFsmState(RECEIPT_OR_RESET_WAITING);
                    }
                }
            }
        }
        else
        {
            // ���������� ������ �������?
            // ������� ������� ���� "����� �������" (��) ���������� �� ���������� ����������� �����:
            // 2 - �����(����������), � 3 - ��(��������).
            if (uiDiscreteSignalState == DISCRETE_SIGNAL_IS_ACTIVE)
            {
                // ��������� ��������� ���������� ������ - ����� ���������.
                CPss21::DiscreteOutputsSet(GetLinkedDiscreteOutputsPointer(), NEW_VIOLATION);
                // ��������� ��� ������������ ���������� ���� � ������� ���������� ������ ����������.
                CPss21::SetAlarmWindowType(GetAlarmWindowIndex(), ALARM_TYPE());
                CPss21::SetAlarmWindowColor(GetAlarmWindowIndex(), ALARM_TYPE());
                CPss21::GetAlarmWindowControlPointer(GetAlarmWindowIndex()) -> SetActivityState(1);
                // ���������������� ����� ��������������� ������ ���������������� � ���������
                // ������������ ��� �������� ����������� ������� � ���������� ���������?
                if (CPss21::m_xDeviceConfiguration.AutoUnset)
                {
                    SetFsmState(RECEIPT_OR_RESET_OR_AUTOUNSET_WAITING);
                }
                else
                {
                    SetFsmState(RECEIPT_OR_RESET_WAITING);
                }
            }
        }
        break;

    case RECEIPT_OR_RESET_OR_AUTOUNSET_WAITING:
        // ���������������� ����� ��������������� ������ ���������������� � ���������
        // ������������ ��� �������� ����������� ������� � ���������� ���������.
        // ���������� ������ �������?
        if (uiDiscreteSignalState == DISCRETE_SIGNAL_IS_ACTIVE)
        {
            // ��������� ��������� ���������� ������ - �� ����� ���������.
            CPss21::DiscreteOutputsSet(GetLinkedDiscreteOutputsPointer(), NOT_NEW_VIOLATION);
        }
        else if (uiDiscreteSignalState == DISCRETE_SIGNAL_IS_NOT_ACTIVE)
        {
            // ��������� ��� ������������ ���������� ���� � ������� ���������� ������ ����������.
            CPss21::SetAlarmWindowType(GetAlarmWindowIndex(), NORMAL);
            CPss21::GetAlarmWindowControlPointer(GetAlarmWindowIndex()) -> SetActivityState(0);
            SetFsmState(ACTIVE_STATE_WAITING);
            break;
        }

        // ������� ��������?
        if (CPss21::GetDiscreteSignalsReset())
        {
            // ��������� ��� ������������ ���������� ���� � ������� ���������� ������ ����������.
            CPss21::SetAlarmWindowType(GetAlarmWindowIndex(), INDICATION);
            SetFsmState(RESETED_NOT_ACTIVE_STATE_WAITING);
        }
        // ������� �����������?
        else if (CPss21::GetDiscreteSignalsReceipt())
        {
            // ��������� ��� ������������ ���������� ���� � ������� ���������� ������ ����������.
            CPss21::SetAlarmWindowType(GetAlarmWindowIndex(), INDICATION);
            SetFsmState(RECEIPTED_RESET_OR_NOT_ACTIVE_STATE_WAITING);
        }
        break;

    case RECEIPT_OR_RESET_WAITING:
        // ���������� ������ �������?
        if (uiDiscreteSignalState == DISCRETE_SIGNAL_IS_ACTIVE)
        {
            // ��������� ��������� ���������� ������ - �� ����� ���������.
            CPss21::DiscreteOutputsSet(GetLinkedDiscreteOutputsPointer(), NOT_NEW_VIOLATION);
        }

        // ������� ��������?
        if (CPss21::GetDiscreteSignalsReset())
        {
            // ��������� ��� ������������ ���������� ���� � ������� ���������� ������ ����������.
            CPss21::SetAlarmWindowType(GetAlarmWindowIndex(), INDICATION);
            SetFsmState(RESETED_NOT_ACTIVE_STATE_WAITING);
        }
        // ������� �����������?
        else if (CPss21::GetDiscreteSignalsReceipt())
        {
            // ��������� ��� ������������ ���������� ���� � ������� ���������� ������ ����������.
            CPss21::SetAlarmWindowType(GetAlarmWindowIndex(), INDICATION);
            SetFsmState(RECEIPTED_RESET_OR_NOT_ACTIVE_STATE_WAITING);
        }
        break;

    case RECEIPTED_RESET_OR_NOT_ACTIVE_STATE_WAITING:
        // ������� ��������?
        if (CPss21::GetDiscreteSignalsReset())
        {
            SetFsmState(RESETED_NOT_ACTIVE_STATE_WAITING);
        }
        // ���������� ������ �� �������?
        else if (uiDiscreteSignalState == DISCRETE_SIGNAL_IS_NOT_ACTIVE)
        {
            SetFsmState(RECEIPTED_RESET_WAITING);
        }
        else if (uiDiscreteSignalState == DISCRETE_SIGNAL_IS_ACTIVE)
        {
            // ��������� ��������� ���������� ������ - �� ����� ���������.
            CPss21::DiscreteOutputsSet(GetLinkedDiscreteOutputsPointer(), NOT_NEW_VIOLATION);
        }
        break;

    case RESETED_NOT_ACTIVE_STATE_WAITING:
        // ���������� ������ �� �������?
        if (uiDiscreteSignalState == DISCRETE_SIGNAL_IS_NOT_ACTIVE)
        {
            // ��������� ��� ������������ ���������� ���� � ������� ���������� ������ ����������.
            CPss21::SetAlarmWindowType(GetAlarmWindowIndex(), NORMAL);
            CPss21::GetAlarmWindowControlPointer(GetAlarmWindowIndex()) -> SetActivityState(0);
            SetFsmState(ACTIVE_STATE_WAITING);
        }
        else if (uiDiscreteSignalState == DISCRETE_SIGNAL_IS_ACTIVE)
        {
            // ��������� ��������� ���������� ������ - �� ����� ���������.
            CPss21::DiscreteOutputsSet(GetLinkedDiscreteOutputsPointer(), NOT_NEW_VIOLATION);
        }
        break;

    case RECEIPTED_RESET_WAITING:
        // ���������� ������ �������?
        if (uiDiscreteSignalState == DISCRETE_SIGNAL_IS_ACTIVE)
        {
            // ��������� ��������� ���������� ������ - �� ����� ���������.
            CPss21::DiscreteOutputsSet(GetLinkedDiscreteOutputsPointer(), NOT_NEW_VIOLATION);
        }
        else if (uiDiscreteSignalState == DISCRETE_SIGNAL_IS_NAMUR_ERROR)
        {
            SetFsmState(NAMUR_STATE_ON);
        }

        // ������� ��������?
        if (CPss21::GetDiscreteSignalsReset())
        {
            // ��������� ��� ������������ ���������� ���� � ������� ���������� ������ ����������.
            CPss21::SetAlarmWindowType(GetAlarmWindowIndex(), NORMAL);
            CPss21::GetAlarmWindowControlPointer(GetAlarmWindowIndex()) -> SetActivityState(0);
            SetFsmState(ACTIVE_STATE_WAITING);
        }
        break;

    case NAMUR_STATE_ON:
        // ��������� ��� ������������ ���������� ���� � ������� ���������� ������ ����������.
        CPss21::SetAlarmWindowType(GetAlarmWindowIndex(), INDICATION);
        CPss21::SetAlarmWindowColor(GetAlarmWindowIndex(), NAMUR_INDICATION);
        CPss21::GetAlarmWindowControlPointer(GetAlarmWindowIndex()) -> SetActivityState(1);
//        CPss21::BoardWindowsUpdate();
        SetFsmState(NAMUR_INPUT_CORRECT_STATE_WAITING);
        break;

    case NAMUR_INPUT_CORRECT_STATE_WAITING:
        // ���������� ������ NAMUR ����������?
        if (uiDiscreteSignalState != DISCRETE_SIGNAL_IS_NAMUR_ERROR)
        {
            // ��������� ��� ������������ ���������� ���� � ������� ���������� ������ ����������.
            CPss21::SetAlarmWindowType(GetAlarmWindowIndex(), NORMAL);
            CPss21::GetAlarmWindowControlPointer(GetAlarmWindowIndex()) -> SetActivityState(0);
//            CPss21::BoardWindowsUpdate();
            SetFsmState(ACTIVE_STATE_WAITING);
        }
        break;

    default:
        break;
    }
}
//-----------------------------------------------------------------------------------------------------








//-----------------------------------------------------------------------------------------------------
CNormalAlarmDfa::CNormalAlarmDfa()
{
    SetFsmState(START);
}

//-----------------------------------------------------------------------------------------------------
CNormalAlarmDfa::~CNormalAlarmDfa()
{
    //dtor
}

//-----------------------------------------------------------------------------------------------------








//-----------------------------------------------------------------------------------------------------
CPreventiveAlarmLowLevelDfa::CPreventiveAlarmLowLevelDfa()
{
    SetFsmState(ACTIVE_STATE_WAITING);
}

//-----------------------------------------------------------------------------------------------------
CPreventiveAlarmLowLevelDfa::~CPreventiveAlarmLowLevelDfa()
{
    //dtor
}

//-----------------------------------------------------------------------------------------------------
CPreventiveAlarmHighLevelDfa::CPreventiveAlarmHighLevelDfa()
{
    SetFsmState(ACTIVE_STATE_WAITING);
}

//-----------------------------------------------------------------------------------------------------
CPreventiveAlarmHighLevelDfa::~CPreventiveAlarmHighLevelDfa()
{
    //dtor
}

//-----------------------------------------------------------------------------------------------------
CPreventiveAlarmLowLevelNamurDfa::CPreventiveAlarmLowLevelNamurDfa()
{
    SetFsmState(ACTIVE_STATE_WAITING);
}

//-----------------------------------------------------------------------------------------------------
CPreventiveAlarmLowLevelNamurDfa::~CPreventiveAlarmLowLevelNamurDfa()
{
    //dtor
}

//-----------------------------------------------------------------------------------------------------
CPreventiveAlarmHighLevelNamurDfa::CPreventiveAlarmHighLevelNamurDfa()
{
    SetFsmState(ACTIVE_STATE_WAITING);
}

//-----------------------------------------------------------------------------------------------------
CPreventiveAlarmHighLevelNamurDfa::~CPreventiveAlarmHighLevelNamurDfa()
{
    //dtor
}
//-----------------------------------------------------------------------------------------------------







//-----------------------------------------------------------------------------------------------------
CEmergencyAlarmLowLevelDfa::CEmergencyAlarmLowLevelDfa()
{
    SetFsmState(ACTIVE_STATE_WAITING);
}

//-----------------------------------------------------------------------------------------------------
CEmergencyAlarmLowLevelDfa::~CEmergencyAlarmLowLevelDfa()
{
    //dtor
}

//-----------------------------------------------------------------------------------------------------
CEmergencyAlarmHighLevelDfa::CEmergencyAlarmHighLevelDfa()
{
    SetFsmState(ACTIVE_STATE_WAITING);
}

//-----------------------------------------------------------------------------------------------------
CEmergencyAlarmHighLevelDfa::~CEmergencyAlarmHighLevelDfa()
{
    //dtor
}


//-----------------------------------------------------------------------------------------------------
CEmergencyAlarmLowLevelNamurDfa::CEmergencyAlarmLowLevelNamurDfa()
{
    SetFsmState(ACTIVE_STATE_WAITING);
}

//-----------------------------------------------------------------------------------------------------
CEmergencyAlarmLowLevelNamurDfa::~CEmergencyAlarmLowLevelNamurDfa()
{
    //dtor
}

//-----------------------------------------------------------------------------------------------------
CEmergencyAlarmHighLevelNamurDfa::CEmergencyAlarmHighLevelNamurDfa()
{
    SetFsmState(ACTIVE_STATE_WAITING);
}

//-----------------------------------------------------------------------------------------------------
CEmergencyAlarmHighLevelNamurDfa::~CEmergencyAlarmHighLevelNamurDfa()
{
    //dtor
}
//-----------------------------------------------------------------------------------------------------








//-----------------------------------------------------------------------------------------------------
CIndicationAlarmLowLevelDfa::CIndicationAlarmLowLevelDfa()
{
    SetFsmState(ACTIVE_STATE_WAITING);
}

//-----------------------------------------------------------------------------------------------------
CIndicationAlarmLowLevelDfa::~CIndicationAlarmLowLevelDfa()
{
    //dtor
}

//-----------------------------------------------------------------------------------------------------
// ������� ��������� ������������ ����������� �������.
void CIndicationAlarmLowLevelDfa::Fsm(void)
{
    uint8_t uiDiscreteSignalState = DISCRETE_SIGNAL_IS_INVALID;

    // ���������� ���� ������������?
    if ((CPss21::GetDiscreteInputsBadState(GetDiscreteStateIndex()) & 0x03) == INPUT_IS_INVALID)
    {
        uiDiscreteSignalState = DISCRETE_SIGNAL_IS_INVALID;
    }
    else
    {
        // ��� �������� ����������� ������� namur?
        if (IS_NAMUR_ON())
        {
            switch (CPss21::GetDiscreteInputsBadState(GetDiscreteStateIndex()))
            {
            case OFF:
            case ON:
                // ���������� ������ �������?
                // ������� ������� ���� "NAMUR" ���������� �� ���������� ����������� �����:
//	- ��������� "0" (OFF) - "����������"
//	- ��������� "1" (ON) - "��������"
                if ((CPss21::GetDiscreteInputsBadState(GetDiscreteStateIndex()) & 0x03) == OFF)
                {
                    // ��������� �������� �������� ����������� ����� ������������ � ������������ ������
                    // ������������������ � ������ ������� ����������.
                    CPss21::SetDiscreteInputState(GetDiscreteStateIndex(), 0);

                    if (ACTIVE_LEVEL())
                    {
                        uiDiscreteSignalState = DISCRETE_SIGNAL_IS_ACTIVE;
                    }
                    else
                    {
                        uiDiscreteSignalState = DISCRETE_SIGNAL_IS_NOT_ACTIVE;
                    }
                }
                else if ((CPss21::GetDiscreteInputsBadState(GetDiscreteStateIndex()) & 0x03) == ON)
                {
                    // ��������� �������� �������� ����������� ����� ������������ � ������������ ������
                    // ������������������ � ������ ������� ����������.
                    CPss21::SetDiscreteInputState(GetDiscreteStateIndex(), 1);

                    if (ACTIVE_LEVEL())
                    {
                        uiDiscreteSignalState = DISCRETE_SIGNAL_IS_NOT_ACTIVE;
                    }
                    else
                    {
                        uiDiscreteSignalState = DISCRETE_SIGNAL_IS_ACTIVE;
                    }
                }
                break;

            case WIRE_BREAK:
            case SHORT_CIRCUIT:
                uiDiscreteSignalState = DISCRETE_SIGNAL_IS_NAMUR_ERROR;
                // ��������� �������� �������� ����������� ����� ������������ � ������������ ������
                // ������������������ � ������ ������� ����������.
                CPss21::SetDiscreteInputState(GetDiscreteStateIndex(), 0);
                break;

            default:
                break;
            }
        }
        else
        {
            // ���������� ������ �������?
            // ������� ������� ���� "����� �������" (��) ���������� �� ���������� ����������� �����:
            //        1.3. � ��������� ��������� �������� ���� "����� �������" (��) ������ ����:
//	- ��������� "0" (OFF) � "3" (��) - "��������"
//	- ��������� "1" (ON) � "2" (�����) - "����������"
            if (((CPss21::GetDiscreteInputsBadState(GetDiscreteStateIndex()) & 0x03) == WIRE_BREAK) ||
                    ((CPss21::GetDiscreteInputsBadState(GetDiscreteStateIndex()) & 0x03) == ON))
            {
                // ��������� �������� �������� ����������� ����� ������������ � ������������ ������
                // ������������������ � ������ ������� ����������.
                CPss21::SetDiscreteInputState(GetDiscreteStateIndex(), 0);

                if (ACTIVE_LEVEL())
                {
                    uiDiscreteSignalState = DISCRETE_SIGNAL_IS_ACTIVE;
                }
                else
                {
                    uiDiscreteSignalState = DISCRETE_SIGNAL_IS_NOT_ACTIVE;
                }
            }
            else if (((CPss21::GetDiscreteInputsBadState(GetDiscreteStateIndex()) & 0x03) == SHORT_CIRCUIT) ||
                     ((CPss21::GetDiscreteInputsBadState(GetDiscreteStateIndex()) & 0x03) == OFF))
            {
                // ��������� �������� �������� ����������� ����� ������������ � ������������ ������
                // ������������������ � ������ ������� ����������.
                CPss21::SetDiscreteInputState(GetDiscreteStateIndex(), 1);

                if (ACTIVE_LEVEL())
                {
                    uiDiscreteSignalState = DISCRETE_SIGNAL_IS_NOT_ACTIVE;
                }
                else
                {
                    uiDiscreteSignalState = DISCRETE_SIGNAL_IS_ACTIVE;
                }
            }
        }
    }

    switch (GetFsmState())
    {
    case START:
        break;

    case ACTIVE_STATE_WAITING:
//        CPss21::m_aucRtuHoldingRegistersArray[0] = CPss21::m_aucDiscreteInputsBadState[0];
        // ��� �������� ����������� ������� namur?
        if (IS_NAMUR_ON())
        {
            // ���������� ������ NAMUR � ������ ��� ��?
            if (uiDiscreteSignalState == DISCRETE_SIGNAL_IS_NAMUR_ERROR)
            {
                SetFsmState(NAMUR_STATE_ON);
            }
            else
            {
                // ���������� ������ �������?
                if (uiDiscreteSignalState == DISCRETE_SIGNAL_IS_ACTIVE)
                {
                    // ��������� ��������� ���������� ������ - ����� ���������.
                    CPss21::DiscreteOutputsSet(GetLinkedDiscreteOutputsPointer(), NEW_VIOLATION);
                    // ��������� ��� ������������ ���������� ���� � ������� ���������� ������ ����������.
                    CPss21::SetAlarmWindowType(GetAlarmWindowIndex(), ALARM_TYPE());
                    CPss21::SetAlarmWindowColor(GetAlarmWindowIndex(), ALARM_TYPE());
                    // ������������ ���� ������������, ��� ����������� �����������.
                    CPss21::GetAlarmWindowControlPointer(GetAlarmWindowIndex()) -> SetActivityState(1);
                    SetFsmState(NOT_ACTIVE_STATE_WAITING);
                }
            }
        }
        else
        {
            // ���������� ������ �������?
            // ������� ������� ���� "����� �������" (��) ���������� �� ���������� ����������� �����:
            // 2 - �����(����������), � 3 - ��(��������).
            if (uiDiscreteSignalState == DISCRETE_SIGNAL_IS_ACTIVE)
            {
                // ��������� ��������� ���������� ������ - ����� ���������.
                CPss21::DiscreteOutputsSet(GetLinkedDiscreteOutputsPointer(), NEW_VIOLATION);
                // ��������� ��� ������������ ���������� ���� � ������� ���������� ������ ����������.
                CPss21::SetAlarmWindowType(GetAlarmWindowIndex(), ALARM_TYPE());
                CPss21::SetAlarmWindowColor(GetAlarmWindowIndex(), ALARM_TYPE());
                // ������������ ���� ������������, ��� ����������� �����������.
                CPss21::GetAlarmWindowControlPointer(GetAlarmWindowIndex()) -> SetActivityState(1);
                SetFsmState(NOT_ACTIVE_STATE_WAITING);
            }
        }

        break;

    case NOT_ACTIVE_STATE_WAITING:
        // ��� �������� ����������� ������� namur?
        if (IS_NAMUR_ON())
        {
            if (uiDiscreteSignalState == DISCRETE_SIGNAL_IS_NAMUR_ERROR)
            {
                SetFsmState(NAMUR_STATE_ON);
            }
            else
            {
                // ���������� ������ �� �������?
                if (uiDiscreteSignalState == DISCRETE_SIGNAL_IS_NOT_ACTIVE)
                {
                    // ��������� ��� ������������ ���������� ���� � ������� ���������� ������ ����������.
                    CPss21::SetAlarmWindowType(GetAlarmWindowIndex(), NORMAL);
                    // ������������ ���� ������������, ��� ����������� ����������� �����������.
                    CPss21::GetAlarmWindowControlPointer(GetAlarmWindowIndex()) -> SetActivityState(0);
                    SetFsmState(ACTIVE_STATE_WAITING);
                }
                else if (uiDiscreteSignalState == DISCRETE_SIGNAL_IS_ACTIVE)
                {
                    // ��������� ��������� ���������� ������ - �� ����� ���������.
                    CPss21::DiscreteOutputsSet(GetLinkedDiscreteOutputsPointer(), NOT_NEW_VIOLATION);
                }
            }
        }
        else
        {
            // ���������� ������ �� �������?
            if (uiDiscreteSignalState == DISCRETE_SIGNAL_IS_NOT_ACTIVE)
            {
                // ��������� ��� ������������ ���������� ���� � ������� ���������� ������ ����������.
                CPss21::SetAlarmWindowType(GetAlarmWindowIndex(), NORMAL);
                // ������������ ���� ������������, ��� ����������� ����������� �����������.
                CPss21::GetAlarmWindowControlPointer(GetAlarmWindowIndex()) -> SetActivityState(0);
                SetFsmState(ACTIVE_STATE_WAITING);
            }
            else if (uiDiscreteSignalState == DISCRETE_SIGNAL_IS_ACTIVE)
            {
                // ��������� ��������� ���������� ������ - �� ����� ���������.
                CPss21::DiscreteOutputsSet(GetLinkedDiscreteOutputsPointer(), NOT_NEW_VIOLATION);
            }
        }
        break;

    case NAMUR_STATE_ON:
        // ��������� ��� ������������ ���������� ���� � ������� ���������� ������ ����������.
        CPss21::SetAlarmWindowType(GetAlarmWindowIndex(), INDICATION);
        CPss21::SetAlarmWindowColor(GetAlarmWindowIndex(), NAMUR_INDICATION);
        CPss21::GetAlarmWindowControlPointer(GetAlarmWindowIndex()) -> SetActivityState(1);
        SetFsmState(NAMUR_INPUT_CORRECT_STATE_WAITING);
        break;

    case NAMUR_INPUT_CORRECT_STATE_WAITING:
        // ���������� ������ NAMUR ����������?
        if (uiDiscreteSignalState != DISCRETE_SIGNAL_IS_NAMUR_ERROR)
        {
            // ��������� ��� ������������ ���������� ���� � ������� ���������� ������ ����������.
            CPss21::SetAlarmWindowType(GetAlarmWindowIndex(), NORMAL);
            CPss21::GetAlarmWindowControlPointer(GetAlarmWindowIndex()) -> SetActivityState(0);
            SetFsmState(ACTIVE_STATE_WAITING);
        }
        break;

    default:
        break;
    }
}
//-----------------------------------------------------------------------------------------------------








//-----------------------------------------------------------------------------------------------------
CIndicationAlarmHighLevelDfa::CIndicationAlarmHighLevelDfa()
{
    SetFsmState(ACTIVE_STATE_WAITING);
}

//-----------------------------------------------------------------------------------------------------
CIndicationAlarmHighLevelDfa::~CIndicationAlarmHighLevelDfa()
{
    //dtor
}
//-----------------------------------------------------------------------------------------------------


//-----------------------------------------------------------------------------------------------------
CIndicationAlarmLowLevelNamurDfa::CIndicationAlarmLowLevelNamurDfa()
{
    SetFsmState(ACTIVE_STATE_WAITING);
}

//-----------------------------------------------------------------------------------------------------
CIndicationAlarmLowLevelNamurDfa::~CIndicationAlarmLowLevelNamurDfa()
{
    //dtor
}
//-----------------------------------------------------------------------------------------------------
CIndicationAlarmHighLevelNamurDfa::CIndicationAlarmHighLevelNamurDfa()
{
    SetFsmState(ACTIVE_STATE_WAITING);
}

//-----------------------------------------------------------------------------------------------------
CIndicationAlarmHighLevelNamurDfa::~CIndicationAlarmHighLevelNamurDfa()
{
    //dtor
}







//-----------------------------------------------------------------------------------------------------
CErrorAlarmDfa::CErrorAlarmDfa()
{
    SetFsmState(ACTIVE_STATE_WAITING);
}

//-----------------------------------------------------------------------------------------------------
CErrorAlarmDfa::~CErrorAlarmDfa()
{
    //dtor
}

//-----------------------------------------------------------------------------------------------------
// ������� ��������� ��������� ������.
void CErrorAlarmDfa::Fsm(void)
{
    switch (GetFsmState())
    {
    case START:
        break;

    case ACTIVE_STATE_WAITING:
        // ���������� ������ �������?
        if (CPss21::GetErrorAlarmState(GetDiscreteStateIndex()) ^ ACTIVE_LEVEL())
        {
            // ��� ������������������� ������������ ����������� ������� ����� ����� ������� ���������,
            // ��� ��� ����� ������������?
            if ((CPss21::GetCommonAlarmType() != ERROR) &&
                    (CPss21::GetCommonAlarmType() < ALARM_TYPE()))
            {
                // ��������� ��� ����� ������������.
                CPss21::SetCommonAlarmType(ALARM_TYPE());

                // ������� ��� ����� ������������.
                CPss21::AlarmTypeErrorChange();
            }

            CPss21::SetAlarmWindowColor(GetAlarmWindowIndex(), ALARM_TYPE());
//            // ��������� ��� ������������ ���������� ���� � ������� ���������� ������ ����������.
            CPss21::ErrorWindowOn(GetAlarmWindowIndex());
            SetFsmState(RECEIPT_WAITING);
        }
        break;

    case RECEIPT_WAITING:
        // ������� �����������?
        if (CPss21::GetDiscreteSignalsReceipt())
        {
            // �������� �������� ������������ � ��������� - �����������.
            CPss21::AlarmTypeReceipt();
            SetFsmState(RECEIPTED_NOT_ACTIVE_STATE_WAITING);
        }
        break;

    case RECEIPTED_NOT_ACTIVE_STATE_WAITING:
        // ���������� ������ �� �������?
        if (!(CPss21::GetErrorAlarmState(GetDiscreteStateIndex()) ^ ACTIVE_LEVEL()))
        {
            // �������� �������� ������������ � ��������� - ��������.
            CPss21::AlarmTypeReset();
            CPss21::AllAlarmWindowOff();
            SetFsmState(ACTIVE_STATE_WAITING);
        }
        break;

    default:
        break;
    }
}
//-----------------------------------------------------------------------------------------------------









////-----------------------------------------------------------------------------------------------------
//CInputModuleErrorAlarmDfa::CInputModuleErrorAlarmDfa()
//{
//    SetFsmState(START);
//}
//
////-----------------------------------------------------------------------------------------------------
//CInputModuleErrorAlarmDfa::~CInputModuleErrorAlarmDfa()
//{
//    //dtor
//}
//
////-----------------------------------------------------------------------------------------------------
//
//
//
//
//
//
//
//
////-----------------------------------------------------------------------------------------------------
//COutpuModuleErrorAlarmDfa::COutpuModuleErrorAlarmDfa()
//{
//    SetFsmState(START);
//}
//
////-----------------------------------------------------------------------------------------------------
//COutpuModuleErrorAlarmDfa::~COutpuModuleErrorAlarmDfa()
//{
//    //dtor
//}
//
////-----------------------------------------------------------------------------------------------------







