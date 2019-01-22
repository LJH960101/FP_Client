#pragma once
#include "NetworkTest.h"
#define _WINSOCK_DEPRECATED_NO_WARNINGS
#include "AllowWindowsPlatformTypes.h"
#include <WinSock2.h>
#include "HideWindowsPlatformTypes.h"

namespace MyTool {
	// ���̸� �Ǿտ� �ٿ��� ������.
	int Send(SOCKET sock, const char* buf, int len, int flags = 0);
	FString UINT64ToFString(const UINT64 val);
	UINT64 FStringToUINT64(const FString val);
}