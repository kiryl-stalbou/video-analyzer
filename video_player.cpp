#include "video_player.h"
#include "utils.h"



VideoPlayer::VideoPlayer(HWND hwnd, VideoInfo videoInfo) : _hwnd(hwnd), _videoInfo(videoInfo), _videoReader(videoInfo)
{
    InitializeCriticalSection(&_criticalSection);
    InitializeConditionVariable(&_conditionVariable);

    _thread = CreateThread(nullptr, 0, _threadLoop, this, 0, nullptr);

    _showFrame();
}

VideoPlayer::~VideoPlayer() 
{
    EnterCriticalSection(&_criticalSection);

    _isActive = false;

    LeaveCriticalSection(&_criticalSection);
    WakeAllConditionVariable(&_conditionVariable);
    
    WaitForSingleObject(_thread, INFINITE);
    CloseHandle(_thread);
 
    DeleteCriticalSection(&_criticalSection);
}

void VideoPlayer::play() 
{
    if (_isPlaying) return;

    _isPlaying = true;

    WakeConditionVariable(&_conditionVariable);
}

void VideoPlayer::pause()
{
    if (!_isPlaying) return;

    _isPlaying = false;
}

void VideoPlayer::reset()
{
    seek(0);
}

void VideoPlayer::seek(UINT32 frameIndex)
{
    _videoReader.seekFrame(frameIndex);

    if (!_isPlaying) _showFrame();
}

void VideoPlayer::speed(float speed)
{
    _speed = speed;
}

bool VideoPlayer::toggleFlip()
{
    _shouldFlip = !_shouldFlip;

    if (!_isPlaying) _showFrame();

    return _shouldFlip;
}

bool VideoPlayer::toggleEffects()
{
    _shouldApplyEffects = !_shouldApplyEffects;

    if (!_isPlaying) _showFrame();

    return _shouldApplyEffects;
}

void VideoPlayer::blend(Color color)
{
    _blendColor = color;

    if (_shouldApplyEffects && !_isPlaying) _showFrame();
}

DWORD VideoPlayer::_threadLoop(LPVOID param) 
{
    VideoPlayer* player = static_cast<VideoPlayer*>(param);

    while (player->_isActive)
    {
        if (player->_canShowFrame())
        {
            player->_showFrame();

            Sleep(static_cast<DWORD>(player->_videoInfo.frameDuration / player->_speed));
        }
    }

    return 0;
}

bool VideoPlayer::_canShowFrame()
{
    EnterCriticalSection(&_criticalSection);

    while (_isActive && !_isPlaying)
    {
        SleepConditionVariableCS(&_conditionVariable, &_criticalSection, INFINITE);
    }

    LeaveCriticalSection(&_criticalSection);

    return _isActive && _isPlaying;
}

void VideoPlayer::_showFrame()
{
    BYTE* bytes = nullptr;

    if (!_readFrame(&bytes))
    {
        _isPlaying = false;
        return;
    }

    if (_shouldApplyEffects)
    {
        _applyEffects(bytes);
    }

    _drawFrame(bytes);

    if (onDrawFrame)
    {
        onDrawFrame(_videoReader.currentFrameIndex);
    }

    delete[] bytes;
}

bool VideoPlayer::_readFrame(BYTE** bytes)
{
    HRESULT hr = S_OK;

    hr = _videoReader.readFrame(bytes);

    return SUCCEEDED(hr);
}

void VideoPlayer::_applyEffects(BYTE* bytes) const
{
    for (UINT32 i = 0; i < _videoInfo.width * _videoInfo.height; ++i)
    {
        BYTE* pixel = bytes + i * 4;

        BYTE alpha = _blendColor.alpha;
        BYTE red = pixel[2];
        BYTE green = pixel[1];
        BYTE blue = pixel[0];

        pixel[2] = (red * alpha + _blendColor.red * (255 - alpha)) / 255;
        pixel[1] = (green * alpha + _blendColor.green * (255 - alpha)) / 255;
        pixel[0] = (blue * alpha + _blendColor.blue * (255 - alpha)) / 255;
    }
}

void VideoPlayer::_drawFrame(BYTE* bytes) 
{
    BITMAPINFO bmi = {};
    bmi.bmiHeader.biPlanes = 1;
    bmi.bmiHeader.biBitCount = 32;
    bmi.bmiHeader.biCompression = BI_RGB;
    bmi.bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
    bmi.bmiHeader.biWidth = _videoInfo.width;
    bmi.bmiHeader.biHeight = _videoInfo.height * (_shouldFlip ? 1 : -1);

    RECT hwndRect;
    GetClientRect(_hwnd, &hwndRect);
    int hwndWidth = hwndRect.right - hwndRect.left;
    int hwndHeight = hwndRect.bottom - hwndRect.top;

    HDC hdc = GetDC(_hwnd);
    
    StretchDIBits(hdc, 0, 0, hwndWidth, hwndHeight, 0, 0, _videoInfo.width, _videoInfo.height, bytes, &bmi, DIB_RGB_COLORS, SRCCOPY);

    ReleaseDC(_hwnd, hdc);
}