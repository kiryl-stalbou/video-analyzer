#include <string>
#include <windowsx.h>

#include "video_info_window.h"
#include "video_info.h"
#include "Resource.h"



using std::wstring, std::to_wstring;

namespace VideoInfoWindow
{

    //
    // Variables
    //

    VideoInfo _videoInfo;



    //
    // Window Setup
    //

    void show(HWND hwnd, VideoInfo videoInfo)
    {
        HINSTANCE hInstance = (HINSTANCE)GetWindowLongPtr(hwnd, GWLP_HINSTANCE);

        _videoInfo = videoInfo;

        if (init(hInstance, SW_SHOW))
        {
            runMessageLoop();
        }
    }

    bool init(HINSTANCE hInstance, int nCmdShow)
    {
        WNDCLASSEXW wcexw;
        wcexw.cbSize = sizeof(WNDCLASSEX);
        wcexw.cbClsExtra = 0;
        wcexw.cbWndExtra = 0;
        wcexw.lpfnWndProc = wndProc;
        wcexw.lpszMenuName = nullptr;
        wcexw.hInstance = hInstance;
        wcexw.lpszClassName = L"Video_Info_Class";
        wcexw.style = CS_HREDRAW | CS_VREDRAW;
        wcexw.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);
        wcexw.hCursor = LoadCursor(nullptr, IDC_ARROW);
        wcexw.hIcon = LoadIcon(hInstance, MAKEINTRESOURCE(IDC_VIDEOANALYZER));
        wcexw.hIconSm = LoadIcon(wcexw.hInstance, MAKEINTRESOURCE(IDI_SMALL));

        RegisterClassExW(&wcexw);

        HWND hWnd = CreateWindowW(
            L"Video_Info_Class", L"Video Info", WS_OVERLAPPEDWINDOW,
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
        wstring fileSize = to_wstring(_videoInfo.fileSize) + L" Bytes";
        wstring duration = to_wstring(_videoInfo.duration) + L" seconds";
        wstring resolution = to_wstring(_videoInfo.width) + L"x" + to_wstring(_videoInfo.height);
        wstring frameRate = to_wstring(_videoInfo.frameRate) + L" fps";
        wstring frameDuration = to_wstring(_videoInfo.frameDuration) + L" milliseconds";
        wstring bitRate = to_wstring(_videoInfo.bitRate) + L" Kbps";
        wstring frameCount = to_wstring(_videoInfo.frameCount);

        HWND hControls[] = {
            CreateWindowW(L"STATIC", L"File Name:", WS_CHILD | WS_VISIBLE | SS_LEFT, 10, 10, 150, 20, hwnd, nullptr, nullptr, nullptr),
            CreateWindowW(L"STATIC", _videoInfo.fileName.c_str(), WS_CHILD | WS_VISIBLE | SS_LEFT, 160, 10, 250, 20, hwnd, nullptr, nullptr, nullptr),

            CreateWindowW(L"STATIC", L"File Type:", WS_CHILD | WS_VISIBLE | SS_LEFT, 10, 40, 150, 20, hwnd, nullptr, nullptr, nullptr),
            CreateWindowW(L"STATIC", _videoInfo.fileType.c_str(), WS_CHILD | WS_VISIBLE | SS_LEFT, 160, 40, 250, 20, hwnd, nullptr, nullptr, nullptr),

            CreateWindowW(L"STATIC", L"File Size:", WS_CHILD | WS_VISIBLE | SS_LEFT, 10, 70, 150, 20, hwnd, nullptr, nullptr, nullptr),
            CreateWindowW(L"STATIC", fileSize.c_str(), WS_CHILD | WS_VISIBLE | SS_LEFT, 160, 70, 250, 20, hwnd, nullptr, nullptr, nullptr),

            CreateWindowW(L"STATIC", L"Duration:", WS_CHILD | WS_VISIBLE | SS_LEFT, 10, 100, 150, 20, hwnd, nullptr, nullptr, nullptr),
            CreateWindowW(L"STATIC", duration.c_str(), WS_CHILD | WS_VISIBLE | SS_LEFT, 160, 100, 250, 20, hwnd, nullptr, nullptr, nullptr),

            CreateWindowW(L"STATIC", L"Resolution:", WS_CHILD | WS_VISIBLE | SS_LEFT, 10, 130, 150, 20, hwnd, nullptr, nullptr, nullptr),
            CreateWindowW(L"STATIC", resolution.c_str(), WS_CHILD | WS_VISIBLE | SS_LEFT, 160, 130, 250, 20, hwnd, nullptr, nullptr, nullptr),

            CreateWindowW(L"STATIC", L"Frame Rate:", WS_CHILD | WS_VISIBLE | SS_LEFT, 10, 160, 150, 20, hwnd, nullptr, nullptr, nullptr),
            CreateWindowW(L"STATIC", frameRate.c_str(), WS_CHILD | WS_VISIBLE | SS_LEFT, 160, 160, 250, 20, hwnd, nullptr, nullptr, nullptr),

            CreateWindowW(L"STATIC", L"Frame Duration:", WS_CHILD | WS_VISIBLE | SS_LEFT, 10, 190, 150, 20, hwnd, nullptr, nullptr, nullptr),
            CreateWindowW(L"STATIC", frameDuration.c_str(), WS_CHILD | WS_VISIBLE | SS_LEFT, 160, 190, 250, 20, hwnd, nullptr, nullptr, nullptr),

            CreateWindowW(L"STATIC", L"Bit Rate:", WS_CHILD | WS_VISIBLE | SS_LEFT, 10, 220, 150, 20, hwnd, nullptr, nullptr, nullptr),
            CreateWindowW(L"STATIC", bitRate.c_str(), WS_CHILD | WS_VISIBLE | SS_LEFT, 160, 220, 250, 20, hwnd, nullptr, nullptr, nullptr),

            CreateWindowW(L"STATIC", L"Codec:", WS_CHILD | WS_VISIBLE | SS_LEFT, 10, 250, 150, 20, hwnd, nullptr, nullptr, nullptr),
            CreateWindowW(L"STATIC", _videoInfo.codecName.c_str(), WS_CHILD | WS_VISIBLE | SS_LEFT, 160, 250, 250, 20, hwnd, nullptr, nullptr, nullptr),

            CreateWindowW(L"STATIC", L"Frame Count:", WS_CHILD | WS_VISIBLE | SS_LEFT, 10, 280, 150, 20, hwnd, nullptr, nullptr, nullptr),
            CreateWindowW(L"STATIC", frameCount.c_str(), WS_CHILD | WS_VISIBLE | SS_LEFT, 160, 280, 250, 20, hwnd, nullptr, nullptr, nullptr),
        };

        HFONT hFont = CreateFont(-18, 0, 0, 0, FW_NORMAL, FALSE, FALSE, FALSE,
            DEFAULT_CHARSET, OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS,
            DEFAULT_QUALITY, DEFAULT_PITCH | FF_SWISS, L"Arial");

        for (HWND hControl : hControls)
        {
            SendMessage(hControl, WM_SETFONT, (WPARAM)hFont, TRUE);
        }

        return true;
    }

    void handleClose(HWND hwnd)
    {
        DestroyWindow(hwnd);
    }

}