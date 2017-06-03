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
 * AudioDecoder.cpp
 * This file for Media audio decoder module class implement.
 *
 * Eamil:   epengao@126.com
 * Author:  Gao Peng
 * Version: Intial first version.
 * ****************************************************************
 */

#include "ECLog.h"
#include "ECUtil.h"
#include "ECAutoLock.h"
#include "AudioDecPort.h"
#include "AudioDecoder.h"
#include "FFmpegAudioDecoder.h"

AudioDecoder::AudioDecoder()
:m_pPort(NULL)
,m_pSourcePort(NULL)
,m_pCurrPacket(NULL)
,m_pFFmpegDecoder(NULL)
{
    m_pPort = new AudioDecPort(this);
    m_pFFmpegDecoder = new FFmpegAudioDecoder();
}

AudioDecoder::~AudioDecoder()
{
    if (m_pPort) delete m_pPort;
    if (m_pFFmpegDecoder) delete m_pFFmpegDecoder;
}

EC_U32 AudioDecoder::OpenDecoder(MediaContext* pMediaContext,
                                 SourceAudioPort* pSourcePort)
{
    EC_U32 nRet;

    if ((pSourcePort == NULL) ||
        (pMediaContext == NULL) )
    {
        nRet = EC_Err_BadParam;
    }
    else
    {
        m_pSourcePort = pSourcePort;
        {
            ECAutoLock autoLock(&m_mtxDec);
            nRet = m_pFFmpegDecoder->OpenDecoder(pMediaContext);
        }
    }
    return nRet;
}

void AudioDecoder::CloseDecoder()
{
    ECAutoLock autoLock(&m_mtxDec);
    if (m_pCurrPacket)
    {
        m_pSourcePort->ReturnAudioEmptyPacket(m_pCurrPacket);
        m_pCurrPacket = NULL;
    }
    m_pFFmpegDecoder->CloseDecoder();
}

void AudioDecoder::Flush()
{
    ECAutoLock autolock(&m_mtxDec);
    if (m_pCurrPacket)
    {
        m_pSourcePort->ReturnAudioEmptyPacket(m_pCurrPacket);
        m_pCurrPacket = NULL;
    }
    m_pFFmpegDecoder->FlushDecoder();
}

EC_U32 AudioDecoder::GetAudioFrame(AudioFrame* pFrameOut, bool rawFFPCMBuf)
{
    ECAutoLock lock(&m_mtxDec);
    EC_U32 nRet = m_pFFmpegDecoder->GetOutputFrame(pFrameOut, rawFFPCMBuf);
    if (nRet!= EC_Err_None && pFrameOut)
    {
        if (m_pCurrPacket == NULL)
        {
            nRet = m_pSourcePort->GetAudioDataPacket(&m_pCurrPacket);
        }
        if (nRet == Source_Err_None && m_pCurrPacket)
        {
            nRet = m_pFFmpegDecoder->SetInputPacket(m_pCurrPacket);
            if (nRet != Audio_Dec_Err_KeepPkt)
            {
                m_pSourcePort->ReturnAudioEmptyPacket(m_pCurrPacket);
                m_pCurrPacket = NULL;
            }
            nRet = m_pFFmpegDecoder->GetOutputFrame(pFrameOut, rawFFPCMBuf);
        }
    }
    return nRet;
}
