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
 * ECSignal.cpp
 * This file for mutil thread used signal implementation.
 *
 * Eamil:   epengao@126.com
 * Author:  Gao Peng
 * Version: Intial first version.
 * ****************************************************************
 */

#include "ECType.h"
#include "ECSignal.h"


ECSignal::ECSignal()
{
    pthread_cond_init(&m_Cond, 0);
    pthread_mutex_init(&m_Mutex, NULL);
}

ECSignal::~ECSignal()
{
    pthread_cond_destroy(&m_Cond);
    pthread_mutex_destroy(&m_Mutex);
}

void ECSignal::Send()
{
    pthread_cond_signal(&m_Cond);
}

void ECSignal::Wait()
{
    pthread_cond_wait(&m_Cond, &m_Mutex);
}

void ECSignal::TimedWait(EC_U32 nTimeout)
{
    struct timespec timeout;
    timeout.tv_nsec = 0;
    timeout.tv_sec = time(NULL) + nTimeout;

    pthread_cond_timedwait(&m_Cond, &m_Mutex, &timeout);;
}
