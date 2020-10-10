/*
	vfdlib.c

	Virtual Floppy Drive for Windows
	Driver control library
	Miscellaneous functions

	Copyright (C) 2003-2008 Ken Kato
*/

#ifdef __cplusplus
#pragma message(__FILE__": Compiled as C++ for testing purpose.")
#endif	// __cplusplus

#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#include <stdio.h>

#ifdef _DEBUG
#include <psapi.h>
#endif	// _DEBUG

#include "vfdtypes.h"
#include "vfdapi.h"
#include "vfdlib.h"

//
// DLL Global variables
//

extern HINSTANCE	g_hDllModule = NULL;	// Handle to this DLL itself
extern UINT			g_cDllRefCnt = 0;		// Reference count of this DLL
extern UINT			g_nNotifyMsg = 0;		// VFD notification message

//
// DllMain
//
BOOL WINAPI DllMain(
	HINSTANCE		hInstance,
	DWORD			dwReason,
	LPVOID			lpReserved)
{
#ifdef _DEBUG
	char			name[MAX_PATH];
	HMODULE			hMod;
	DWORD			size;

	if (EnumProcessModules(GetCurrentProcess(), &hMod, sizeof(hMod), &size)) {
		GetModuleBaseName(GetCurrentProcess(), hMod, name, sizeof(name));
	}
	else {
		strcpy_s(name, sizeof(name), "unknown");
	}
#endif	// _DEBUG

	UNREFERENCED_PARAMETER(lpReserved);

	if (dwReason == DLL_PROCESS_ATTACH) {
		VFDTRACE(0, ("DLL_PROCESS_ATTACH - %s\n", name));

		//	this DLL doesn't need DLL_THREAD_ATTACH and DLL_THREAD_DETACH
		DisableThreadLibraryCalls(hInstance);

		//	store the DLL instance handle
		g_hDllModule = hInstance;

		//	register the VFD notification message
		g_nNotifyMsg = RegisterWindowMessage(VFD_NOTIFY_MESSAGE);

	}
	else if (dwReason == DLL_PROCESS_DETACH) {
		VFDTRACE(0, ("DLL_PROCESS_DETACH - %s\n", name));
	}

	return TRUE;
}


//
//	Get VFD notification message value
//
UINT WINAPI VfdGetNotifyMessage()
{
	return g_nNotifyMsg;
}


//
//	Get message text from this DLL module
//
PSTR ModuleMessage(
	DWORD			nFormat, ...)
{
	PSTR	p;
	va_list	args;

	va_start(args, nFormat);

	if (!FormatMessage(
		FORMAT_MESSAGE_FROM_HMODULE |
		FORMAT_MESSAGE_ALLOCATE_BUFFER,
		g_hDllModule, nFormat, 0, (LPTSTR)&p, 0, &args)) {
		p = NULL;
	}

	va_end(args);

	return p;
}

//
//	Get system error message string
//
PCSTR SystemMessage(
	DWORD			nError)
{
	static CHAR		msg[256];

	if (!FormatMessage(
		FORMAT_MESSAGE_FROM_SYSTEM |
		FORMAT_MESSAGE_IGNORE_INSERTS,
		NULL, nError, 0, msg, sizeof(msg), NULL)) {

		sprintf_s(msg, sizeof(msg),
			"Unknown system error %lu (0x%08x)\n", nError, nError);
	}

	return msg;
}

#ifdef _DEBUG
//
//	Format and output debug string
//
void DebugTrace(
	PCSTR			sFormat, ...)
{
	CHAR			buf[512];
	int				len;
	va_list			args;

	len = sprintf_s(buf, sizeof(buf),
		"%s(%lu) : ", TraceFile, TraceLine);

	va_start(args, sFormat);

	vsprintf_s(buf + len, sizeof(buf) - len, sFormat, args);

	OutputDebugString(buf);
}
#endif	// _DEBUG
