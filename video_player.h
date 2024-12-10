#pragma once

#include <string>
#include <functional>
#include <windows.h>

#include "color.h"
#include "video_info.h"
#include "video_reader.h"



using std::function;

class VideoPlayer {
public:

    explicit VideoPlayer(HWND hwnd, VideoInfo videoInfo);
    ~VideoPlayer();

    void play();
    void pause();
    void reset();
    void seek(UINT32 frameIndex);
    bool toggleFlip();
    bool toggleEffects();
    void blend(Color color);
    void speed(float speed);

    function<void(UINT32)> onDrawFrame;

private:
    
    static DWORD _threadLoop(LPVOID param);

    bool _canShowFrame();
    void _showFrame();
    bool _readFrame(BYTE** bytes);
    void _applyEffects(BYTE* bytes) const;
    void _drawFrame(BYTE* bytes);

    HWND _hwnd;
    HANDLE _thread;
    Color _blendColor;
    VideoInfo _videoInfo;
    VideoReader _videoReader;

    float _speed = 1.0;
    volatile bool _isActive = true;
    volatile bool _isPlaying = false;
    volatile bool _shouldFlip = false;
    volatile bool _shouldApplyEffects = false;

    CRITICAL_SECTION _criticalSection;
    CONDITION_VARIABLE _conditionVariable;

};
