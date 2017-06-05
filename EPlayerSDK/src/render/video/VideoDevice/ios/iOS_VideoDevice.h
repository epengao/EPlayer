/*
* This software is developed for study and improve coding skill ...
*
* Project:  Enjoyable Coding< EC >
* Copyright (C) 2014-2016 Gao Peng

* This library is free software; you can redistribute it and/or
* modify it under the terms of the GNU Library General Public
* License as published by the Free Software Foundation; either
* version 2 of the License, or (at your option) any later version.

* This library is distributed in the hope that it will be useful,
* but WITHOUT ANY WARRANTY; without even the implied warranty of
* MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
* Library General Public License for more details.

* You should have received a copy of the GNU Library General Public
* License along with this library; if not, write to the Free
* Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA

* ---------------------------------------------------------------------
* iOS_VideoRender.h
* Here we define a class to Package the video iOS render APIs, set all these
* APIs as a class, and this will be used in VideoRender class at iOS platform.
*
* Eamil:   epengao@126.com
* Author:  Peter Gao
* Version: Intial first version.
* --------------------------------------------------------------------
*/

#ifndef IOS_VIDEO_RENDER_H
#define IOS_VIDEO_RENDER_H

#include "ECType.h"
#include "Source.h"
#include "VideoDec.h"
#include "VideoRend.h"
#include "VideoRenderDeviceI.h"
extern "C"
{
#include "libavcodec/avcodec.h"
#include "libswscale/swscale.h"
};

class iOS_VideoRender : public VideoRenderDeviceI
{
public:
    iOS_VideoRender();
    ~iOS_VideoRender();
public:
    EC_U32 Init(MediaCtxInfo* pMediaInfo,
                EC_U32 nScreenWidth, 
                EC_U32 nScreenHight, 
                EC_VOIDP pDrawable);
    EC_VOID Stop();
    EC_VOID Uninit();
    EC_VOID ClearScreen();
    EC_VOID DrawFrame(VideoFrame* pFrame);
    EC_VOID UpdateVideoScreen(MediaEngVideoScreen *pVideoScreen);
private:
    EC_U32              m_nVideoWidth;
    EC_U32              m_nVideoHeight;
    AVPicture           m_sAVPicture;
    EC_VOIDP            m_pVideoScreen;
    struct SwsContext*  m_pImgConvertCtx;
};

#undef IOS_UIKIT_INCLUDE
#endif /* IOS_VIDEO_RENDER_H */
