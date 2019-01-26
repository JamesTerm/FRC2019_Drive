// DriverStation.cpp : Defines the entry point for the application.
//

#include "stdafx.h"
#include "DriverStation.h"
#include <algorithm>
#include <functional>

//These have no use here, but need to be implemented
std::string DefaultCommandPrompt()
{
	return "NotUsedHere";
}

void SetCommandPromptCallback(std::function<std::string(void)> callback)
{
}


#define MAX_LOADSTRING 100

// Global Variables:
HINSTANCE hInst;                                // current instance
WCHAR szTitle[MAX_LOADSTRING];                  // The title bar text
WCHAR szWindowClass[MAX_LOADSTRING];            // the main window class name

#pragma comment (lib,"winmm")
#pragma comment (lib,"Shlwapi.lib")
#include <timeapi.h>
#include <shlwapi.h>
#include "../Robot_Tester.h"

//Since the lambda cannot capture, we must give it access to the robot here
RobotTester *s_pRobotTester = nullptr;  

__inline void GetParentDirectory(std::wstring &path)
{
	path = (path.substr(0, path.find_last_of(L"/\\")).c_str());
}

int APIENTRY wWinMain(_In_ HINSTANCE hInstance,
	_In_opt_ HINSTANCE hPrevInstance,
	_In_ LPWSTR    lpCmdLine,
	_In_ int       nCmdShow)
{
	{ // Three magic lines of code
		TIMECAPS TimeCaps_;
		timeGetDevCaps(&TimeCaps_, sizeof(TimeCaps_));
		timeBeginPeriod(TimeCaps_.wPeriodMin);
	}

	{
		std::wstring path;
		wchar_t Buffer[MAX_PATH];
		GetModuleFileName(nullptr, Buffer, MAX_PATH);
		PathRemoveFileSpecW(Buffer);
		path = Buffer;
		GetParentDirectory(path);
		GetParentDirectory(path);
		SetCurrentDirectoryW(path.c_str());
	}
	UNREFERENCED_PARAMETER(hPrevInstance);
	UNREFERENCED_PARAMETER(lpCmdLine);

	// TODO: Place code here.

	// Initialize global strings
	LoadStringW(hInstance, IDS_APP_TITLE, szTitle, MAX_LOADSTRING);
	LoadStringW(hInstance, IDC_DRIVERSTATION, szWindowClass, MAX_LOADSTRING);
	//MyRegisterClass(hInstance);

	#if 0
	// Perform application initialization:
	if (!InitInstance(hInstance, nCmdShow))
	{
		return FALSE;
	}
	#else

	const HWND pParent = nullptr;  //just to show what the parameter is

	// Display the main dialog box.  I've always wanted to put the callback in the same place!
	HWND m_hDlg = CreateDialogW(hInstance, MAKEINTRESOURCE(IDD_DriveStation1), pParent,
		[](HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
	{
		INT_PTR ret = FALSE;
		UNREFERENCED_PARAMETER(lParam);
		switch (message)
		{
		case WM_INITDIALOG:
			return (INT_PTR)TRUE;
		case WM_COMMAND:
		{
			int wmId = LOWORD(wParam);
			// Parse the menu selections:
			switch (wmId)
			{
			case IDStart:
				//DebugBreak();
				OutputDebugStringW(L"Start\n");
				//printf("start");
				s_pRobotTester->StartStreaming();
				ret = DefWindowProc(hWnd, message, wParam, lParam);
				break;
			case IDStop:
				OutputDebugStringW(L"Stop\n");
				//printf("stop");
				s_pRobotTester->StopStreaming();
				ret = DefWindowProc(hWnd, message, wParam, lParam);
				break;
			case IDC_Tele:
			case IDC_Auton:
			case IDC_Test:
			{
				int game_mode = 0;
				switch (wmId)
				{
				case IDC_Auton: game_mode = 0; break;
				case IDC_Tele: game_mode = 1; break;
				case IDC_Test: game_mode = 2; break;
				}
				s_pRobotTester->SetGameMode(game_mode);
				ret = DefWindowProc(hWnd, message, wParam, lParam);
				break;
			}
			case IDM_EXIT:
			case IDCANCEL:  //Not sure why this 2 is the same as the close button
				printf("Shutting down\n");
				DestroyWindow(hWnd);
				break;
			default:
				ret=DefWindowProc(hWnd, message, wParam, lParam);
			}
		}
		break;
		//May want to call a destructor here
		//case WM_CLOSE:
		//	DestroyWindow(hWnd);
		//	break;
		case WM_PAINT:
		{
			PAINTSTRUCT ps;
			HDC hdc = BeginPaint(hWnd, &ps);
			// TODO: Add any drawing code that uses hdc here...
			EndPaint(hWnd, &ps);
		}
		break;
		case WM_DESTROY:
			PostQuitMessage(0);
			break;
			//default:
			//    return DefWindowProc(hWnd, message, wParam, lParam);
		}
		return ret;
	}
	);

	if (!m_hDlg)
	{
		return FALSE;
	}

	//We made it this far... start up the robot
	RobotTester _robot_tester;
	s_pRobotTester = &_robot_tester;
	_robot_tester.RobotTester_init();

	//Robot must be started before sending out these messages
	//TO avoid needing to read from the tester... match the current state against the current default
	SendDlgItemMessageW(m_hDlg, IDC_Tele, BM_SETCHECK, true, 0);
	SendDlgItemMessageW(m_hDlg, IDStop, BM_CLICK, true, 0);
	ShowWindow(m_hDlg, nCmdShow);
	UpdateWindow(m_hDlg);

	#endif


	HACCEL hAccelTable = LoadAccelerators(hInstance, MAKEINTRESOURCE(IDC_DRIVERSTATION));
	MSG msg;

	// Main message loop:
	while (GetMessage(&msg, nullptr, 0, 0))
	{
		if (!TranslateAccelerator(msg.hwnd, hAccelTable, &msg))
		{
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}
	}

	return (int)msg.wParam;
}
