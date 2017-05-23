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
* ECBufferStream.h
* This file for Buffer stream define.
*
* Eamil:   epengao@126.com
* Author:  Gao Peng
* Version: Intial first version.
* ****************************************************************
*/

#include "ECMemOP.h"
#include "ECBufferStream.h"

#define MIN(x,y) ((x)<(y)?(x):(y))

ECBufferStream::ECBufferStream(EC_U32 nSize, bool fixedSize)
:m_nDataSize(0)
,m_pBuffer(NULL)
,m_pReadPos(NULL)
,m_pWritePos(NULL)
,m_nBufSize(nSize)
,m_Fixed(fixedSize)
{
    if ((nSize > 0) && fixedSize)
    {
        Reset(nSize);
    }
    else
    {
        m_pBuffer = 0;
        m_Fixed = false;
    }
}

ECBufferStream::~ECBufferStream()
{
    if (m_pBuffer) ECMemFree(m_pBuffer);
}

void ECBufferStream::Clear()
{
    m_nDataSize = 0;
    m_pReadPos = m_pBuffer;
    m_pWritePos = m_pBuffer;
}

void ECBufferStream::Reset(EC_U32 nNewSize)
{
    if (m_pBuffer) ECMemFree(m_pBuffer);

    m_nDataSize = 0;
    m_nBufSize = nNewSize;
    m_pBuffer = (char*)ECMemAlloc(nNewSize);
    m_pReadPos = m_pBuffer;
    m_pWritePos = m_pBuffer;
}

EC_U32 ECBufferStream::Read(char* pBuf, EC_U32 nReadSize)
{
    EC_U32 nRet = 0;

    if ((pBuf != NULL) &&
        (m_nDataSize > 0) && (nReadSize > 0))
    {
        EC_U32 nRead = MIN(nReadSize, m_nDataSize);
        if (m_pReadPos >= m_pWritePos)
        {
            int nRightSide = (m_pBuffer + m_nBufSize) - m_pReadPos;
            if (nRightSide > (int)nRead)
            {
                ECMemCopy(pBuf, m_pReadPos, nRead);
                m_pReadPos = m_pReadPos + nRead;
            }
            else if (nRightSide < (int)nRead)
            {
                EC_U32 nLeftSide = nRead - nRightSide;
                ECMemCopy(pBuf, m_pReadPos, nRightSide);
                ECMemCopy(pBuf + nRightSide, m_pBuffer, nLeftSide);
                m_pReadPos = m_pBuffer + nLeftSide;
            }
            else
            {
                ECMemCopy(pBuf, m_pReadPos, nRead);
                m_pReadPos = m_pBuffer;
            }
        }
        else
        {
            ECMemCopy(pBuf, m_pReadPos, nRead);
            m_pReadPos = m_pReadPos + nRead;
        }
        nRet = nRead;
        m_nDataSize = m_nDataSize - nRead;
    }

    return nRet;
}

EC_U32 ECBufferStream::Write(char* pBuf, EC_U32 nWriteSize)
{
    if (!m_Fixed)
        JudgeBufferSize(nWriteSize);

    EC_U32 nRet = 0;
    EC_U32 nFreeSize = m_nBufSize - m_nDataSize;

    if ((pBuf != NULL) &&
        (nWriteSize > 0) && (nFreeSize > 0))
    {
        EC_U32 nWrite = MIN(nWriteSize, nFreeSize);
        if (m_pReadPos <= m_pWritePos)
        {
            int nRightSide = (m_pBuffer + m_nBufSize) - m_pWritePos;
            if (nRightSide > (int)nWrite)
            {
                ECMemCopy(m_pWritePos, pBuf, nWrite);
                m_pWritePos = m_pWritePos + nWrite;
            }
            else if (nRightSide < (int)nWrite)
            {
                EC_U32 nLeftSide = nWrite - nRightSide;
                ECMemCopy(m_pWritePos, pBuf, nRightSide);
                ECMemCopy(m_pBuffer, pBuf + nRightSide, nLeftSide);
                m_pWritePos = m_pBuffer + nLeftSide;
            }
            else
            {
                ECMemCopy(m_pWritePos, pBuf, nWrite);
                m_pWritePos = m_pBuffer;
            }
        }
        else
        {
            ECMemCopy(m_pWritePos, pBuf, nWrite);
            m_pWritePos = m_pWritePos + nWrite;
        }

        nRet = nWrite;
        m_nDataSize = m_nDataSize + nWrite;
    }

    return nRet;
}

EC_U32 ECBufferStream::DataSize() const
{
    return m_nDataSize;
}

EC_U32 ECBufferStream::FreeSize() const
{
    return (m_nBufSize - m_nDataSize);
}

void ECBufferStream::JudgeBufferSize(EC_U32 nSize)
{
    if (nSize > FreeSize())
    {
        char* tmpBuf = NULL;
        EC_U32 tmpSize = m_nDataSize;
        if (tmpSize > 0)
        {
            tmpBuf = (char*)ECMemAlloc(m_nDataSize);
            Read(tmpBuf, m_nDataSize);
        }
        Reset(m_nDataSize + nSize * 2);
        if (tmpSize > 0)
        {
            Write(tmpBuf, tmpSize);
            ECMemFree(tmpBuf);
        }
    }
}
