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
#include "Keyboard.h"

//Since the lambda cannot capture, we must give it access to the robot here
RobotTester *s_pRobotTester = nullptr;  
void BindRobot(RobotTester &_robot_tester);  //forward declare
Keyboard *s_Keyboard = nullptr;

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

	//Well this is a bit painful, but we must ensure our path is aligned to obtain the LUA file
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
			//TO avoid needing to read from the tester... match the current state against the current default
			CheckDlgButton(hWnd, IDC_Tele, BM_SETCHECK);
			CheckDlgButton(hWnd, IDC_Stop, BM_SETCHECK);
			//Button_SetState(hWnd, IDStop, BM_CLICK, true, 0);
			return (INT_PTR)TRUE;
		case WM_COMMAND:
		{
			int wmId = LOWORD(wParam);
			// Parse the menu selections:
			switch (wmId)
			{
			case IDC_Start:
				//DebugBreak();
				OutputDebugStringW(L"Start\n");
				//printf("start");
				s_pRobotTester->StartStreaming();
				CheckDlgButton(hWnd, IDC_Stop, BST_UNCHECKED);
				CheckDlgButton(hWnd, IDC_Start, BM_SETCHECK);
				break;
			case IDC_Stop:
				OutputDebugStringW(L"Stop\n");
				//printf("stop");
				s_pRobotTester->StopStreaming();
				CheckDlgButton(hWnd, IDC_Stop, BM_SETCHECK);
				CheckDlgButton(hWnd, IDC_Start, BST_UNCHECKED);
				break;
			case IDC_Tele:
			case IDC_Auton:
			case IDC_Test:
			{
				CheckDlgButton(hWnd, IDC_Tele, BST_UNCHECKED);
				CheckDlgButton(hWnd, IDC_Auton, BST_UNCHECKED);
				CheckDlgButton(hWnd, IDC_Test, BST_UNCHECKED);
				int game_mode = 0;
				switch (wmId)
				{
				case IDC_Auton: 
					CheckDlgButton(hWnd, IDC_Auton, BM_SETCHECK);
					game_mode = 0;
					break;
				case IDC_Tele: 
					CheckDlgButton(hWnd, IDC_Tele, BM_SETCHECK);
					game_mode = 1;
					break;
				case IDC_Test: 
					CheckDlgButton(hWnd, IDC_Test, BM_SETCHECK);
					game_mode = 2; 
					break;
				}
				s_pRobotTester->SetGameMode(game_mode);
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
		case WM_KEYUP:
			OutputDebugStringW(L"KeyPressedUp\n");
			if (s_Keyboard)
				s_Keyboard->KeyPressRelease((int)wParam, false);
			break;
		case WM_KEYDOWN:
			OutputDebugStringW(L"KeyPressedDN\n");
			if (s_Keyboard)
				s_Keyboard->KeyPressRelease((int)wParam, true);
			break;
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
	//Bind robot for Keyboard binding
	BindRobot(_robot_tester);
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


//To bind the keyboard we'll need to access AutonMain and Eventmap
#include "../../main/cpp/Base/Base_Includes.h"
#include "../../main/cpp/Base/Vec2d.h"
#include "../../main/cpp/Base/Misc.h"
#include "../../main/cpp/Base/Event.h"
#include "../../main/cpp/Base/EventMap.h"
#include "../../main/cpp/Base/Script.h"
#include "../../main/cpp/Common/Entity_Properties.h"
#include "../../main/cpp/Common/Physics_1D.h"
#include "../../main/cpp/Common/Physics_2D.h"
#include "../../main/cpp/Common/Entity2D.h"
#include "../../main/cpp/Common/Goal.h"
#include "../../main/cpp/Common/Ship_1D.h"
#include "../../main/cpp/Common/Ship.h"

#include "../../main/cpp/Config/ActiveCollection.h"
#include "../../main/cpp/AutonMain.h"
AutonMain *s_RobotContainer=nullptr;
void BindKeyboard()
{
	if (s_RobotContainer)
	{
		if (s_Keyboard)
		{
			assert(false);  //recoverable but need to see use-case
			delete s_Keyboard;
		}
		Ship_Tester *_pRobot = s_RobotContainer->GetRobot();
		Keyboard *_Keyboard = new Keyboard;
		_Keyboard->Keyboard_init(_pRobot->GetEventMap());
		s_Keyboard = _Keyboard;
	}
	else
	{
		delete s_Keyboard;
		s_Keyboard = nullptr;
	}

}

void BindRobot(RobotTester &_robot_tester)
{
	_robot_tester.RobotTester_SetParentBindCallback(
		[](AutonMain *instance)
	{
		s_RobotContainer = instance;
		BindKeyboard();
	}
	);
}
