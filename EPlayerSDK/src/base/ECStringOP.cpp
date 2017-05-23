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
 * ECStringOP.cpp
 * This file for redefing all EC string operation interface implement.
 *
 * Eamil:   epengao@126.com
 * Author:  Gao Peng
 * Version: Intial first version.
 * ****************************************************************
 */

#include <stdio.h>
#include <stdlib.h>
#include <wchar.h>
#include "ECStringOP.h"


EC_U32 ECStrLen(const char* pStr)
{
    return (EC_U32)strlen(pStr);
}

char ECToUpper(char cChar)
{
    char cRet = cChar;
    if(('a'<=cChar) && (cChar<='z'))
        cRet = (cChar-32);
    return cRet;
}

char ECToLower(char cChar)
{
    char cRet = cChar;
    if(('A'<=cChar) && (cChar<='Z'))
        cRet = (cChar+32);
    return cRet;
}

char* ECToUpperStr(char* pStr)
{
    char* pRet = pStr;
    while((*pStr) != '\0')
    {
        *pStr = ECToUpper(*pStr);
        pStr++;
    }
    return pRet;
}

char* ECToLowerStr(char* pStr)
{
    char* pRet = pStr;
    while((*pStr) != '\0')
    {
        *pStr = ECToLower(*pStr);
        pStr++;
    }
    return pRet;
}

char* ECStrCopy(char* pDest, const char* pSrc)
{
    return strcpy(pDest, pSrc);
}

char* ECStrNCopy(char* pDest, const char* pSrc, EC_U32 uSize)
{
    return strncpy(pDest, pSrc, uSize);
}

char* ECStrCat(char* pDest, const char* pSrc)
{
    return strcat(pDest, pSrc);
}

char* ECStrAddPrefix(char* pStr, const char* pPrefix)
{
    char* pNewStr = new char[ECStrLen(pStr) +
                             ECStrLen(pPrefix) + 1];
    ECStrCopy(pNewStr, pPrefix);
    ECStrCat(pNewStr, pStr);
    delete []pStr;
    pStr = pNewStr;
    return pStr;
}

char* ECStrInsertChar(char* pStr, char cChar, EC_U32 uIndex)
{
    EC_U32 uLength = ECStrLen(pStr);
    if(uIndex > uLength)
        return NULL;

    char* pNewStr = new char[uLength + 2];
    if((0 < uIndex) && (uIndex < uLength))
    {
        pNewStr[uIndex] = cChar;
        ECStrNCopy(pNewStr, pStr, uIndex);
        ECStrCopy(pNewStr + uIndex + 1, pStr + uIndex);
    }
    else if (uLength == uIndex)
    {
        ECStrCopy(pNewStr, pStr);
        pNewStr[uLength] = cChar;
        pNewStr[uLength+1] = '\0';
    }
    else if(0 == uIndex)
    {
        pNewStr[0] = cChar;
        ECStrCopy(pNewStr + 1, pStr);
    }

    delete pStr;
    pStr = pNewStr;
    return pStr;
}

char* ECStrInsertStr(char* pStr, const char* pInsert, EC_U32 uIndex)
{
    EC_U32 uSrcLength = ECStrLen(pStr);
    EC_U32 uInsertLength = ECStrLen(pInsert);
    EC_U32 uTotalLength = uSrcLength + uInsertLength;

    if(uIndex > uSrcLength)
        return NULL;

    char* pNewStr = new char[uTotalLength + 1];
    if((0 < uIndex) && (uIndex < uSrcLength))
    {
        ECStrCopy(pNewStr + uIndex, pInsert);
        ECStrNCopy(pNewStr, pStr, uSrcLength);
        ECStrCopy(pNewStr + uInsertLength, pStr + uIndex);
    }
    else if (uSrcLength == uIndex)
    {
        ECStrCat(pNewStr, pInsert);
    }
    else if(0 == uIndex)
    {
        ECStrCopy(pNewStr, pInsert);
        ECStrCopy(pNewStr + uInsertLength, pStr);
    }

    delete pStr;
    pStr = pNewStr;
    return pStr;
}

char* ECStrStr(char* pDest, char* pFind)
{
    return strstr(pDest, pFind);
}

char* ECStrSet(char* pStr, char cChar)
{
    memset(pStr, cChar, ECStrLen(pStr));
    return pStr;
}

int ECStrCompare(char* pStr1, char* pStr2)
{
    return strcmp(pStr1, pStr2);
}

int ECStrNCompare(char* pStr1, char* pStr2, EC_U32 uSize)
{
    return strncmp(pStr1, pStr2, uSize);
}

int ECStrCaseCompare(char* pStr1, char* pStr2)
{
    int nRet;
    char* pTmpStr1 = new char[ECStrLen(pStr1) + 1];
    char* pTmpStr2 = new char[ECStrLen(pStr2) + 1];
    ECStrCopy(pTmpStr1, pStr1);
    ECStrCopy(pTmpStr2, pStr2);
    nRet = ECStrCompare(ECToLowerStr(pTmpStr1),
                      ECToLowerStr(pTmpStr2));
    delete []pTmpStr1;
    delete []pTmpStr2;
    return nRet;
}

int ECStrNCaseCompare(char* pStr1, char* pStr2, EC_U32 uSize)
{
    int nRet;
    char* pTmpStr1 = new char[ECStrLen(pStr1) + 1];
    char* pTmpStr2 = new char[ECStrLen(pStr2) + 1];
    ECStrCopy(pTmpStr1, pStr1);
    ECStrCopy(pTmpStr2, pStr2);
    nRet = ECStrNCompare(ECToLowerStr(pTmpStr1),
                       ECToLowerStr(pTmpStr2), uSize);
    delete []pTmpStr1;
    delete []pTmpStr2;
    return nRet;
}

int ECStrToInt(const char* pStr)
{
    return atoi(pStr);
    return 0;
}

char* ECIntToStr(char* pStr, int nInt)
{
    sprintf(pStr, "%d", nInt);
    return pStr;
}

char* ECWideStrToCStr(char* pStr, const EC_WCHAR* pWstr)
{
    wcstombs(pStr, pWstr, wcslen(pWstr));
    return pStr;
}

EC_WCHAR* ECCStrToWideStr(EC_WCHAR* pWstr, const char* pStr)
{
    mbstowcs(pWstr, pStr, ECStrLen(pStr));
    return pWstr;
}
