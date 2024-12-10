#pragma once

#include <string>
#include <windows.h>



using std::wstring;

struct VideoInfo {
    wstring filePath;
    wstring fileName;
    wstring fileType;
    wstring codecName;
    uint64_t fileSize;    // Bytes
    UINT32 width;
    UINT32 height;
    UINT32 bitRate;       // Kbps
    UINT32 frameCount;
    double duration;      // Seconds
    double frameRate;     // Fps
    double frameDuration; // Milliseconds 
};