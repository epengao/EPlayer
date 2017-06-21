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
 * MessageHub.cpp
 * This file for MessageHub class implemetaion
 *
 * Eamil:   epengao@126.com
 * Author:  Gao Peng
 * Version: Intial first version.
 * ****************************************************************
 */

#include "ECLog.h"
#include "ECAutoLock.h"
#include "MessageHub.h"

MessageHub* MessageHub::m_pInstance = NULL;

MessageHub* MessageHub::GetInstance()
{
    if (m_pInstance == NULL)
    {
        m_pInstance = new MessageHub();
    }
    return m_pInstance;
}

MessageHub::MessageHub()
{
    m_Exit = false;
    m_MsgNotifer.Callback = NULL;
    m_MsgNotifer.pUserData = NULL;
    m_pMsgQueue = new ECRingQueue<EC_U32>();
    m_pThreadDriver = new ECThreadDriver(this);

    m_pThreadDriver->Run();
}

MessageHub::~MessageHub()
{
    if (m_pMsgQueue)
    {
        ECAutoLock lock(&m_mtxQ);
        m_pMsgQueue->Clean();
        delete m_pMsgQueue;
        m_pMsgQueue = NULL;
    }
    if (m_pThreadDriver)
    {
        m_Exit = true;
        m_ProvokeSig.Send();
        m_pThreadDriver->Exit();
        delete m_pThreadDriver;
    }
    m_pInstance = NULL;
}

void MessageHub::SendMessage(EC_U32 msg)
{
    {
        ECAutoLock lock(&m_mtxQ);
        if (m_pMsgQueue)
        {
            m_pMsgQueue->Push(msg);
        }
    }
    m_ProvokeSig.Send();
}

void MessageHub::SetNotifer(void* pUserData,
                            void(*callbackEntry)(void*, EC_U32 msg))
{
    m_MsgNotifer.pUserData = pUserData;
    m_MsgNotifer.Callback = callbackEntry;
}

void MessageHub::DoRunning()
{
    if (m_pMsgQueue)
    {
        EC_U32 nMsg, nRet;
        {
            ECAutoLock lock(&m_mtxQ);
            nRet = m_pMsgQueue->Pop(&nMsg);
        }
        if( (nRet == EC_Err_None) &&
            (m_MsgNotifer.Callback != NULL) )
        {
            ECLogI("Send EPlayerSDK Msg: %d", nMsg);
            m_MsgNotifer.Callback(m_MsgNotifer.pUserData, nMsg);
        }
    }
    if (!m_Exit)
    {
        m_ProvokeSig.Wait();
    }
}
