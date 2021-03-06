#include "MyTool.h"
#include "Serializer.h"

int MyTool::Send(SOCKET sock, const char * buf, int len, int flags)
{
	char* newBuf = new char[len + sizeof(int)];
	int intLen = CSerializer::IntSerialize(newBuf, len);
	memcpy(newBuf + intLen, buf, len);
	int retval = send(sock, newBuf, len + intLen, flags);
	delete[] newBuf;
	return retval;
}

FString MyTool::UINT64ToFString(const UINT64 val)
{
	return FString::Printf(TEXT("%llu"), val);
}

UINT64 MyTool::FStringToUINT64(const FString val)
{
	return FCString::Strtoui64(*val, NULL, 10);
}