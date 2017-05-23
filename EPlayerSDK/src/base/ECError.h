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
 * ECError.h
 * This file for define the base error code.
 *
 * Eamil:   epengao@126.com
 * Author:  Gao Peng
 * Version: Intial first version.
 * ****************************************************************
 */

#ifndef EC_ERROR_H
#define EC_ERROR_H

#ifdef __cplusplus
extern "C" {
#endif

#define    EC_Err_None                   0x00000000
#define    EC_Err_BadParam               0x00000001
#define    EC_Err_DataNotExist           0x00000002
#define    EC_Err_NotImplement           0x00000003
#define    EC_Err_FileOpenFaild          0x00000004
#define    EC_Err_OperatorFaild          0x00000005
#define    EC_Err_ContainerFull          0x00000006
#define    EC_Err_ContainerEmpty         0x00000007
#define    EC_Err_DataAlreadyExist       0x00000008

#define    EC_Err_Memory_Base            0x0000F000
#define    EC_Err_Memory_Low             (EC_Err_Memory_Base|0x01)
#define    EC_Err_Memory_Overflow        (EC_Err_Memory_Base|0x02)
#define    EC_Err_Memory_NotAvailable    (EC_Err_Memory_Base|0x03)

#ifdef __cplusplus
} /* extern "C" */
#endif

#endif /* EC_ERROR_H */
