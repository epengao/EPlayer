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
 * ECSUtil.h
 * This program is file for OS depends Util functions.
 *
 * Eamil:   epengao@126.com
 * Author:  Gao Peng
 * Version: Intial first version.
 * ****************************************************************
 */

#ifndef EC_UTIL_H
#define EC_UTIL_H

#include "ECType.h"

template <class T>
inline const T& ECMax(const T& a, const T& b)
{
    return a > b ? a : b;
}

#ifdef __cplusplus
extern "C" {
#endif

/*Sleep millisecond (ms)*/
void ECSleep(EC_U32 nTime);
/*Sleep microsecond (us)*/
void ECUSleep(EC_U32 nTime);

EC_U32 ECGetCPUCores();
EC_U32 ECGetRandNumber();
EC_U32 ECGetSystemTime();
EC_U64 ECGetSystemTimeU();

char* ECGetEnv(const char* pKey);
void ECSetEnv(const char* pKey, const char* pVal);

#ifdef __cplusplus
}
#endif /* __cplusplus */
#endif /* EC_UTIL_H */
