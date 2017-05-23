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
 * ECRingQueue.h
 * This file defines a Ring Queue which can store any type of data.
 *
 * Eamil:   epengao@126.com
 * Author:  Gao Peng
 * Version: Intial first version.
 * ****************************************************************
 */

#ifndef EC_RING_QUEUE_H
#define EC_RING_QUEUE_H

#include "ECType.h"
#include "ECError.h"

#define EC_RING_QUEUE_DFT_SIZE 512

template<typename T>
class ECRingQueue
{
public:
    ECRingQueue(EC_U32 nSize=EC_RING_QUEUE_DFT_SIZE);
    ~ECRingQueue();
public:
    EC_U32 Push(T data);
    EC_U32 Pop(T *data);
    EC_U32 Front(T *pData) const;
    EC_U32 Back(T *pData)  const;
    bool   IsFull() const;
    bool   IsEmpty() const;
    EC_U32 Count() const;
    EC_U32 Size() const;
    void   Clean();

private:
    EC_U32 m_nSize;
    EC_U32 m_nHead;
    EC_U32 m_nTail;
    EC_U32 m_nItemsCount;
    T*     m_pData;
};

template<typename T>
ECRingQueue<T>::ECRingQueue(
EC_U32 nSize /*=EC_RING_QUEUE_DFT_SIZE*/)
:m_nSize(nSize)
,m_nHead(0)
,m_nTail(0)
,m_nItemsCount(0)
,m_pData(NULL)
{
    m_pData = new T[m_nSize];
}

template<typename T>
ECRingQueue<T>::~ECRingQueue()
{
    if (m_pData) delete[]m_pData;
}

template<typename T>
EC_U32 ECRingQueue<T>::Push(T data)
{
    if ((m_nItemsCount >= m_nSize) || (NULL == m_pData))
        return EC_Err_ContainerFull;

    EC_U32 nPos = (m_nTail % m_nSize);
    m_pData[nPos] = data;
    m_nTail++;
    m_nItemsCount++;

    return EC_Err_None;
}

template<typename T>
EC_U32 ECRingQueue<T>::Pop(T *pOutData)
{
    if ((m_nItemsCount == 0) || (NULL == m_pData))
        return EC_Err_ContainerEmpty;
    if (NULL == pOutData)
        return EC_Err_BadParam;

    EC_U32 nPos = (m_nHead % m_nSize);
    *pOutData = m_pData[nPos];
    m_nHead++;
    m_nItemsCount--;

    return EC_Err_None;
}

template<typename T>
EC_U32 ECRingQueue<T>::Front(T *pOutData) const
{
    if ((m_nItemsCount == 0) || (NULL == m_pData))
        return EC_Err_ContainerEmpty;

    EC_U32 nPos = (m_nHead % m_nSize);
    *pOutData = m_pData[nPos];

    return EC_Err_None;
}

template<typename T>
EC_U32 ECRingQueue<T>::Back(T *pOutData) const
{
    if ((m_nItemsCount == 0) || (NULL == m_pData))
        return EC_Err_ContainerEmpty;

    EC_U32 nPos = (m_nTail-1 % m_nSize);
    *pOutData = m_pData[nPos];

    return EC_Err_None;
}

template<typename T>
bool ECRingQueue<T>::IsFull() const
{
    return (m_nItemsCount >= m_nSize);
}

template<typename T>
bool ECRingQueue<T>::IsEmpty() const
{
    return (0 == m_nItemsCount);
}

template<typename T>
EC_U32 ECRingQueue<T>::Count() const
{
    return m_nItemsCount;
}

template<typename T>
EC_U32 ECRingQueue<T>::Size() const
{
    return m_nSize;
}

template<typename T>
void ECRingQueue<T>::Clean()
{
    m_nHead = 0;
    m_nTail = 0;
    m_nItemsCount = 0;
}

#endif /* EC_RING_QUEUE_H */
