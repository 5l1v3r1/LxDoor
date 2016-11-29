#ifndef WININETDOWNLOADHEADFILE

#define WININETDOWNLOADHEADFILE

#include <windows.h>
#include <shlwapi.h>
#pragma comment(lib, "shlwapi.lib")
#include <wininet.h>
#pragma comment(lib, "Wininet.lib")

#define DOWNLOAD_SIZE	16*1024       //每次下载的字节数

int HttpDownLoadFile(LPCTSTR szUrl, LPCTSTR szFilePath)
{
	HINTERNET hSession = NULL;
	HINTERNET hConnect = NULL;
	char buffer[DOWNLOAD_SIZE];
	BOOL fgSucceed = FALSE;
	char csHead[] = "Accept: */*\r\n";

	hSession = InternetOpen("Internet Explorer", INTERNET_OPEN_TYPE_DIRECT, NULL, NULL, NULL);
	if (!hSession)  return FALSE;

	hConnect = InternetOpenUrl(hSession, szUrl, csHead, lstrlen(csHead),
		INTERNET_FLAG_PRAGMA_NOCACHE | INTERNET_FLAG_RELOAD, 0);

	if (hConnect)
	{
		DWORD dwRet = sizeof(buffer);
		fgSucceed = HttpQueryInfo(hConnect, HTTP_QUERY_STATUS_CODE, buffer, &dwRet, NULL);
		dwRet = (DWORD)StrToLong(buffer);

		if (dwRet == HTTP_STATUS_OK)
		{
			DWORD rt;
			HANDLE hfile = CreateFile(szFilePath, GENERIC_ALL, FILE_SHARE_WRITE,
				0, OPEN_ALWAYS, FILE_ATTRIBUTE_NORMAL, 0);

			if (hfile != INVALID_HANDLE_VALUE)
			{
				DWORD dwCount = 0;    //下载字节计数器
				fgSucceed = TRUE;
				dwRet = 1;
				while (fgSucceed&&dwRet > 0)
				{
					fgSucceed = InternetReadFile(hConnect, buffer, sizeof(buffer), &dwRet);

					if (dwRet > 0)
					{
						SetFilePointer(hfile, 0, 0, FILE_END);
						WriteFile(hfile, buffer, dwRet, &rt, 0);
					}
					Sleep(50);
				}

				CloseHandle(hfile);
			}
			else
			{
				return 3;
			}
		}
		else
			return 2;
	}
	else
		return 1;

	InternetCloseHandle(hSession);
	InternetCloseHandle(hConnect);

	return 0;
}

#endif	//WININETDOWNLOADHEADFILE
