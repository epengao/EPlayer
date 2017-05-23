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
* Inc., 59 Temple Place, Eite 330, Boston, MA  02111-1307  USA
*
* Project: EC < Enjoyable Coding >
*
* MediaEngine.cpp
* This file for EPlayerAPI SDK Wrapper as MediaEngine c++ class.
*
* Eamil:   epengao@126.com
* Author:  Gao Peng
* Version: Intial first version.
* ****************************************************************
*/

#ifndef MEDIA_ENGINE_H
#define MEDIA_ENGINE_H

#include "EPlayerAPI.h"

typedef struct
{
	void* pVideoWnd;
	unsigned int nWndWidth;
	unsigned int nWndHeight;
}VideoOutput;

class MediaEngine
{
public:
    MediaEngine(void *pVideoWnd,
                unsigned int nVideoWndWidth,
                unsigned int nVideoWndHeight);
    ~MediaEngine();
    int  OpenMedia(char *pMediaPath);
    void Play();
    void Pause();
    void Seek(unsigned int nPos);
    void Stop();
    bool HasVideo();
    bool IsPlaying();
    bool HasPlayMedia();
    void GetMediaInfo(MediaInfo *pMediaInfo);
	void SetMediaNotify(MediaNotifier *pNotify);
    unsigned int GetPlayPosition();
    unsigned int GetMediaDuration();
    void UpdateVideoScreen(void *pVideoWnd, unsigned int nVideoWndWidth, unsigned int nVideoWndHeight);

private:
    void VideoScreenSizeAlign();
private:
	EPlayerAPI    m_sMediaAPI;
	MediaInfo     m_sMediaInfo;
    VideoOutput   m_sVideoOutput;
};

#endif /* MEDIA_ENGINE_H */
