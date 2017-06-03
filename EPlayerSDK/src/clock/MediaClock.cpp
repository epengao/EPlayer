/*
 * ****************************************************************
 * This software is a media player SDK implementation
 * GPL:
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Library General Public
 * License as published by the Free Software Foundation; either
 * version 2 of the License, or (at your option) any later version.
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Library General Public License for more details. You should
 * have received a copy of the GNU Library General Public License
 * along with this library; if not, write to the Free Foundation,
 * Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 *
 * Project: EC < Enjoyable Coding >
 *
 * MediaClock.cpp
 * This file implement a clock used to sync audio/vido stream.
 *
 * Eamil:   epengao@126.com
 * Author:  Gao Peng
 * Version: Intial first version.
 * ****************************************************************
 */

#include "ECLog.h"
#include "ECUtil.h"
#include "MediaClock.h"

MediaClock::MediaClock()
:m_nTime(0)
,m_nLastUpdate(0)
,m_bSeekAdjust(false)
,m_nSeekAdjustTime(0)
,m_nStatu(MediaClock_Status_Stoped)
,m_nSeekAdjustTimeType(MediaTimeType_UnDefine)
{
}

MediaClock::~MediaClock()
{
}

void MediaClock::Run()
{
    m_nLastUpdate = ECGetSystemTime();
    m_nStatu = MediaClock_Status_Running;
}

void MediaClock::Pause()
{
    m_nTime = GetClockTime();
    m_nStatu = MediaClock_Status_Pause;
}

void MediaClock::Reset()
{
    m_nTime = 0;
    m_nLastUpdate = 0;
    m_nStatu = MediaClock_Status_Stoped;
    ResetMediaSeekTime();
}

TimeStamp MediaClock::GetClockTime()
{
    TimeStamp nTime = m_nTime;
    if (MediaClock_Status_Running == m_nStatu)
    {
        TimeStamp nCurrSysTime = ECGetSystemTime();
        TimeStamp nTimePassed = nCurrSysTime - m_nLastUpdate;
        nTime = m_nTime + nTimePassed;
    }
    return nTime;
}

void MediaClock::SetMediaTime(TimeStamp nTime)
{
    m_nTime = nTime;
    m_nLastUpdate = ECGetSystemTime();
}

void MediaClock::UpdateClockTime(TimeStamp nTime)
{
    if (MediaClock_Status_Running == m_nStatu)
    {
        m_nTime = nTime;
        m_nLastUpdate = ECGetSystemTime();
    }
}

void MediaClock::ClockTimeGoForward(TimeStamp nTime)
{
    m_nTime = m_nTime + nTime;
}

void MediaClock::ResetMediaSeekTime()
{
    m_bSeekAdjust = false;
    m_nSeekAdjustTime = 0;
    m_nSeekAdjustTimeType = MediaTimeType_UnDefine;
}

void MediaClock::SyncMediaSeekTime(MediaTimeType timeType, TimeStamp timeValue)
{
    if(m_nSeekAdjustTime == 0)
    {
        /* Just save seek time */
        m_nSeekAdjustTime = timeValue;
        m_nSeekAdjustTimeType = timeType;
    }
    else
    {
        if(timeValue < m_nSeekAdjustTime)
        {
            m_bSeekAdjust = true;
            m_nSeekAdjustTimeType = timeType;
        }
        else if(timeValue > m_nSeekAdjustTime)
        {
            m_bSeekAdjust = true;
            m_nSeekAdjustTime = timeValue;
        }
        else
        {
            ResetMediaSeekTime();
        }
    }
}

void MediaClock::GetMediaSeekAdjustTime(MediaTimeType* timeTypeOut, TimeStamp* timeValueOut)
{
    if(m_bSeekAdjust && timeTypeOut && timeValueOut)
    {
        *timeValueOut = m_nSeekAdjustTime;
        *timeTypeOut = m_nSeekAdjustTimeType;
    }
    ResetMediaSeekTime();
}
