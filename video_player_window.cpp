#include <string>
#include <algorithm>
#include <windows.h>
#include <windowsx.h>
#include <commctrl.h>

#include "video_player_window.h"
#include "video_info_window.h"
#include "video_analyzer.h"
#include "video_player.h"
#include "video_info.h"
#include "Resource.h"



using std::wstring, std::min;

namespace VideoPlayerWindow
{

    //
    // Variables
    //

    VideoInfo _videoInfo;

    VideoPlayer* videoPlayer = nullptr;

    HWND hVideoPathStatic, hVideoAreaStatic, hSelectVideoFileButton, hShowVideoInfoButton, hStartVideoButton, hPauseVideoButton, hResetVideoButton, hFlipVideoCheckbox, hApplyVideoEffectsCheckbox, hAlphaTextBox, hRedTextBox, hGreenTextBox, hBlueTextBox, hAlphaLabel, hRedLabel, hGreenLabel, hBlueLabel, hSpeedVideoSlider, hSeekVideoSlider;



    //
    // Window Setup
    //

    bool init(HINSTANCE hInstance, int nCmdShow)
    {
        WNDCLASSEXW wcexw;
        wcexw.cbSize = sizeof(WNDCLASSEX);
        wcexw.cbClsExtra = 0;
        wcexw.cbWndExtra = 0;
        wcexw.lpfnWndProc = wndProc;
        wcexw.lpszMenuName = nullptr;
        wcexw.hInstance = hInstance;
        wcexw.lpszClassName = L"Video_Player_Class";
        wcexw.style = CS_HREDRAW | CS_VREDRAW;
        wcexw.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);
        wcexw.hCursor = LoadCursor(nullptr, IDC_ARROW);
        wcexw.hIcon = LoadIcon(hInstance, MAKEINTRESOURCE(IDC_VIDEOANALYZER));
        wcexw.hIconSm = LoadIcon(wcexw.hInstance, MAKEINTRESOURCE(IDI_SMALL));

        RegisterClassExW(&wcexw);

        HWND hWnd = CreateWindowW(
            L"Video_Player_Class", L"Video Player", WS_OVERLAPPEDWINDOW,
            CW_USEDEFAULT, 0, CW_USEDEFAULT, 0,
            nullptr, nullptr, hInstance, nullptr
        );

        if (!hWnd) return false;

        ShowWindow(hWnd, nCmdShow);
        UpdateWindow(hWnd);

        return true;
    }

    int runMessageLoop()
    {
        MSG msg;

        while (GetMessage(&msg, nullptr, 0, 0))
        {
            TranslateMessage(&msg);
            DispatchMessage(&msg);
        }

        return (int)msg.wParam;
    }

    LRESULT CALLBACK wndProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
    {
        switch (uMsg)
        {
            HANDLE_MSG(hwnd, WM_GETMINMAXINFO, handleGetMinMaxInfo);
            HANDLE_MSG(hwnd, WM_CREATE, handleCreate);
            HANDLE_MSG(hwnd, WM_COMMAND, handleCommand);
            HANDLE_MSG(hwnd, WM_HSCROLL, handleHScroll);
            HANDLE_MSG(hwnd, WM_CLOSE, handleClose);
        }

        return DefWindowProc(hwnd, uMsg, wParam, lParam);
    }



    //
    // Window Lifecycle
    //

    void handleGetMinMaxInfo(HWND hWnd, LPMINMAXINFO mmi)
    {
        mmi->ptMinTrackSize = WINDOW_SIZE;
        mmi->ptMaxTrackSize = WINDOW_SIZE;
    }

    bool handleCreate(HWND hwnd, LPCREATESTRUCT lpCreateStruct)
    {
        HWND hControls[] = {
            hSelectVideoFileButton = CreateWindow(L"BUTTON", L"Select Video File", WS_VISIBLE | WS_CHILD | BS_FLAT,
                10, 10, 150, 40, hwnd, (HMENU)ID_SELECT_VIDEO, NULL, NULL),

            hShowVideoInfoButton = CreateWindow(L"BUTTON", L"Show Video Info", WS_VISIBLE | WS_CHILD | BS_FLAT,
                170, 10, 150, 40, hwnd, (HMENU)ID_SHOW_VIDEO_INFO, NULL, NULL),

            hVideoPathStatic = CreateWindow(L"STATIC", L"No video selected", WS_VISIBLE | WS_CHILD | SS_CENTER,
                330, 20, 300, 20, hwnd, NULL, NULL, NULL),

            hVideoAreaStatic = CreateWindow(L"STATIC", NULL, WS_VISIBLE | WS_CHILD | SS_BLACKFRAME,
                10, 60, 1280, 720, hwnd, NULL, NULL, NULL),

            hStartVideoButton = CreateWindow(L"BUTTON", L"Play", WS_VISIBLE | WS_CHILD | BS_FLAT,
                10, 800, 150, 40, hwnd, (HMENU)ID_PLAY_VIDEO, NULL, NULL),

            hPauseVideoButton = CreateWindow(L"BUTTON", L"Pause", WS_VISIBLE | WS_CHILD | BS_FLAT,
                170, 800, 150, 40, hwnd, (HMENU)ID_PAUSE_VIDEO, NULL, NULL),

            hResetVideoButton = CreateWindow(L"BUTTON", L"Reset", WS_VISIBLE | WS_CHILD | BS_FLAT,
                330, 800, 150, 40, hwnd, (HMENU)ID_RESET_VIDEO, NULL, NULL),

            hFlipVideoCheckbox = CreateWindow(L"BUTTON", L"Flip", WS_VISIBLE | WS_CHILD | SS_CENTER | BS_AUTOCHECKBOX,
                490, 810, 60, 20, hwnd, (HMENU)ID_FLIP_VIDEO, NULL, NULL),

            hApplyVideoEffectsCheckbox = CreateWindow(L"BUTTON", L"Apply Effects", WS_VISIBLE | WS_CHILD | SS_CENTER | BS_AUTOCHECKBOX,
                560, 810, 130, 20, hwnd, (HMENU)ID_APPLY_VIDEO_EFFECTS, NULL, NULL),

            hAlphaLabel = CreateWindow(L"STATIC", L"A", WS_VISIBLE | WS_CHILD | SS_CENTER,
                712, 785, 20, 20, hwnd, NULL, NULL, NULL),

            hRedLabel = CreateWindow(L"STATIC", L"R", WS_VISIBLE | WS_CHILD | SS_CENTER,
                762, 785, 20, 20, hwnd, NULL, NULL, NULL),

            hGreenLabel = CreateWindow(L"STATIC", L"G", WS_VISIBLE | WS_CHILD | SS_CENTER,
                812, 785, 20, 20, hwnd, NULL, NULL, NULL),

            hBlueLabel = CreateWindow(L"STATIC", L"B", WS_VISIBLE | WS_CHILD | SS_CENTER,
                862, 785, 20, 20, hwnd, NULL, NULL, NULL),

            hAlphaTextBox = CreateWindow(L"EDIT", L"0", WS_VISIBLE | WS_CHILD | WS_BORDER | ES_NUMBER | SS_CENTER,
                700, 810, 45, 20, hwnd, (HMENU)ID_ALPHA_TEXTBOX, NULL, NULL),

            hRedTextBox = CreateWindow(L"EDIT", L"0", WS_VISIBLE | WS_CHILD | WS_BORDER | ES_NUMBER | SS_CENTER,
                750, 810, 45, 20, hwnd, (HMENU)ID_RED_TEXTBOX, NULL, NULL),

            hGreenTextBox = CreateWindow(L"EDIT", L"0", WS_VISIBLE | WS_CHILD | WS_BORDER | ES_NUMBER | SS_CENTER,
                800, 810, 45, 20, hwnd, (HMENU)ID_GREEN_TEXTBOX, NULL, NULL),

            hBlueTextBox = CreateWindow(L"EDIT", L"0", WS_VISIBLE | WS_CHILD | WS_BORDER | ES_NUMBER | SS_CENTER,
                850, 810, 45, 20, hwnd, (HMENU)ID_BLUE_TEXTBOX, NULL, NULL),

            hBlueLabel = CreateWindow(L"STATIC", L"Speed", WS_VISIBLE | WS_CHILD | SS_CENTER,
                990, 785, 60, 20, hwnd, NULL, NULL, NULL),

            hSpeedVideoSlider = CreateWindow(TRACKBAR_CLASS, NULL, WS_VISIBLE | WS_CHILD | TBS_HORZ,
                920, 810, 200, 20, hwnd, (HMENU)ID_SPEED_VIDEO, NULL, NULL),

            hSeekVideoSlider = CreateWindow(TRACKBAR_CLASS, NULL, WS_VISIBLE | WS_CHILD | TBS_HORZ,
                10, 850, 1280, 30, hwnd, (HMENU)ID_SEEK_VIDEO, NULL, NULL),
        };

        HFONT hFont = CreateFont(-18, 0, 0, 0, FW_NORMAL, FALSE, FALSE, FALSE,
            DEFAULT_CHARSET, OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS,
            DEFAULT_QUALITY, DEFAULT_PITCH | FF_SWISS, L"Arial");

        for (HWND hControl : hControls)
        {
            SendMessage(hControl, WM_SETFONT, (WPARAM)hFont, TRUE);
        }

        SendMessage(hAlphaTextBox, EM_SETLIMITTEXT, 3, 0);
        SendMessage(hRedTextBox, EM_SETLIMITTEXT, 3, 0);
        SendMessage(hGreenTextBox, EM_SETLIMITTEXT, 3, 0);
        SendMessage(hBlueTextBox, EM_SETLIMITTEXT, 3, 0);

        SendMessage(hSpeedVideoSlider, TBM_SETRANGE, TRUE, MAKELPARAM(10, 5000));
        SendMessage(hSpeedVideoSlider, TBM_SETPOS, TRUE, 100);

        return true;
    }

    void handleCommand(HWND hwnd, int id, HWND hwndCtl, UINT codeNotify)
    {
        switch (id) {
        case ID_SELECT_VIDEO:
            handleSelectVideo(hwnd);
            break;
        case ID_SHOW_VIDEO_INFO:
            handleShowVideoInfo(hwnd);
            break;
        case ID_PLAY_VIDEO:
            handlePlayVideo(hwnd);
            break;
        case ID_PAUSE_VIDEO:
            handlePauseVideo(hwnd);
            break;
        case ID_RESET_VIDEO:
            handleResetVideo(hwnd);
            break;
        case ID_FLIP_VIDEO:
            handleFlipVideo(hwnd);
            break;
        case ID_APPLY_VIDEO_EFFECTS:
            handleApplyVideoEffects(hwnd);
            break;
        case ID_ALPHA_TEXTBOX:
        case ID_RED_TEXTBOX:
        case ID_GREEN_TEXTBOX:
        case ID_BLUE_TEXTBOX:
            handleChangeBlendColor(hwnd, codeNotify);
            break;
        }
    }

    void handleHScroll(HWND hwnd, HWND hwndCtl, UINT code, int pos)
    {
        if (!videoPlayer) return;
        if (code == SB_ENDSCROLL) return;
        if (code == SB_THUMBPOSITION) return;
        
        if (hwndCtl == hSeekVideoSlider)
        {
            int frameIndex = (int)SendMessage(hSeekVideoSlider, TBM_GETPOS, 0, 0);
            videoPlayer->seek(frameIndex);
        }

        if (hwndCtl == hSpeedVideoSlider)
        {
            int speed = (int)SendMessage(hSpeedVideoSlider, TBM_GETPOS, 0, 0);
            videoPlayer->speed(speed / 100.0f);
        }
    }

    void handleClose(HWND hwnd)
    {
        PostQuitMessage(0);
    }



    //
    // Window Controls
    //

    void handleSelectVideo(HWND hwnd)
    {
        OPENFILENAME ofn{};
        wchar_t lpstrFile[260] = {};
        ofn.lStructSize = sizeof(ofn);
        ofn.hwndOwner = hwnd;
        ofn.lpstrFile = lpstrFile;
        ofn.nMaxFile = sizeof(lpstrFile);
        ofn.lpstrFilter = L"Video Files\0*.MP4;*.WMV;*.AVI;*.MKV;*.MOV\0All Files\0*.*\0";
        ofn.nFilterIndex = 1;
        ofn.lpstrFileTitle = NULL;
        ofn.nMaxFileTitle = 0;
        ofn.lpstrInitialDir = NULL;
        ofn.lpstrTitle = L"Select a video file";
        ofn.Flags = OFN_PATHMUSTEXIST | OFN_FILEMUSTEXIST;

        if (!GetOpenFileName(&ofn)) return;
        HRESULT hr = VideoAnalyzer::readInfo(wstring(ofn.lpstrFile), _videoInfo);

        if (FAILED(hr)) return;

        if (videoPlayer) delete videoPlayer;
        videoPlayer = new VideoPlayer(hVideoAreaStatic, _videoInfo);
        videoPlayer->onDrawFrame = handleDrawFrame;

        SetWindowText(hVideoPathStatic, _videoInfo.filePath.c_str());

        PostMessage(hSeekVideoSlider, TBM_SETRANGE, TRUE, MAKELPARAM(0, _videoInfo.frameCount));
        PostMessage(hSeekVideoSlider, TBM_SETPOS, TRUE, 0);

        PostMessage(hSpeedVideoSlider, TBM_SETPOS, TRUE, 100);

        Button_SetCheck(hFlipVideoCheckbox, BST_UNCHECKED);
        Button_SetCheck(hApplyVideoEffectsCheckbox, BST_UNCHECKED);

        SetWindowText(hAlphaTextBox, L"0");
        SetWindowText(hRedTextBox, L"0");
        SetWindowText(hGreenTextBox, L"0");
        SetWindowText(hBlueTextBox, L"0");
    }

    void handleShowVideoInfo(HWND hwnd)
    {
        if (_videoInfo.filePath.empty()) return;

        VideoInfoWindow::show(hwnd, _videoInfo);
    }

    void handlePlayVideo(HWND hwnd)
    {
        if (!videoPlayer) return;

        videoPlayer->play();
    }

    void handlePauseVideo(HWND hwnd)
    {
        if (!videoPlayer) return;

        videoPlayer->pause();
    }

    void handleResetVideo(HWND hwnd)
    {
        if (!videoPlayer) return;

        videoPlayer->reset();
    }

    void handleFlipVideo(HWND hwnd)
    {
        if (!videoPlayer)
        {
            Button_SetCheck(hFlipVideoCheckbox, BST_UNCHECKED);
            return;
        }

        bool shouldFlip = videoPlayer->toggleFlip();

        Button_SetCheck(hFlipVideoCheckbox, shouldFlip ? BST_CHECKED : BST_UNCHECKED);
    }

    void handleApplyVideoEffects(HWND hwnd)
    {
        if (!videoPlayer)
        {
            Button_SetCheck(hApplyVideoEffectsCheckbox, BST_UNCHECKED);
            return;
        }

        bool shouldApplyEffects = videoPlayer->toggleEffects();

        Button_SetCheck(hApplyVideoEffectsCheckbox, shouldApplyEffects ? BST_CHECKED : BST_UNCHECKED);
    }

    void handleChangeBlendColor(HWND hwnd, UINT codeNotify)
    {
        if (!videoPlayer) return;
        if (codeNotify != EN_CHANGE) return;

        BYTE alpha, red, green, blue;
        wchar_t alphaBuffer[4], redBuffer[4], greenBuffer[4], blueBuffer[4];

        GetWindowText(hAlphaTextBox, alphaBuffer, 4);
        GetWindowText(hRedTextBox, redBuffer, 4);
        GetWindowText(hGreenTextBox, greenBuffer, 4);
        GetWindowText(hBlueTextBox, blueBuffer, 4);

        alpha = min(_wtoi(alphaBuffer), 255);
        red = min(_wtoi(redBuffer), 255);
        green = min(_wtoi(greenBuffer), 255);
        blue = min(_wtoi(blueBuffer), 255);

        Color color = { alpha, red, green, blue };

        videoPlayer->blend(color);
    }



    //
    // Video Player
    //

    void handleDrawFrame(UINT32 frameIndex)
    {
        PostMessage(hSeekVideoSlider, TBM_SETPOS, TRUE, frameIndex);
    }

}