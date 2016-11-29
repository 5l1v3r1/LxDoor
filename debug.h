#ifndef DEBUGHEADFILE
#define DEBUGHEADFILE

#include <windows.h>
#include "TCHAR.h"

#include "debugh.h"

#ifdef DE

void __cdecl Dbp(LPCWSTR pFormat, ...)
{
	static WCHAR strBuf[1024];

	va_list pArg;

	va_start(pArg, pFormat);
	wvsprintfW(strBuf, pFormat, pArg);
	va_end(pArg);

	OutputDebugStringW(strBuf);
}

void __cdecl Dbp(LPCSTR pFormat, ...)
{
	static CHAR strBuf[1024];

	va_list pArg;

	va_start(pArg, pFormat);
	wvsprintfA(strBuf, pFormat, pArg);
	va_end(pArg);

	OutputDebugStringA(strBuf);
}

int __cdecl DbpErr(LPCWSTR pFormat, ...)
{
	static WCHAR strBuf[1024];
	PVOID lpMsgBuf;

	FormatMessageW(OPTIONS, NULL, GetLastError(), LOCALLANG, (LPWSTR)&lpMsgBuf, 0, NULL);

	va_list pArg;

	va_start(pArg, pFormat);
	wvsprintfW(strBuf, pFormat, pArg);
	va_end(pArg);

	OutputDebugStringW((LPWSTR)lpMsgBuf);
	OutputDebugStringW((LPWSTR)strBuf);

	return 0;
}

int __cdecl DbpErr(LPCSTR pFormat, ...)
{
	static CHAR strBuf[1024];
	PVOID lpMsgBuf;

	FormatMessageA(OPTIONS, NULL, GetLastError(), LOCALLANG, (LPSTR)&lpMsgBuf, 0, NULL);

	va_list pArg;

	va_start(pArg, pFormat);
	wvsprintfA(strBuf, pFormat, pArg);
	va_end(pArg);

	OutputDebugStringA((LPSTR)lpMsgBuf);
	OutputDebugStringA((LPSTR)strBuf);

	return 0;
}

int __cdecl MsgErr(LPCSTR pFormat, ...)
{
	static CHAR strBuf[1024];
	PVOID lpMsgBuf;

	FormatMessageA(OPTIONS, NULL, GetLastError(), LOCALLANG, (LPSTR)&lpMsgBuf, 0, NULL);

	va_list pArg;

	va_start(pArg, pFormat);
	wvsprintfA(strBuf, pFormat, pArg);
	va_end(pArg);

	return MessageBoxExA(NULL, (LPCSTR)lpMsgBuf, (LPCSTR)strBuf, MB_OK, 0001);
}

int __cdecl MsgErr(LPCWSTR pFormat, ...)
{
	WCHAR strBuf[1024];
	PVOID lpMsgBuf;

	FormatMessageW(OPTIONS, NULL, GetLastError(), LOCALLANG, (LPWSTR)&lpMsgBuf, 0, NULL);

	va_list pArg;

	va_start(pArg, pFormat);
	wvsprintfW(strBuf, pFormat, pArg);
	va_end(pArg);

	return MessageBoxExW(NULL, (LPWSTR)lpMsgBuf, (LPWSTR)strBuf, MB_OK, 0001);
}

int __cdecl MsgDbg(LPCWSTR pFormat, ...)
{
	WCHAR buf[1024];
	va_list pArg;

	va_start(pArg, pFormat);
	wvsprintfW(buf, pFormat, pArg);
	va_end(pArg);

	MessageBoxExW(NULL, (LPWSTR)buf, L"MsgBox", MB_OK, 0001);

	return 0;
}

int __cdecl MsgDbg(LPCSTR pFormat, ...)
{
	CHAR buf[1024];
	va_list pArg;

	va_start(pArg, pFormat);
	wvsprintfA(buf, pFormat, pArg);
	va_end(pArg);

	MessageBoxExA(NULL, (LPCSTR)buf, "MsgBox", MB_OK, 0001);

	return 0;
}

int __cdecl LogEx(WCHAR logfile[], LPCWSTR pFormat, ...)
{
	WCHAR strBuf[1024];

	va_list pArg;

	va_start(pArg, pFormat);
	wvsprintfW(strBuf, pFormat, pArg);
	va_end(pArg);

	__try
	{
		DWORD rt;
		HANDLE hfile = CreateFileW(logfile, GENERIC_WRITE, FILE_SHARE_WRITE, 0, OPEN_ALWAYS,
			FILE_ATTRIBUTE_NORMAL, 0);

		SetFilePointer(hfile, 0, 0, FILE_END);
		WriteFile(hfile, strBuf, lstrlenW(strBuf), &rt, 0);
		WriteFile(hfile, L"\r\n\r\n", lstrlenW(L"\r\n\r\n"), &rt, 0);
		CloseHandle(hfile);
		return 1;
	}
	__except (1)
	{
		MsgErr(_T("can't log"));
		return 0;
	}
}

int __cdecl LogEx(CHAR logfile[], LPCSTR pFormat, ...)
{
	CHAR strBuf[1024];

	va_list pArg;

	va_start(pArg, pFormat);
	wvsprintfA(strBuf, pFormat, pArg);
	va_end(pArg);

	__try
	{
		DWORD rt;
		HANDLE hfile = CreateFileA(logfile, GENERIC_WRITE, FILE_SHARE_WRITE, 0, OPEN_ALWAYS,
			FILE_ATTRIBUTE_NORMAL, 0);

		SetFilePointer(hfile, 0, 0, FILE_END);
		WriteFile(hfile, strBuf, lstrlenA(strBuf), &rt, 0);
		WriteFile(hfile, "\r\n\r\n", lstrlenA("\r\n\r\n"), &rt, 0);
		CloseHandle(hfile);
		return 1;
	}
	__except (1)
	{
		MsgErr(_T("can't log"));
		return 0;
	}
}

int __cdecl Log(LPCWSTR pFormat, ...)
{
	WCHAR strBuf[1024];

	va_list pArg;

	va_start(pArg, pFormat);
	wvsprintfW(strBuf, pFormat, pArg);
	va_end(pArg);

	return LogEx(L"C:\\log.txt", (LPWSTR)strBuf);
}

int __cdecl Log(LPCSTR pFormat, ...)
{
	CHAR strBuf[1024];

	va_list pArg;

	va_start(pArg, pFormat);
	wvsprintfA(strBuf, pFormat, pArg);
	va_end(pArg);

	return LogEx("C:\\log.txt", (LPSTR)strBuf);
}

int __cdecl LogExTime(WCHAR logfile[], LPCWSTR pFormat, ...)
{
	WCHAR strBuf[1024];

	va_list pArg;

	va_start(pArg, pFormat);
	wvsprintfW(strBuf, pFormat, pArg);
	va_end(pArg);

	WCHAR szText[2048];

	SYSTEMTIME	SysTime;
	GetLocalTime(&SysTime);

	wsprintfW(szText,
		L"\r\n[%02d/%02d/%d %02d:%02d:%02d]\r\n",
		SysTime.wMonth, SysTime.wDay, SysTime.wYear,
		SysTime.wHour, SysTime.wMinute, SysTime.wSecond
		);

	LogEx(logfile, L"%s%s", szText, strBuf);

	return 0;
}

int __cdecl LogExTime(CHAR logfile[], LPCSTR pFormat, ...)
{
	CHAR strBuf[1024];

	va_list pArg;

	va_start(pArg, pFormat);
	wvsprintfA(strBuf, pFormat, pArg);
	va_end(pArg);

	CHAR szText[2048];

	SYSTEMTIME	SysTime;
	GetLocalTime(&SysTime);

	wsprintfA(szText,
		"\r\n[%02d/%02d/%d %02d:%02d:%02d]\r\n",
		SysTime.wMonth, SysTime.wDay, SysTime.wYear,
		SysTime.wHour, SysTime.wMinute, SysTime.wSecond
		);

	LogEx(logfile, "%s%s", szText, strBuf);

	return 0;
}

int __cdecl recordEx(WCHAR logfile[], LPCWSTR pFormat, ...)
{
	WCHAR strBuf[1024];

	va_list pArg;

	va_start(pArg, pFormat);
	wvsprintfW(strBuf, pFormat, pArg);
	va_end(pArg);

	LogExTime(logfile, strBuf);
	Msg(strBuf, L"Record Ex Proc");

	return 0;
}

int __cdecl recordEx(CHAR logfile[], LPCSTR pFormat, ...)
{
	CHAR strBuf[1024];

	va_list pArg;

	va_start(pArg, pFormat);
	wvsprintfA(strBuf, pFormat, pArg);
	va_end(pArg);

	LogExTime(logfile, strBuf);
	Msg(strBuf, "Record Ex Proc");

	return 0;
}

#else

#define MsgErr(msg)
#define MsgDbg(m)
#define Log(me)
#define LogEx(_X_, _Y_)
#define LogExTime(logfile, msg)
#define recordEx(_X_, _Y_)
#define record(_X_)
#define Dbp(_X_)
#define hexbuf(_X_, _Y_)
#define DbpErr(_X_)

/*
#define hexbuf( //
#define geterr( //
#define LogEx(	//
#define Log(	//
#define LogExTime(	//
#define WriteEx(	//
#define Write(	//
#define recordEx(	//
#define record(	//
*/

#endif	//DE

LPVOID __stdcall VirtualAlloc(size_t _size)
{
	return VirtualAlloc(NULL, _size, MEM_COMMIT | MEM_RESERVE, PAGE_EXECUTE_READWRITE);
}

BOOL __stdcall VirtualFree(LPVOID lp, size_t _size)
{
	return VirtualFree(lp, 0, MEM_RELEASE);
}

HANDLE __stdcall CreateThread(LPTHREAD_START_ROUTINE lpaddr, LPVOID lp)
{
	DWORD id;
	return CreateThread(0, 0, lpaddr, lp, 0, &id);
}

HANDLE __stdcall CreateThread(LPTHREAD_START_ROUTINE lpaddr)
{
	DWORD id;
	return CreateThread(0, 0, lpaddr, 0, 0, &id);
}

int __cdecl Msg(LPCSTR pFormat, ...)
{
	CHAR buf[1024];
	va_list pArg;

	va_start(pArg, pFormat);
	wvsprintfA(buf, pFormat, pArg);
	va_end(pArg);

	MessageBoxExA(NULL, (LPCSTR)buf, "MsgBox", MB_OK, 0001);

	return 0;
}

int __cdecl Msg(LPCWSTR pFormat, ...)
{
	WCHAR buf[1024];
	va_list pArg;

	va_start(pArg, pFormat);
	wvsprintfW(buf, pFormat, pArg);
	va_end(pArg);

	MessageBoxExW(NULL, (LPWSTR)buf, L"MsgBox", MB_OK, 0001);

	return 0;
}

int Msg(DWORD num)
{
	return Msg("%d", num);
}

int __stdcall GetErr(WCHAR msg[])
{
	PVOID lpMsgBuf;

	FormatMessageW(OPTIONS, NULL, GetLastError(), LOCALLANG, (LPWSTR)&lpMsgBuf, 0, NULL);

	lstrcpyW(msg, (WCHAR *)lpMsgBuf);

	return 0;
}

int __stdcall GetErr(CHAR msg[])
{
	PVOID lpMsgBuf;

	FormatMessageA(OPTIONS, NULL, GetLastError(), LOCALLANG, (LPSTR)&lpMsgBuf, 0, NULL);

	lstrcpyA(msg, (CHAR *)lpMsgBuf);

	return 0;
}

int __cdecl ErrMsg(LPCSTR pFormat, ...)
{
	static CHAR strBuf[1024];

	PVOID lpMsgBuf;

	FormatMessageA(OPTIONS, NULL, GetLastError(), LOCALLANG, (LPSTR)&lpMsgBuf, 0, NULL);

	va_list pArg;

	va_start(pArg, pFormat);
	wvsprintfA(strBuf, pFormat, pArg);
	va_end(pArg);

	return MessageBoxExA(NULL, (LPCSTR)lpMsgBuf, strBuf, MB_OK, 0001);
}

int __cdecl ErrMsg(LPCWSTR pFormat, ...)
{
	WCHAR strBuf[1024];

	PVOID lpMsgBuf;

	FormatMessageW(OPTIONS, NULL, GetLastError(), LOCALLANG, (LPWSTR)&lpMsgBuf, 0, NULL);

	va_list pArg;

	va_start(pArg, pFormat);
	wvsprintfW(strBuf, pFormat, pArg);
	va_end(pArg);

	return MessageBoxExW(NULL, (LPCWSTR)lpMsgBuf, strBuf, MB_OK, 0001);
}

#endif	//defined
