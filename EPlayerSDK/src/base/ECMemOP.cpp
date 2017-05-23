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
 * ECMemOP.cpp
 * This file for all EC memory operation interface implementation.
 *
 * Eamil:   epengao@126.com
 * Author:  Gao Peng
 * Version: Intial first version.
 * ****************************************************************
 */

#include <stdlib.h>
#include <string.h>
#include "ECType.h"
#include "ECMemOP.h"


EC_PTR ECMemAlloc(EC_U32 uSize)
{
    return malloc(uSize);
}

void ECMemFree(EC_PTR pMem)
{
    free(pMem);
}

void ECMemSet(EC_PTR pMem, int val, EC_U32 uSize)
{
    memset(pMem, val, uSize);
}

void ECMemCopy(EC_PTR pDest, EC_PTR pSrc, EC_U32 uSize)
{
    memcpy(pDest, pSrc, uSize);
}

int ECMemCompare(EC_PTR pBuf1, EC_PTR pBuf2, EC_U32 uSize)
{
    return memcmp(pBuf1, pBuf2, uSize);
}

void ECMemMove(EC_PTR pDest, EC_PTR pSrc, EC_U32 uSize)
{
    memmove (pDest, pSrc, uSize);
}
