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
 * ECStringOP.h
 * This file for redefing all EC string operation interface define.
 *
 * Eamil:   epengao@126.com
 * Author:  Gao Peng
 * Version: Intial first version.
 * ****************************************************************
 */

#include <string.h>
#include "ECType.h"

#ifdef __cplusplus
extern "C" {
#endif

EC_U32 ECStrLen(const char* pStr);

char   ECToUpper(char cChar);
char   ECToLower(char cChar);

char*  ECToUpperStr(char* pStr);
char*  ECToLowerStr(char* pStr);
char*  ECStrCopy(char* pDest, const char* pSrc);
char*  ECStrNCopy(char* pDest, const char* pSrc, EC_U32 uSize);
char*  ECStrCat(char* pDest, const char* pSrc);
char*  ECStrAddPrefix(char* pStr, const char* pPrefix);
char*  ECStrInsertChar(char* pStr, char cChar, EC_U32 uIndex);
char*  ECStrInsertStr(char* pStr, const char* pInsert, EC_U32 uIndex);
char*  ECStrStr(char* pDest, char* pFind);
char*  ECStrSet(char* pStr, char cChar);
char*  ECIntToStr(char* pStr, int nInt);
char*  ECWideStrToCStr(char* pStr, EC_WCHAR* pWstr);
char*  ECCStrToWideStr(EC_WCHAR* pWstr, char* pStr);

int    ECStrToInt(const char* pStr);
int    ECStrCompare(char* pStr1, char* pStr2);
int    ECStrNCompare(char* pStr1, char* pStr2, EC_U32 uSize);
int    ECStrCaseCompare(char* pStr1, char* pStr2);
int    ECStrNCaseCompare(char* pStr1, char* pStr2, EC_U32 uSize);

#ifdef __cplusplus
}
#endif
