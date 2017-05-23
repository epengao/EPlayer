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
 * ECLog.h
 * This file for all log output hreader file define.
 *
 * Eamil:   epengao@126.com
 * Author:  Gao Peng
 * Version: Intial first version.
 * ****************************************************************
 */

#ifndef EC_LOG_H
#define EC_LOG_H

#ifdef __cplusplus
extern "C" {
#endif
    
#include "ECOSDef.h"

#ifdef EC_OS_Win32
#include <stdio.h>
#elif defined EC_OS_Linux
#include <stdio.h>
#elif defined EC_OS_MacOS
#include <stdio.h>
#elif defined EC_OS_iOS
#include <stdio.h>
#elif defined EC_OS_Android
#include <android/log.h>
#endif

#ifdef EC_OS_Win32
#define EC_log_out(fmt, ...)    printf(fmt, ##__VA_ARGS__)
#elif defined EC_OS_Linux
#define EC_log_out(fmt, ...)    printf(fmt, ##__VA_ARGS__)
#elif defined EC_OS_MacOS
#define EC_log_out(fmt, ...)    printf(fmt, ##__VA_ARGS__)
#elif defined EC_OS_iOS
#define EC_log_out(fmt, ...)    printf(fmt, ##__VA_ARGS__)
#elif defined EC_OS_Android
#define EC_log_out(fmt, ...)    __android_log_print(ANDROID_LOG_INFO, "ECAudioPlayer", fmt, ##__VA_ARGS__ )
#endif

/* Following is the detail log output define */
#define __EC_LOGI(format, ...)                                  \
    do                                                          \
    {                                                           \
        EC_log_out("EC_Info: %s %s %d " format "\n",            \
              __FILE__, __FUNCTION__, __LINE__, ##__VA_ARGS__ );\
    } while (0)

#define __EC_LOGW(format, ...)                                  \
    do                                                          \
    {                                                           \
        EC_log_out("EC_Waring: %s %s %d " format "\n",          \
              __FILE__, __FUNCTION__, __LINE__, ##__VA_ARGS__ );\
    } while (0)

#define __EC_LOGE(format, ...)                                  \
    do                                                          \
    {                                                           \
        EC_log_out("EC_Error: %s %s %d " format "\n",           \
              __FILE__, __FUNCTION__, __LINE__, ##__VA_ARGS__ );\
    } while (0)


/* Following is the simple output log define */
#define __S_EC_LOGI(format, ...)                                \
    do                                                          \
    {                                                           \
    EC_log_out("Info:  " format "\n", ##__VA_ARGS__ );          \
} while (0)

#define __S_EC_LOGW(format, ...)                                \
    do                                                          \
    {                                                           \
    EC_log_out("Warring:  " format "\n", ##__VA_ARGS__ );       \
} while (0)

#define __S_EC_LOGE(format, ...)                                \
    do                                                          \
    {                                                           \
    EC_log_out("Err:  " format "\n", ##__VA_ARGS__ );           \
} while (0)

/* EC log macro define */
#define ECLogI    __EC_LOGI
#define ECLogW    __EC_LOGW
#define ECLogE    __EC_LOGE

#define ecLogI   __S_EC_LOGI
#define ecLogW   __S_EC_LOGW
#define ecLogE   __S_EC_LOGE


#ifdef __cplusplus
}
#endif /* __cplusplus */
#endif /* EC_LOG_H */
