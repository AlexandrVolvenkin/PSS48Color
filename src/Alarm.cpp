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
// Автомат обработки сигнализации дискретного сигнала.
void CAlarmDfa::Fsm(void)
{
    bool bIsDiscreteInputStateActive = false;
    // тип текущего дискретного сигнала namur?
    if (IS_NAMUR_ON())
    {
        switch (CPss21::GetDiscreteInputsBadState(GetDiscreteStateIndex()))
        {
        case OFF:
        case ON:
            // Дискретный сигнал активен?
            // уровень сигналы типа "namur"  определяем по состояниям дискретного входа:
            // 0 - OFF(разомкнуто), и 1 - ON(замкнуто).
            if ((CPss21::GetDiscreteInputsBadState(GetDiscreteStateIndex()) & 0x01) ^ ACTIVE_LEVEL())
            {
                bIsDiscreteInputStateActive = true;
                // установим значение текущего дискретного входа отображаемое в пространстве модбас
                // интерпретированное с учетом условия активности.
                CPss21::SetDiscreteInputState(GetDiscreteStateIndex(), 1);
            }
            else
            {
                bIsDiscreteInputStateActive = false;
                // установим значение текущего дискретного входа отображаемое в пространстве модбас
                // интерпретированное с учетом условия активности.
                CPss21::SetDiscreteInputState(GetDiscreteStateIndex(), 0);
            }
            break;

        case WIRE_BREAK:
        case SHORT_CIRCUIT:
            bIsDiscreteInputStateActive = false;
            // установим значение текущего дискретного входа отображаемое в пространстве модбас
            // интерпретированное с учетом условия активности.
            CPss21::SetDiscreteInputState(GetDiscreteStateIndex(), 0);
            break;

        default:
            break;
        }
    }
    else
    {
        // Дискретный сигнал активен?
        // уровень сигналы типа "СУХОЙ КОНТАКТ" (СК) определяем по состояниям дискретного входа:
        // 2 - обрыв(разомкнуто), и 3 - КЗ(замкнуто).
        if (((CPss21::GetDiscreteInputsBadState(GetDiscreteStateIndex()) >> 1) & 0x01) ^ ACTIVE_LEVEL())
        {
            bIsDiscreteInputStateActive = true;
            // установим значение текущего дискретного входа отображаемое в пространстве модбас
            // интерпретированное с учетом условия активности.
            CPss21::SetDiscreteInputState(GetDiscreteStateIndex(), 1);
        }
        else
        {
            bIsDiscreteInputStateActive = false;
            // установим значение текущего дискретного входа отображаемое в пространстве модбас
            // интерпретированное с учетом условия активности.
            CPss21::SetDiscreteInputState(GetDiscreteStateIndex(), 0);
        }
    }

    switch (GetFsmState())
    {
    case START:
        break;

//    case ACTIVE_STATE_CHECK:
//        break;

    case ACTIVE_STATE_WAITING:
        // тип текущего дискретного сигнала namur?
        if (IS_NAMUR_ON())
        {
            switch (CPss21::GetDiscreteInputsBadState(GetDiscreteStateIndex()))
            {
            // дискретный сигнал достоверен
            case OFF:
            case ON:
                // Дискретный сигнал активен?
//                if ((CPss21::GetDiscreteInputsBadState(GetDiscreteStateIndex()) & 0x01) ^ ACTIVE_LEVEL())
                if (bIsDiscreteInputStateActive)
                {
//                    // установим значение текущего дискретного входа отображаемое в пространстве модбас
//                    // интерпретированное с учетом условия активности.
//                    CPss21::SetDiscreteInputState(GetDiscreteStateIndex(), 1);
                    // Установим связанные дискретный выходы - новое нарушение.
                    CPss21::DiscreteOutputsSet(GetLinkedDiscreteOutputsPointer(), NEW_VIOLATION);
                    // Установим тип сигнализации связанному окну в массиве управления окнами извещателя.
                    CPss21::SetAlarmWindowType(GetAlarmWindowIndex(), ALARM_TYPE());
                    CPss21::SetAlarmWindowColor(GetAlarmWindowIndex(), ALARM_TYPE());
                    // Запрограммирован режим автоматического сброса предупредительой и аварийной
                    // сигнализации при переходе дискретного сигнала в неактивное состояние?
                    if (CPss21::m_xDeviceConfiguration.AutoUnset)
                    {
                        SetFsmState(RECEIPT_OR_RESET_OR_AUTOUNSET_WAITING);
                    }
                    else
                    {
                        SetFsmState(RECEIPT_OR_RESET_WAITING);
                    }
                }
//                else
//                {
//                    // установим значение текущего дискретного входа отображаемое в пространстве модбас
//                    // интерпретированное с учетом условия активности.
//                    CPss21::SetDiscreteInputState(GetDiscreteStateIndex(), 0);
//                }
                break;

            // дискретный сигнал не достоверен
            case WIRE_BREAK:
            case SHORT_CIRCUIT:
//                // установим значение текущего дискретного входа отображаемое в пространстве модбас
//                // интерпретированное с учетом условия активности.
//                CPss21::SetDiscreteInputState(GetDiscreteStateIndex(), 0);
                // Установим тип сигнализации связанному окну в массиве управления окнами извещателя.
                CPss21::SetAlarmWindowType(GetAlarmWindowIndex(), INDICATION);
                CPss21::SetAlarmWindowColor(GetAlarmWindowIndex(), NAMUR_INDICATION);
                SetFsmState(NAMUR_INPUT_CORRECT_STATE_WAITING);
                break;

            default:
                break;
            }
        }
        else
        {
            // Дискретный сигнал активен?
            // уровень сигналы типа "СУХОЙ КОНТАКТ" (СК) определяем по состояниям дискретного входа:
            // 2 - обрыв(разомкнуто), и 3 - КЗ(замкнуто).
            //            if (((CPss21::GetDiscreteInputsBadState(GetDiscreteStateIndex()) >> 1) & 0x01) ^ ACTIVE_LEVEL())
            if (bIsDiscreteInputStateActive)
            {
//                // установим значение текущего дискретного входа отображаемое в пространстве модбас
//                // интерпретированное с учетом условия активности.
//                CPss21::SetDiscreteInputState(GetDiscreteStateIndex(), 1);
                // Установим связанные дискретный выходы - новое нарушение.
                CPss21::DiscreteOutputsSet(GetLinkedDiscreteOutputsPointer(), NEW_VIOLATION);
                // Установим тип сигнализации связанному окну в массиве управления окнами извещателя.
                CPss21::SetAlarmWindowType(GetAlarmWindowIndex(), ALARM_TYPE());
                CPss21::SetAlarmWindowColor(GetAlarmWindowIndex(), ALARM_TYPE());
                // Запрограммирован режим автоматического сброса предупредительой и аварийной
                // сигнализации при переходе дискретного сигнала в неактивное состояние?
                if (CPss21::m_xDeviceConfiguration.AutoUnset)
                {
                    SetFsmState(RECEIPT_OR_RESET_OR_AUTOUNSET_WAITING);
                }
                else
                {
                    SetFsmState(RECEIPT_OR_RESET_WAITING);
                }
            }
//            else
//            {
//                // установим значение текущего дискретного входа отображаемое в пространстве модбас
//                // интерпретированное с учетом условия активности.
//                CPss21::SetDiscreteInputState(GetDiscreteStateIndex(), 0);
//            }
        }
        break;

    case RECEIPT_OR_RESET_OR_AUTOUNSET_WAITING:
        // Запрограммирован режим автоматического сброса предупредительой и аварийной
        // сигнализации при переходе дискретного сигнала в неактивное состояние.
        // Дискретный сигнал активен?
//        if (CPss21::GetDiscreteInputState(GetDiscreteStateIndex()) ^ ACTIVE_LEVEL())
        if (bIsDiscreteInputStateActive)
        {
            // Установим связанные дискретный выходы - не новое нарушение.
            CPss21::DiscreteOutputsSet(GetLinkedDiscreteOutputsPointer(), NOT_NEW_VIOLATION);
        }
        else
        {
            // Установим тип сигнализации связанному окну в массиве управления окнами извещателя.
            CPss21::SetAlarmWindowType(GetAlarmWindowIndex(), NORMAL);
            SetFsmState(ACTIVE_STATE_WAITING);
            break;
        }

        // Событие сброшено?
        if (CPss21::GetDiscreteSignalsReset())
        {
            // Установим тип сигнализации связанному окну в массиве управления окнами извещателя.
            CPss21::SetAlarmWindowType(GetAlarmWindowIndex(), INDICATION);
            SetFsmState(RESETED_NOT_ACTIVE_STATE_WAITING);
        }
        // Событие квитировано?
        else if (CPss21::GetDiscreteSignalsReceipt())
        {
            // Установим тип сигнализации связанному окну в массиве управления окнами извещателя.
            CPss21::SetAlarmWindowType(GetAlarmWindowIndex(), INDICATION);
            SetFsmState(RECEIPTED_RESET_OR_NOT_ACTIVE_STATE_WAITING);
        }
        break;

    case RECEIPT_OR_RESET_WAITING:
        // Дискретный сигнал активен?
//        if (CPss21::GetDiscreteInputState(GetDiscreteStateIndex()) ^ ACTIVE_LEVEL())
        if (bIsDiscreteInputStateActive)
        {
            // Установим связанные дискретный выходы - не новое нарушение.
            CPss21::DiscreteOutputsSet(GetLinkedDiscreteOutputsPointer(), NOT_NEW_VIOLATION);
        }

        // Событие сброшено?
        if (CPss21::GetDiscreteSignalsReset())
        {
            // Установим тип сигнализации связанному окну в массиве управления окнами извещателя.
            CPss21::SetAlarmWindowType(GetAlarmWindowIndex(), INDICATION);
            SetFsmState(RESETED_NOT_ACTIVE_STATE_WAITING);
        }
        // Событие квитировано?
        else if (CPss21::GetDiscreteSignalsReceipt())
        {
            // Установим тип сигнализации связанному окну в массиве управления окнами извещателя.
            CPss21::SetAlarmWindowType(GetAlarmWindowIndex(), INDICATION);
            SetFsmState(RECEIPTED_RESET_OR_NOT_ACTIVE_STATE_WAITING);
        }
        break;

    case RECEIPTED_RESET_OR_NOT_ACTIVE_STATE_WAITING:
        // Событие сброшено?
        if (CPss21::GetDiscreteSignalsReset())
        {
            SetFsmState(RESETED_NOT_ACTIVE_STATE_WAITING);
        }
        // Дискретный сигнал не активен?
        else if (!(bIsDiscreteInputStateActive))//if (!(CPss21::GetDiscreteInputState(GetDiscreteStateIndex()) ^ ACTIVE_LEVEL()))
        {
            SetFsmState(RECEIPTED_RESET_WAITING);
        }
        else
        {
            // Установим связанные дискретный выходы - не новое нарушение.
            CPss21::DiscreteOutputsSet(GetLinkedDiscreteOutputsPointer(), NOT_NEW_VIOLATION);
        }
        break;

    case RESETED_NOT_ACTIVE_STATE_WAITING:
        // Дискретный сигнал не активен?
//        if (!(CPss21::GetDiscreteInputState(GetDiscreteStateIndex()) ^ ACTIVE_LEVEL()))
        if (!(bIsDiscreteInputStateActive))
        {
            // Установим тип сигнализации связанному окну в массиве управления окнами извещателя.
            CPss21::SetAlarmWindowType(GetAlarmWindowIndex(), NORMAL);
            SetFsmState(ACTIVE_STATE_WAITING);
        }
        else
        {
            // Установим связанные дискретный выходы - не новое нарушение.
            CPss21::DiscreteOutputsSet(GetLinkedDiscreteOutputsPointer(), NOT_NEW_VIOLATION);
        }
        break;

    case RECEIPTED_RESET_WAITING:
        // Дискретный сигнал активен?
        //        if (CPss21::GetDiscreteInputState(GetDiscreteStateIndex()) ^ ACTIVE_LEVEL())
        if (bIsDiscreteInputStateActive)
        {
            // Установим связанные дискретный выходы - не новое нарушение.
            CPss21::DiscreteOutputsSet(GetLinkedDiscreteOutputsPointer(), NOT_NEW_VIOLATION);
        }

        // Событие сброшено?
        if (CPss21::GetDiscreteSignalsReset())
        {
            // Установим тип сигнализации связанному окну в массиве управления окнами извещателя.
            CPss21::SetAlarmWindowType(GetAlarmWindowIndex(), NORMAL);
            SetFsmState(ACTIVE_STATE_WAITING);
        }
        break;

    case NAMUR_INPUT_CORRECT_STATE_WAITING:
        // тип текущего дискретного сигнала namur?
        if (IS_NAMUR_ON())
        {
            switch (CPss21::GetDiscreteInputsBadState(GetDiscreteStateIndex()))
            {
            // дискретный сигнал достоверен
            case OFF:
            case ON:
                // Установим тип сигнализации связанному окну в массиве управления окнами извещателя.
                CPss21::SetAlarmWindowType(GetAlarmWindowIndex(), NORMAL);
                SetFsmState(ACTIVE_STATE_WAITING);
                break;

            default:
                break;
            }
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
// Автомат обработки сигнализации дискретного сигнала.
void CIndicationAlarmLowLevelDfa::Fsm(void)
{
    switch (GetFsmState())
    {
    case START:
        break;

    case ACTIVE_STATE_WAITING:
        // Дискретный сигнал активен?
        if (CPss21::GetDiscreteInputState(GetDiscreteStateIndex()) ^ ACTIVE_LEVEL())
        {
            // Установим связанные дискретный выходы - новое нарушение.
            CPss21::DiscreteOutputsSet(GetLinkedDiscreteOutputsPointer(), NEW_VIOLATION);
            // Установим тип сигнализации связанному окну в массиве управления окнами извещателя.
            CPss21::SetAlarmWindowType(GetAlarmWindowIndex(), ALARM_TYPE());
            CPss21::SetAlarmWindowColor(GetAlarmWindowIndex(), ALARM_TYPE());
            // Активизируем окно сигнализации, для отображения извещателем.
            CPss21::GetAlarmWindowControlPointer(GetAlarmWindowIndex()) -> SetActivityState(1);
            CPss21::BoardWindowsUpdate();
            SetFsmState(NOT_ACTIVE_STATE_WAITING);
        }
        break;

    case NOT_ACTIVE_STATE_WAITING:
        // Дискретный сигнал не активен?
        if (!(CPss21::GetDiscreteInputState(GetDiscreteStateIndex()) ^ ACTIVE_LEVEL()))
        {
            // Установим тип сигнализации связанному окну в массиве управления окнами извещателя.
            CPss21::SetAlarmWindowType(GetAlarmWindowIndex(), NORMAL);
            // Деактмвируем окно сигнализации, для прекращения отображения извещателем.
            CPss21::GetAlarmWindowControlPointer(GetAlarmWindowIndex()) -> SetActivityState(0);
            CPss21::BoardWindowsUpdate();
            SetFsmState(ACTIVE_STATE_WAITING);
        }
        else
        {
            // Установим связанные дискретный выходы - не новое нарушение.
            CPss21::DiscreteOutputsSet(GetLinkedDiscreteOutputsPointer(), NOT_NEW_VIOLATION);
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
// Автомат обработки системных ошибок.
void CErrorAlarmDfa::Fsm(void)
{
    switch (GetFsmState())
    {
    case START:
        break;

    case ACTIVE_STATE_WAITING:
        // Дискретный сигнал активен?
        if (CPss21::GetErrorAlarmState(GetDiscreteStateIndex()) ^ ACTIVE_LEVEL())
        {
            // Тип запрограммированной сигнализации дискретного сигнала имеет более высокий приоритет,
            // чем тип общей сигнализации?
            if ((CPss21::GetCommonAlarmType() != ERROR) &&
                    (CPss21::GetCommonAlarmType() < ALARM_TYPE()))
            {
                // Установим тип общей сигнализацию.
                CPss21::SetCommonAlarmType(ALARM_TYPE());

                // Изменим тип общей сигнализацию.
                CPss21::AlarmTypeErrorChange();
            }

            CPss21::SetAlarmWindowColor(GetAlarmWindowIndex(), ALARM_TYPE());
//            // Установим тип сигнализации связанному окну в массиве управления окнами извещателя.
            CPss21::ErrorWindowOn(GetAlarmWindowIndex());
            SetFsmState(RECEIPT_WAITING);
        }
        break;

    case RECEIPT_WAITING:
        // Событие квитировано?
        if (CPss21::GetDiscreteSignalsReceipt())
        {
            // Переведём автоматы сигнализаций в состояние - квитировано.
            CPss21::AlarmTypeReceipt();
            SetFsmState(RECEIPTED_NOT_ACTIVE_STATE_WAITING);
        }
        break;

    case RECEIPTED_NOT_ACTIVE_STATE_WAITING:
        // Дискретный сигнал не активен?
        if (!(CPss21::GetErrorAlarmState(GetDiscreteStateIndex()) ^ ACTIVE_LEVEL()))
        {
            // Переведём автоматы сигнализаций в состояние - сброшено.
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







