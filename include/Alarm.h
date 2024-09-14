//-----------------------------------------------------------------------------------------------------
//  Source      : FileName.cpp
//  Created     : 01.06.2022
//  Author      : Alexandr Volvenkin
//  email       : aav-36@mail.ru
//  GitHub      : https://github.com/AlexandrVolvenkin
//-----------------------------------------------------------------------------------------------------
#ifndef CALARM_H
#define CALARM_H

#include <stdint.h>

#include "Configuration.h"
#include "AlarmWindow.h"

//-----------------------------------------------------------------------------------------------------
class CAlarmDfa : public CDfaLite
{
public:

    enum DISCRETE_SIGNAL_STATE_CODE
    {
        // дискретный сигнал неактивен
        DISCRETE_SIGNAL_IS_NOT_ACTIVE = 0,
        // дискретный сигнал активен
        DISCRETE_SIGNAL_IS_ACTIVE = 1,
        // дискретный сигнал недостоверен
        DISCRETE_SIGNAL_IS_NAMUR_ERROR = 2,
        // дискретный сигнал недостоверен
        DISCRETE_SIGNAL_IS_INVALID = 3,
    };

    enum
    {
        START = 0,
        ACTIVE_STATE_WAITING,
        RECEIPT_OR_RESET_WAITING,
        RECEIPT_OR_RESET_OR_AUTOUNSET_WAITING,
        RECEIPTED_RESET_OR_NOT_ACTIVE_STATE_WAITING,
        RESETED_NOT_ACTIVE_STATE_WAITING,
        RECEIPTED_RESET_WAITING,
        NOT_ACTIVE_STATE_WAITING,
        NAMUR_STATE_ON,
        NAMUR_INPUT_CORRECT_STATE_WAITING,
    };

    enum
    {
        NOT_NEW_VIOLATION = 0,
        NEW_VIOLATION = 1,
    };

    virtual uint8_t ALARM_TYPE(void)
    {
        return NORMAL;
    };

    virtual uint8_t ACTIVE_LEVEL(void)
    {
        return 0;
    };

    virtual uint8_t IS_NAMUR_ON(void)
    {
        return 0;
    };

    CAlarmDfa();
    virtual ~CAlarmDfa();
    virtual void Fsm(void);

    void SetDiscreteStateIndex(uint8_t uiDiscreteStateIndex)
    {
        m_uiDiscreteStateIndex = uiDiscreteStateIndex;
    };
    uint8_t GetDiscreteStateIndex(void)
    {
        return m_uiDiscreteStateIndex;
    };

    void SetAlarmWindowIndex(uint8_t uiAlarmWindowIndex)
    {
        m_uiAlarmWindowIndex = uiAlarmWindowIndex;
    };
    uint8_t GetAlarmWindowIndex(void)
    {
        return m_uiAlarmWindowIndex;
    };

    uint8_t* GetLinkedDiscreteOutputsPointer(void)
    {
        return m_auiLinkedDiscreteOutputs;
    };

protected:
private:
    // »ндекс окна извещател€ - светового табло.
    uint8_t m_uiAlarmWindowIndex;
    // ¬ыходы на ћ–.
    uint8_t m_auiLinkedDiscreteOutputs[DISCRETE_OUTPUT_MODULE_MAX_NUMBER];
    // »сточник дискретного сигнала.
    uint8_t m_uiDiscreteStateIndex;
    // »сточник данных достоверности дискретного сигнала.
    uint8_t m_uiDiscreteInputsBadStateIndex;
};
//-----------------------------------------------------------------------------------------------------








//-----------------------------------------------------------------------------------------------------
class CNormalAlarmDfa : public CAlarmDfa
{
public:

    uint8_t ALARM_TYPE(void)
    {
        return NORMAL;
    };

    uint8_t ACTIVE_LEVEL(void)
    {
        return 0;
    };

    uint8_t IS_NAMUR_ON(void)
    {
        return 0;
    };

    CNormalAlarmDfa();
    virtual ~CNormalAlarmDfa();

protected:
private:
};
//-----------------------------------------------------------------------------------------------------








//-----------------------------------------------------------------------------------------------------
class CPreventiveAlarmLowLevelDfa : public CAlarmDfa
{
public:

    uint8_t ALARM_TYPE(void)
    {
        return PREVENTIVE;
    };

    uint8_t ACTIVE_LEVEL(void)
    {
        return 0;
    };

    uint8_t IS_NAMUR_ON(void)
    {
        return 0;
    };

    CPreventiveAlarmLowLevelDfa();
    virtual ~CPreventiveAlarmLowLevelDfa();


protected:
private:
};
//-----------------------------------------------------------------------------------------------------








//-----------------------------------------------------------------------------------------------------
class CPreventiveAlarmHighLevelDfa : public CAlarmDfa
{
public:

    uint8_t ALARM_TYPE(void)
    {
        return PREVENTIVE;
    };

    uint8_t ACTIVE_LEVEL(void)
    {
        return 1;
    };

    uint8_t IS_NAMUR_ON(void)
    {
        return 0;
    };

    CPreventiveAlarmHighLevelDfa();
    virtual ~CPreventiveAlarmHighLevelDfa();

protected:
private:
};
//-----------------------------------------------------------------------------------------------------








//-----------------------------------------------------------------------------------------------------
class CPreventiveAlarmLowLevelNamurDfa : public CAlarmDfa
{
public:

    uint8_t ALARM_TYPE(void)
    {
        return PREVENTIVE;
    };

    uint8_t ACTIVE_LEVEL(void)
    {
        return 0;
    };

    uint8_t IS_NAMUR_ON(void)
    {
        return 1;
    };

    CPreventiveAlarmLowLevelNamurDfa();
    virtual ~CPreventiveAlarmLowLevelNamurDfa();


protected:
private:
};
//-----------------------------------------------------------------------------------------------------








//-----------------------------------------------------------------------------------------------------
class CPreventiveAlarmHighLevelNamurDfa : public CAlarmDfa
{
public:

    uint8_t ALARM_TYPE(void)
    {
        return PREVENTIVE;
    };

    uint8_t ACTIVE_LEVEL(void)
    {
        return 1;
    };

    uint8_t IS_NAMUR_ON(void)
    {
        return 1;
    };

    CPreventiveAlarmHighLevelNamurDfa();
    virtual ~CPreventiveAlarmHighLevelNamurDfa();

protected:
private:
};
//-----------------------------------------------------------------------------------------------------








//-----------------------------------------------------------------------------------------------------
class CEmergencyAlarmLowLevelDfa : public CAlarmDfa
{
public:

    uint8_t ALARM_TYPE(void)
    {
        return EMERGENCY;
    };

    uint8_t ACTIVE_LEVEL(void)
    {
        return 0;
    };

    uint8_t IS_NAMUR_ON(void)
    {
        return 0;
    };

    CEmergencyAlarmLowLevelDfa();
    virtual ~CEmergencyAlarmLowLevelDfa();

protected:
private:
};
//-----------------------------------------------------------------------------------------------------








//-----------------------------------------------------------------------------------------------------
class CEmergencyAlarmHighLevelDfa : public CAlarmDfa
{
public:

    uint8_t ALARM_TYPE(void)
    {
        return EMERGENCY;
    };

    uint8_t ACTIVE_LEVEL(void)
    {
        return 1;
    };

    uint8_t IS_NAMUR_ON(void)
    {
        return 0;
    };

    CEmergencyAlarmHighLevelDfa();
    virtual ~CEmergencyAlarmHighLevelDfa();

protected:
private:
};
//-----------------------------------------------------------------------------------------------------








//-----------------------------------------------------------------------------------------------------
class CEmergencyAlarmLowLevelNamurDfa : public CAlarmDfa
{
public:

    uint8_t ALARM_TYPE(void)
    {
        return EMERGENCY;
    };

    uint8_t ACTIVE_LEVEL(void)
    {
        return 0;
    };

    uint8_t IS_NAMUR_ON(void)
    {
        return 1;
    };

    CEmergencyAlarmLowLevelNamurDfa();
    virtual ~CEmergencyAlarmLowLevelNamurDfa();

protected:
private:
};
//-----------------------------------------------------------------------------------------------------








//-----------------------------------------------------------------------------------------------------
class CEmergencyAlarmHighLevelNamurDfa : public CAlarmDfa
{
public:

    uint8_t ALARM_TYPE(void)
    {
        return EMERGENCY;
    };

    uint8_t ACTIVE_LEVEL(void)
    {
        return 1;
    };

    uint8_t IS_NAMUR_ON(void)
    {
        return 1;
    };

    CEmergencyAlarmHighLevelNamurDfa();
    virtual ~CEmergencyAlarmHighLevelNamurDfa();

protected:
private:
};
//-----------------------------------------------------------------------------------------------------








//-----------------------------------------------------------------------------------------------------
class CIndicationAlarmLowLevelDfa : public CAlarmDfa
{
public:

    uint8_t ALARM_TYPE(void)
    {
        return INDICATION;
    };

    uint8_t ACTIVE_LEVEL(void)
    {
        return 0;
    };

    uint8_t IS_NAMUR_ON(void)
    {
        return 0;
    };

    CIndicationAlarmLowLevelDfa();
    virtual ~CIndicationAlarmLowLevelDfa();
    virtual void Fsm(void);

protected:
private:
};
//-----------------------------------------------------------------------------------------------------








//-----------------------------------------------------------------------------------------------------
class CIndicationAlarmHighLevelDfa : public CIndicationAlarmLowLevelDfa
{
public:

    uint8_t ALARM_TYPE(void)
    {
        return INDICATION;
    };

    uint8_t ACTIVE_LEVEL(void)
    {
        return 1;
    };

    uint8_t IS_NAMUR_ON(void)
    {
        return 0;
    };

    CIndicationAlarmHighLevelDfa();
    virtual ~CIndicationAlarmHighLevelDfa();

protected:
private:
};

//-----------------------------------------------------------------------------------------------------








//-----------------------------------------------------------------------------------------------------
class CIndicationAlarmLowLevelNamurDfa : public CIndicationAlarmLowLevelDfa
{
public:

    uint8_t ALARM_TYPE(void)
    {
        return INDICATION;
    };

    uint8_t ACTIVE_LEVEL(void)
    {
        return 0;
    };

    uint8_t IS_NAMUR_ON(void)
    {
        return 1;
    };

    CIndicationAlarmLowLevelNamurDfa();
    virtual ~CIndicationAlarmLowLevelNamurDfa();

protected:
private:
};
//-----------------------------------------------------------------------------------------------------








//-----------------------------------------------------------------------------------------------------
class CIndicationAlarmHighLevelNamurDfa : public CIndicationAlarmLowLevelDfa
{
public:

    uint8_t ALARM_TYPE(void)
    {
        return INDICATION;
    };

    uint8_t ACTIVE_LEVEL(void)
    {
        return 1;
    };

    uint8_t IS_NAMUR_ON(void)
    {
        return 1;
    };

    CIndicationAlarmHighLevelNamurDfa();
    virtual ~CIndicationAlarmHighLevelNamurDfa();

protected:
private:
};

//-----------------------------------------------------------------------------------------------------








//-----------------------------------------------------------------------------------------------------
class CErrorAlarmDfa : public CDfaLite
{
public:
    enum
    {
        START = 0,
        ACTIVE_STATE_WAITING,
        RECEIPT_WAITING,
        RECEIPTED_NOT_ACTIVE_STATE_WAITING,
    };

    virtual uint8_t ALARM_TYPE(void)
    {
        return ERROR;
    };

    virtual uint8_t ACTIVE_LEVEL(void)
    {
        return 0;
    };

    uint8_t IS_NAMUR_ON(void)
    {
        return 0;
    };

    CErrorAlarmDfa();
    virtual ~CErrorAlarmDfa();
    virtual void Fsm(void);

    void SetDiscreteStateIndex(uint8_t uiDiscreteStateIndex)
    {
        m_uiDiscreteStateIndex = uiDiscreteStateIndex;
    };
    uint8_t GetDiscreteStateIndex(void)
    {
        return m_uiDiscreteStateIndex;
    };

    void SetAlarmWindowIndex(uint8_t uiAlarmWindowIndex)
    {
        m_uiAlarmWindowIndex = uiAlarmWindowIndex;
    };
    uint8_t GetAlarmWindowIndex(void)
    {
        return m_uiAlarmWindowIndex;
    };

protected:
private:
    // »ндекс окна извещател€ - светового табло.
    uint8_t m_uiAlarmWindowIndex;
    // »сточник дискретного сигнала.
    uint8_t m_uiDiscreteStateIndex;
};
//-----------------------------------------------------------------------------------------------------








////-----------------------------------------------------------------------------------------------------
//class CInputModuleErrorAlarmDfa : public CErrorAlarmDfa
//{
//public:
//
//    uint8_t ALARM_TYPE(void)
//    {
//        return ERROR;
//    };
//
//    uint8_t ACTIVE_LEVEL(void)
//    {
//        return 1;
//    };
//
//    CInputModuleErrorAlarmDfa();
//    virtual ~CInputModuleErrorAlarmDfa();
//
//protected:
//private:
//};
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
//class COutpuModuleErrorAlarmDfa : public CErrorAlarmDfa
//{
//public:
//
//    uint8_t ALARM_TYPE(void)
//    {
//        return EMERGENCY;
//    };
//
//    uint8_t ACTIVE_LEVEL(void)
//    {
//        return 1;
//    };
//
//    COutpuModuleErrorAlarmDfa();
//    virtual ~COutpuModuleErrorAlarmDfa();
//
//protected:
//private:
//};
////-----------------------------------------------------------------------------------------------------

#endif // CALARM_H
