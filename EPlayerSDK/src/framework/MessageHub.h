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
 * MessageHub.h
 * This file for Media MessageHub define.
 *
 * Eamil:   epengao@126.com
 * Author:  Gao Peng
 * Version: Intial first version.
 * ****************************************************************
 */

#ifndef MESSAGE_HUB_H
#define MESSAGE_HUB_H

#include "ECMutex.h"
#include "ECRingQueue.h"
#include "ECThreadDriver.h"

typedef enum
{
    PlayerMessage_PlayStart            = 1,
    PlayerMessage_PlayStop             = 2,
    PlayerMessage_NetworkError         = 3,
    PlayerMessage_Video_BufferingStart = 4,
    PlayerMessage_Video_BufferingStop  = 5,
    PlayerMessage_Audio_BufferingStart = 6,
    PlayerMessage_Audio_BufferingStop  = 7,
    PlayerMessage_Audio_PlayFinished   = 8,
    PlayerMessage_Video_PlayFinished   = 9,
    PlayerMessage_Undefine        = 0xFFFF,
} PlayerMessage;

typedef struct
{
    void* pUserData;
    void (*Callback)(void* pUserData, EC_U32 msg);
} MsgNotifier;

class MessageHub : public ECThreadWorkerI
{
public:
    static MessageHub* GetInstance();

    ~MessageHub();
    void SendMessage(EC_U32 msg);
    void SetNotifer(void* pUserData,
                    void(*callbackEntry)(void*, EC_U32 msg));
private:
    /* Singleton */
    MessageHub();
    MessageHub(const MessageHub &);
    MessageHub& operator = (const MessageHub &);
    static MessageHub* m_pInstance;

    /* ECThreadWorkerI */
    virtual void DoRunning();
    virtual void DoExit() {};

private:
    bool                  m_Exit;
    ECMutex               m_mtxQ;
    ECSignal              m_ProvokeSig;
    MsgNotifier           m_MsgNotifer;
    ECRingQueue<EC_U32>*  m_pMsgQueue;
    ECThreadDriver*       m_pThreadDriver;
};

#endif /* MESSAGE_HUB_H */
