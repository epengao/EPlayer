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
 * VideoDecoder.cpp
 * This file for Media video decoder module class implement.
 *
 * Eamil:   epengao@126.com
 * Author:  Gao Peng
 * Version: Intial first version.
 * ****************************************************************
 */

#include "ECUtil.h"
#include "ECAutoLock.h"
#include "VideoDecPort.h"
#include "VideoDecoder.h"
#include "FFmpegVideoDecoder.h"

VideoDecoder::VideoDecoder()
:m_pPort(NULL)
,m_pSourcePort(NULL)
,m_pCurrPacket(NULL)
,m_pFFmpegDecoder(NULL)
{
    m_pPort = new VideoDecPort(this);
    m_pFFmpegDecoder = new FFmpegVideoDecoder();
}

VideoDecoder::~VideoDecoder()
{
    if (m_pPort) delete m_pPort;
    if (m_pFFmpegDecoder) delete m_pFFmpegDecoder;
}

EC_U32 VideoDecoder::OpenDecoder(MediaContext* pMediaContext,
                                 SourceVideoPort* pSourcePort)
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

void VideoDecoder::CloseDecoder()
{
    ECAutoLock autoLock(&m_mtxDec);
    if (m_pCurrPacket)
    {
        m_pSourcePort->ReturnVideoEmptyPacket(m_pCurrPacket);
        m_pCurrPacket = NULL;
    }
    m_pFFmpegDecoder->CloseDecoder();
}

void VideoDecoder::Flush()
{
    ECAutoLock autolock(&m_mtxDec);
    if (m_pCurrPacket)
    {
        m_pSourcePort->ReturnVideoEmptyPacket(m_pCurrPacket);
        m_pCurrPacket = NULL;
    }
    m_pFFmpegDecoder->FlushDecoder();
}

EC_U32 VideoDecoder::GetVideoFrame(VideoFrame* pFrameOut)
{
    EC_U32 nRet = 0;
    if (pFrameOut)
    {
        ECAutoLock lock(&m_mtxDec);
        nRet = m_pFFmpegDecoder->GetOutputFrame(pFrameOut);
        if(nRet != Video_Dec_Err_None)
        {
            if (m_pCurrPacket == NULL)
            {
                nRet = m_pSourcePort->GetVideoDataPacket(&m_pCurrPacket);
            }
            if (nRet == Source_Err_None && m_pCurrPacket)
            {
                nRet = m_pFFmpegDecoder->SetInputPacket(m_pCurrPacket);
                if (nRet != Video_Dec_Err_KeepPkt)
                {
                    m_pSourcePort->ReturnVideoEmptyPacket(m_pCurrPacket);
                    m_pCurrPacket = NULL;
                }
                nRet = m_pFFmpegDecoder->GetOutputFrame(pFrameOut);
            }
            else
            {
                ECSleep(VIDEO_DEC_WAIT_TIME);
            }
        }
    }
    return nRet;
}
