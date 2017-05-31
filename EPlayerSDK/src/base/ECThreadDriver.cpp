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
 * ECThreadDriver.cpp
 * This file for ECThreadDeiver implementation.
 *
 * Eamil:   epengao@126.com
 * Author:  Gao Peng
 * Version: Intial first version.
 * ****************************************************************
 */

#include "ECAutoLock.h"
#include "ECThreadDriver.h"


ECThreadDriver::ECThreadDriver(
                    ECThreadWorkerI* pThreadWorker,
                    const char* pThreadDriverName /*NULL*/)
:m_runSteps(0)
,m_strName(pThreadDriverName)
,m_pThreadWorker(pThreadWorker)
,m_nStatus(ECThreadDriverStatus_Init)
{
    pthread_create(&m_ThreadHandle, NULL, DriverProcEntry, this);
    m_nStatus = ECThreadDriverStatus_Idle;
}

ECThreadDriver::~ECThreadDriver()
{
}

void ECThreadDriver::Run()
{
    if( ECThreadDriverStatus_Init == m_nStatus ||
        ECThreadDriverStatus_Idle == m_nStatus )
    {
        m_nStatus = ECThreadDriverStatus_Running;
        m_ProvokeSig.Send();
    }
}

void ECThreadDriver::Pause()
{
    if(ECThreadDriverStatus_Running == m_nStatus)
    {
        m_nStatus = ECThreadDriverStatus_Idle;
        m_ProvokeSig.Send();
        m_PauseSig.Wait();
    }
}

void ECThreadDriver::Exit()
{
    if (ECThreadDriverStatus_Stopped != m_nStatus)
    {
        m_nStatus = ECThreadDriverStatus_Stopped;
        m_ProvokeSig.Send();
        pthread_join(m_ThreadHandle, NULL);
    }
}

void ECThreadDriver::RunSteps(EC_U32 nSteps)
{
    if (ECThreadDriverStatus_Idle == m_nStatus)
    {
        m_runSteps = nSteps;
        m_nStatus = ECThreadDriverStatus_Running;
        m_ProvokeSig.Send();
    }
}

void* ECThreadDriver::DriverProcEntry(void* pArgs)
{
    if (NULL == pArgs) return NULL;

    ECThreadDriver* pThreadDriver = (ECThreadDriver*)pArgs;
    if (NULL == pThreadDriver->m_pThreadWorker) return NULL;

    while(1)
    {
        switch (pThreadDriver->m_nStatus)
        {
            case ECThreadDriverStatus_Init:
            {
                pThreadDriver->m_ProvokeSig.Wait();
            }
            break;
            case ECThreadDriverStatus_Idle:
            {
                pThreadDriver->m_PauseSig.Send();
                pThreadDriver->m_ProvokeSig.Wait();
                //pThreadDriver->m_pThreadWorker->DoIdle();
            }
            break;

            case ECThreadDriverStatus_Running:
            {
                pThreadDriver->m_pThreadWorker->DoRunning();
                if(pThreadDriver->m_runSteps > 0)
                {
                    pThreadDriver->m_runSteps --;
                    if (pThreadDriver->m_runSteps == 0)
                    {
                        pThreadDriver->m_nStatus = ECThreadDriverStatus_Idle;
                    }
                }
            }
            break;

            case ECThreadDriverStatus_Stopped:
            {
                pThreadDriver->m_pThreadWorker->DoExit();
                return NULL;
            }
            break;

            default: return NULL;
        }
    }
}
