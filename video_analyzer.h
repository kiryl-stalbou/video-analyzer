#pragma once

#include <string>
#include <windows.h>

#include "video_info.h"



using std::wstring;

class VideoAnalyzer {
public:

    static HRESULT readInfo(const wstring& filePath, VideoInfo& videoInfo);

private:

    static wstring _codecNameBy(const GUID& codecGuid);

};
