#ifndef DEBUGHEADFILEFORCPP
#define DEBUGHEADFILEFORCPP

#define EQ(x,y) ( !lstrcmpi((x),(y)) )
#define lxsend(s,buf) send((s),(buf),lstrlen(buf),0)
#define lxrecv(s,buf) recv(s,buf,sizeof(buf),0)
#define MsgBox(cap,text) MessageBoxEx(NULL,text,cap,MB_OK,0001)
#define DbgErr DbpErr

CONST DWORD OPTIONS = FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_IGNORE_INSERTS;
#define LOCALLANG MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT)

LPVOID __stdcall VirtualAlloc(size_t _size);
BOOL __stdcall VirtualFree(LPVOID lp, size_t _size);
HANDLE __stdcall CreateThread(LPTHREAD_START_ROUTINE lpaddr, LPVOID lp);
HANDLE __stdcall CreateThread(LPTHREAD_START_ROUTINE lpaddr);

int __cdecl ErrMsg(LPCSTR pFormat, ...);
int __cdecl ErrMsg(LPCWSTR pFormat, ...);
int __stdcall GetErr(WCHAR msg[]);
int __stdcall GetErr(CHAR msg[]);
int __cdecl Msg(LPCWSTR pFormat, ...);
int __cdecl Msg(LPCSTR pFormat, ...);

#ifdef DE

int __cdecl MsgErr(LPCSTR pFormat, ...);
int __cdecl MsgErr(LPCWSTR pFormat, ...);

int __cdecl MsgDbg(LPCWSTR pFormat, ...);
int __cdecl MsgDbg(LPCSTR pFormat, ...);

void __cdecl Dbp(LPCWSTR pFormat, ...);
void __cdecl Dbp(LPCSTR pFormat, ...);

int __cdecl DbpErr(LPCTSTR pFormat, ...);

int __cdecl Log(LPCTSTR pFormat, ...);

int __cdecl LogEx(WCHAR logfile[], LPCWSTR pFormat, ...);
int __cdecl LogEx(CHAR logfile[], LPCSTR pFormat, ...);

int __cdecl LogExTime(TCHAR logfile[], LPCTSTR pFormat, ...);

int __cdecl recordEx(WCHAR logfile[], LPCWSTR pFormat, ...);
int __cdecl recordEx(TCHAR logfile[], LPCTSTR pFormat, ...);

#else

#define DbpErr(_X_)
#define MsgErr(msg)
#define Log(me)
#define LogEx(_X_,_Y_)
#define LogExTime(logfile,msg)
#define recordEx(_X_,_Y_)
#define Dbp(_X_)

#endif	//DE

#endif	//defined
