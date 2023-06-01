//-----------------------------------------------------------------------------------------------------
//  Source      : FileName.cpp
//  Created     : 01.06.2022
//  Author      : Alexandr Volvenkin
//  email       : aav-36@mail.ru
//  GitHub      : https://github.com/AlexandrVolvenkin
//-----------------------------------------------------------------------------------------------------
#include "Event.h"

//-----------------------------------------------------------------------------------------------------
CEvent::CEvent()
{

}

//-----------------------------------------------------------------------------------------------------
CEvent::~CEvent()
{

}
//-----------------------------------------------------------------------------------------------------




//-----------------------------------------------------------------------------------------------------
CEventUniteLogicAnd::CEventUniteLogicAnd()
{
    SetFsmState(START);
}

//-----------------------------------------------------------------------------------------------------
CEventUniteLogicAnd::CEventUniteLogicAnd(uint16_t uiTime) :
    m_uiTime(uiTime)
{
    SetFsmState(START);
}

//-----------------------------------------------------------------------------------------------------
CEventUniteLogicAnd::~CEventUniteLogicAnd()
{

}

//-----------------------------------------------------------------------------------------------------
// ���������� ������� ������������ ��������� � ������� UNITED_WAITING_TIME.
uint8_t CEventUniteLogicAnd::IsHappened(uint8_t uiEventOne, uint8_t uiEventTwo)
{
    switch (GetFsmState())
    {
    case START:
        m_uiEventsState = 0;
        SetFsmState(EVENT_WAITING);
        return 0;
        break;

    case EVENT_WAITING:
        // ��������� ������ �������?
        if (uiEventOne)
        {
            m_uiEventsState |= 0x01;
        }
        // ��������� ������ �������?
        if (uiEventTwo)
        {
            m_uiEventsState |= 0x02;
        }
        // ��������� �������� �������� ������� �������.
        m_xTimer.Set(m_uiTime);
        SetFsmState(EVENT_UNITE_WAITING);
        return 0;
        break;

    case EVENT_UNITE_WAITING:
        // ����������� ����� ����������� �������?
        if (m_xTimer.IsOverflow())
        {
            SetFsmState(START);
            return 0;
        }
        else
        {
            // ��������� ������ �������?
            if (uiEventOne)
            {
                m_uiEventsState |= 0x01;
            }
            // ��������� ������ �������?
            if (uiEventTwo)
            {
                m_uiEventsState |= 0x02;
            }
            // ��������� ��� ������������ �������?
            if (m_uiEventsState == 0x03)
            {
                SetFsmState(START);
                return 1;
            }
            else
            {
                return 0;
            }
        }
        break;

    default:
        return 0;
        break;
    }
}
