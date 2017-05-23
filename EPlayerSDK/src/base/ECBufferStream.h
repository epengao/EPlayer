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

#include "ECType.h"

#ifndef EC_BUFFER_STREAM_H
#define EC_BUFFER_STREAM_H

class ECBufferStream
{
public:
    ECBufferStream(EC_U32 nSize = 0, bool fixedSize = false);
    ~ECBufferStream();

    void Clear();
    void Reset(EC_U32 nNewSize);
    EC_U32 Read(char* pBuf, EC_U32 nReadSize);
    EC_U32 Write(char* pBuf, EC_U32 nWriteSize);
    EC_U32 DataSize() const;
    EC_U32 FreeSize() const;

private:
    void JudgeBufferSize(EC_U32 nSize);

private:
    bool  m_Fixed;
    char* m_pBuffer;
    char* m_pReadPos;
    char* m_pWritePos;
    EC_U32 m_nBufSize;
    EC_U32 m_nDataSize;
};

#endif /* EC_BUFFER_STREAM_H */