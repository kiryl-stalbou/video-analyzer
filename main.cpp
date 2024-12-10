#pragma comment(lib, "mf")
#pragma comment(lib, "mfplat")
#pragma comment(lib, "mfuuid")
#pragma comment(lib, "shlwapi")
#pragma comment(lib, "Comctl32")
#pragma comment(lib, "mfreadwrite")

#pragma warning(disable:28251)
#pragma warning(disable:6031)

#include <mfapi.h>

#include "video_player_window.h"



int APIENTRY wWinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPWSTR lpCmdLine, int nCmdShow)
{
    (void)HeapSetInformation(NULL, HeapEnableTerminationOnCorruption, NULL, 0);

    CoInitializeEx(NULL, COINIT_APARTMENTTHREADED | COINIT_DISABLE_OLE1DDE);
    MFStartup(MF_VERSION); 

    if (VideoPlayerWindow::init(hInstance, nCmdShow))
    {
        VideoPlayerWindow::runMessageLoop();
    }

    MFShutdown();
    CoUninitialize();

    return EXIT_SUCCESS;
}