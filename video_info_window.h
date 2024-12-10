#pragma once

#include <windows.h>

#include "video_info.h"



namespace VideoInfoWindow
{

	//
	// Constants
	//

	constexpr POINT WINDOW_SIZE = POINT{ 600, 400 };



	//
	// Window Setup
	//

	void show(HWND hwnd, VideoInfo videoInfo);

	bool init(HINSTANCE hInstance, int nCmdShow);

	int runMessageLoop();

	LRESULT CALLBACK wndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);



	//
	// Window Lifecycle
	//

	void handleGetMinMaxInfo(HWND hWnd, LPMINMAXINFO mmi);

	bool handleCreate(HWND hwnd, LPCREATESTRUCT lpCreateStruct);

	void handleClose(HWND hwnd);

}