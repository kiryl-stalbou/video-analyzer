#pragma once

#include <mfidl.h>
#include <mfreadwrite.h>

#include "video_info.h"



class VideoReader {
public:

    explicit VideoReader(VideoInfo videoInfo);
    ~VideoReader();

    HRESULT readFrame(BYTE** bytes);
    HRESULT seekFrame(UINT32 frameIndex);

    UINT32 currentFrameIndex = 0;

private:

    HRESULT _configureSourceReader();
    HRESULT _selectSourceVideoStream();

    VideoInfo _videoInfo;
    IMFSourceReader* _imfSourceReader;

};
