#ifndef INFOHEADFILE
#define INFOHEADFILE

#include "windows.h"

#define BUFSIZE 16

int __stdcall GetInfo(char * info)
{
	/////get computer name///////
	TCHAR computerbuf[256];
	DWORD computersize = 256;
	memset(computerbuf, 0, 256);
	if (GetComputerName(computerbuf, &computersize))
	{
		computerbuf[computersize] = 0;
	}

	///////get user name/////////
	TCHAR userbuf[256];
	DWORD usersize = 256;
	memset(userbuf, 0, 256);
	if (GetUserName(userbuf, &usersize))
	{
		userbuf[usersize] = 0;
	}

	//system version
	OSVERSIONINFOEX   osviex;
	char sysbuf[256];
	memset(&osviex, 0, sizeof(OSVERSIONINFOEX));
	osviex.dwOSVersionInfoSize = sizeof(OSVERSIONINFOEX);

	lstrcpy(sysbuf, "Unknown ");

	if (GetVersionEx((LPOSVERSIONINFO)&osviex) != 0)
	{
		switch (osviex.dwMajorVersion)
		{
		case 4:
			if (osviex.dwMinorVersion == 0)
				lstrcpy(sysbuf, "Microsoft Windows NT 4 ");
			break;

		case 5:
			if (osviex.dwMinorVersion == 0)
			{
				lstrcpy(sysbuf, "Microsoft Windows 2000 ");
			}
			else if (osviex.dwMinorVersion == 1)
			{
				lstrcpy(sysbuf, "Windows XP ");
			}
			else if (osviex.dwMinorVersion == 2)
			{
				lstrcpy(sysbuf, "Windows 2003 ");
			}
			break;

		case 6:
			if (osviex.dwMinorVersion == 0)
			{
				lstrcpy(sysbuf, "Windows Vista ");
			}
			else if (osviex.dwMinorVersion == 1)
			{
				lstrcpy(sysbuf, "Windows 7 ");
			}
			else if (osviex.dwMinorVersion == 2) lstrcpy(sysbuf, "Win 8 ");
			else if (osviex.dwMinorVersion == 3) lstrcpy(sysbuf, "Win 8.1 ");
			break;

		case 10:
			lstrcpy(sysbuf, "Windows 10 ");
			break;

		default:
			lstrcpy(sysbuf, "Unknown ");
			break;
		}
	}

	char szHostName[64] = { 0 };

	if (gethostname(szHostName, 64) == 0)
	{
		hostent*   hostinfo = gethostbyname(szHostName);
		if (hostinfo != NULL)
		{
			lstrcpy(szHostName, inet_ntoa(*(struct in_addr *)hostinfo->h_addr_list[0]));
		}
		else
		{
			lstrcpy(szHostName, "Unknown");
		}
	}

	////////////////get cpu info//////////////////

	HKEY hKey;
	char cpu[512] = { 0 }, var[128];
	char szcpuinfo[80] = { 0 };
	DWORD dwBufLen = 80, f;
	RegOpenKeyExW(HKEY_LOCAL_MACHINE,
		L"HARDWARE\\DESCRIPTION\\System\\CentralProcessor\\0",
		0, KEY_QUERY_VALUE, &hKey);

	RegQueryValueExW(hKey, L"ProcessorNameString", NULL, NULL, (LPBYTE)szcpuinfo, &dwBufLen);
	lstrcpy(var, szcpuinfo);

	RegQueryValueExW(hKey, L"Identifier", NULL, NULL, (LPBYTE)szcpuinfo, &dwBufLen);

	dwBufLen = 8;
	RegQueryValueExW(hKey, L"~MHz", NULL, NULL, (LPBYTE)&f, &dwBufLen);

	wsprintf(cpu, "%s\r\nIdentifier:%s\r\n%dMHz", var, szcpuinfo, f);
	RegCloseKey(hKey);

	/////////////get mem size////////////
	MEMORYSTATUS ms;
	GlobalMemoryStatus(&ms);
	char membuf[256]; //ŒÔ¿Ìƒ⁄¥Ê:
	wsprintf(membuf, "%dMB", (int)ms.dwTotalPhys / 1024 / 1024);

	/////////////All info////////////////
	wsprintf(info, "\r\nComputer name:%s\r\nUser:%s\r\nSystem:%s\r\nHostname:%s\r\nCPU:%s\r\n",
		computerbuf,
		userbuf,
		sysbuf,
		szHostName,
		cpu
		);

	return 0;
}

#endif
