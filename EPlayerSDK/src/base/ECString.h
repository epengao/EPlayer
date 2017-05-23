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
 * ECString.h
 * This file for EC string interface define.
 *
 * Eamil:   epengao@126.com
 * Author:  Gao Peng
 * Version: Intial first version.
 * ****************************************************************
 */

#ifndef EC_STRING_H
#define EC_STRING_H

#include "ECType.h"

class ECString
{
public:
    ECString();
    ~ECString();
    ECString(const char* pStr);
    ECString(const ECString &rString);
    ECString(const char* pStr, EC_U32 nSize);
    ECString(const ECString &rString, EC_U32 nSize);

    EC_U32 Trim();
    EC_U32 TrimEnd();
    EC_U32 Length() const;
    bool IsNull() const;
    char* ToCStr() const;
    char& operator[](EC_U32 nPos);
    int FindChar(const char cChar) const;
    const char& operator[](EC_U32 nPos) const;
    ECString& operator=(const char* pStr);
    ECString& operator+(const char* pStr);
    ECString& operator=(const ECString &rString);
    ECString& operator+(const ECString &rString);

public:
    friend bool operator <(const ECString &st1, const ECString &st2);
    friend bool operator >(const ECString &st1, const ECString &st2);
    friend bool operator<=(const ECString &st1, const ECString &st2);
    friend bool operator>=(const ECString &st1, const ECString &st2);
    friend bool operator==(const ECString &st1, const ECString &st2);

private:
    char*     m_pStr;
    EC_U32    m_nSize;
};

#endif /* EC_STRING_H   */
