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
 * ECString.cpp
 * This file for EC string interface implementation.
 *
 * Eamil:   epengao@126.com
 * Author:  Gao Peng
 * Version: Intial first version.
 * ****************************************************************
 */

#include "ECError.h"
#include "ECString.h"
#include "ECStringOP.h"

ECString::ECString()
:m_pStr(NULL)
,m_nSize(0)
{
}

ECString::~ECString()
{
    if(m_pStr) delete []m_pStr;
}

ECString::ECString(const char* pStr)
:m_pStr(NULL)
,m_nSize(0)
{
    if(pStr)
    {
        m_nSize = ECStrLen(pStr);
        m_pStr = new char[m_nSize + 1];
        if(m_pStr)
            ECStrCopy(m_pStr, pStr);
    }
}

ECString::ECString(const ECString &rString)
:m_pStr(NULL)
{
    m_nSize = rString.m_nSize;
    if(m_nSize > 0)
    {
        m_pStr = new char[m_nSize + 1];
        if(m_pStr)
            ECStrCopy(m_pStr, rString.m_pStr);
    }
}

ECString::ECString(const char* pStr, EC_U32 nSize)
:m_pStr(NULL)
,m_nSize(nSize)
{
    m_pStr = new char[nSize+1];
    if(m_pStr)
    {
        m_nSize = nSize;
        ECStrNCopy(m_pStr, pStr, nSize);
        m_pStr[m_nSize] = 0;
    }
}

ECString::ECString(const ECString &rString, EC_U32 nSize)
:m_pStr(NULL)
,m_nSize(nSize)
{
    m_pStr = new char[nSize+1];
    if(m_pStr)
    {
        m_nSize = nSize;
        ECStrNCopy(m_pStr, rString.ToCStr(), nSize);
        m_pStr[m_nSize] = 0;
    }
}

EC_U32 ECString::Trim()
{
    if(m_pStr)
    {
        EC_U32 nTrimN = 0;
        char* pTmp = NULL;
        while(' ' == m_pStr[nTrimN])
            nTrimN++;
        EC_U32 nSize = m_nSize - nTrimN;
        pTmp = new char[nSize + 1];
        if(pTmp)
        {
            ECStrCopy(pTmp, m_pStr+nTrimN);
            delete []m_pStr;
            m_pStr = pTmp;
            m_nSize = nSize;
        }
    }

    return EC_Err_None;
}

EC_U32 ECString::TrimEnd()
{
    if(m_pStr)
    {
        EC_U32 nTrim = 0;
        for(int n = m_nSize-1; n >= 0; --n)
        {
            if(('\n'  != m_pStr[n]) && (' ' != m_pStr[n]))
                break;
            nTrim++;
        }

        if(nTrim > 0)
        {
            m_nSize = m_nSize - nTrim;
            m_pStr[m_nSize] = 0;
        }
    }

    return EC_Err_None;
}

EC_U32 ECString::Length() const
{
    return m_nSize;
}

bool ECString::IsNull() const
{
    return (NULL == m_pStr);
}

char* ECString::ToCStr() const
{
    return m_pStr;
}

char& ECString::operator[](EC_U32 nPos)
{
    return m_pStr[nPos];
}

int ECString::FindChar(const char cChar) const
{
    int nRet = -1;
    for(EC_U32 n = 0; n < m_nSize; ++n)
    {
        if(cChar == m_pStr[n])
        {
            nRet = n;
            break;
        }
    }

    return nRet;
}

const char& ECString::operator[](EC_U32 nPos) const
{
    return m_pStr[nPos];
}

ECString& ECString::operator=(const char* pStr)
{
    if(m_pStr)
    {
        delete []m_pStr;
        m_pStr = NULL;
        m_nSize = 0;
    }
    if(pStr)
    {
        m_nSize = ECStrLen(pStr);
        m_pStr = new char[m_nSize + 1];
        ECStrCopy(m_pStr, pStr);
    }

    return (*this);
}

ECString& ECString::operator+(const char* pStr)
{
    if(pStr)
    {
        EC_U32 nSize = m_nSize + ECStrLen(pStr);
        char* pTmp = new char[nSize + 1];
        ECStrCopy(pTmp, m_pStr);
        ECStrCopy(pTmp+m_nSize, pStr);
        delete []m_pStr;
        m_pStr = pTmp;
        m_nSize = nSize;
    }

    return (*this);
}

ECString& ECString::operator=(const ECString &rString)
{
    if(this == &rString)
        return (*this);

    if(m_pStr)
        delete []m_pStr;
    m_nSize = rString.m_nSize;
    if(m_nSize > 0)
    {
        m_pStr = new char[ECStrLen(rString.m_pStr) + 1];
        if(m_pStr)
            ECStrCopy(m_pStr, rString.m_pStr);
        else
            m_nSize = 0;
    }
    else
    {
        m_nSize = 0;
        m_pStr = NULL;
    }
    return (*this);
}

ECString& ECString::operator+(const ECString &rString)
{
    if(rString.m_nSize > 0)
    {
        EC_U32 nSize = m_nSize + rString.m_nSize;
        char* pTmp = new char[nSize + 1];
        ECStrCopy(pTmp, m_pStr);
        ECStrCopy(pTmp+m_nSize, rString.m_pStr);
        delete []m_pStr;
        m_pStr = pTmp;
        m_nSize = nSize;
    }

    return (*this);
}

bool operator<(const ECString &st1, const ECString &st2)
{
    return (ECStrCompare(st1.m_pStr, st2.m_pStr)<0);
}

bool operator>(const ECString &st1, const ECString &st2)
{
    return (ECStrCompare(st1.m_pStr, st2.m_pStr)>0);
}

bool operator>=(const ECString &st1, const ECString &st2)
{
    return (ECStrCompare(st1.m_pStr, st2.m_pStr)>=0);
}

bool operator<=(const ECString &st1, const ECString &st2)
{
    return (ECStrCompare(st1.m_pStr, st2.m_pStr)<=0);
}

bool operator==(const ECString &st1, const ECString &st2)
{
    return (ECStrCompare(st1.m_pStr, st2.m_pStr)==0);
}
