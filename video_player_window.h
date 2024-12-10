#pragma once

#include <windows.h>



namespace VideoPlayerWindow
{

	//
	// Defines
	//

	#define ID_SELECT_VIDEO			101
	#define ID_SHOW_VIDEO_INFO		102
	#define ID_PLAY_VIDEO			103
	#define ID_PAUSE_VIDEO			104
	#define ID_RESET_VIDEO			105
	#define ID_FLIP_VIDEO			106
	#define ID_APPLY_VIDEO_EFFECTS	107
	#define ID_ALPHA_TEXTBOX		108
	#define ID_RED_TEXTBOX			109
	#define ID_GREEN_TEXTBOX		110
	#define ID_BLUE_TEXTBOX			111
	#define ID_SPEED_VIDEO			112
	#define ID_SEEK_VIDEO			113


	//
	// Constants
	//

	constexpr POINT WINDOW_SIZE = POINT{ 1320, 930 };



	//
	// Window Setup
	//

	bool init(HINSTANCE hInstance, int nCmdShow);

	int runMessageLoop();

	LRESULT CALLBACK wndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);



	//
	// Window Lifecycle
	//

	void handleGetMinMaxInfo(HWND hWnd, LPMINMAXINFO mmi);

	bool handleCreate(HWND hwnd, LPCREATESTRUCT lpCreateStruct);

	void handleCommand(HWND hwnd, int id, HWND hwndCtl, UINT codeNotify);

	void handleHScroll(HWND hwnd, HWND hwndCtl, UINT code, int pos);

	void handleClose(HWND hwnd);



	//
	// Window Controls
	//

	void handleSelectVideo(HWND hwnd);

	void handleShowVideoInfo(HWND hwnd);

	void handlePlayVideo(HWND hwnd);

	void handlePauseVideo(HWND hwnd);

	void handleResetVideo(HWND hwnd);

	void handleFlipVideo(HWND hwnd);

	void handleApplyVideoEffects(HWND hwnd);

	void handleChangeBlendColor(HWND hwnd, UINT codeNotify);



	//
	// Video Player
	//

	void handleDrawFrame(UINT32 frameIndex);

}