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
 * ECThreadDriver.h
 * This file for ECThreadDeiver define and encapsulation.
 *
 * Eamil:   epengao@126.com
 * Author:  Gao Peng
 * Version: Intial first version.
 * ****************************************************************
 */

#ifndef EC_THREAD_DRIVER_H
#define EC_THREAD_DRIVER_H

#include <pthread.h>
#include "ECMutex.h"
#include "ECSignal.h"
#include "ECString.h"
#include "ECThreadWorker.h"

typedef enum
{
    ECThreadDriverStatus_Init,
    ECThreadDriverStatus_Idle,
    ECThreadDriverStatus_Running,
    ECThreadDriverStatus_Stopped,
    ECThreadDriverStatus_Unknown = EC_U16_MAX,
} ECThreadDriverStatus;

typedef pthread_t ECThreadHandle;

class ECThreadDriver
{
public:
    ECThreadDriver(
                ECThreadWorkerI* pThreadWorker,
                const char* pThreadDriverName = NULL);
    ~ECThreadDriver();

    void Run();
    void Pause();
    void Exit();
    
    void RunSteps(EC_U32 nSteps);

private:
    static void* DriverProcEntry(void* pArgs);

private:
    EC_U32               m_runSteps;
    ECString             m_strName;
    ECSignal             m_PauseSig;
    ECSignal             m_ProvokeSig;
    ECThreadHandle       m_ThreadHandle;
    ECThreadWorkerI*     m_pThreadWorker;
    ECThreadDriverStatus m_nStatus;
};

#endif /* EC_THREAD_DRIVER_H */
