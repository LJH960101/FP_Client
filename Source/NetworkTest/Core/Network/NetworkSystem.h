/*
	��Ʈ��ũ �ý����� �����ϴ� �����ν��Ͻ� Ŭ����
*/

#pragma once

#define _WINSOCK_DEPRECATED_NO_WARNINGS
#include "NetworkTest.h"
#include "NetworkModule/GameInfo.h"
#include "AllowWindowsPlatformTypes.h"
#include <WinSock2.h>
#include "HideWindowsPlatformTypes.h"
#include <mutex>
#include "Containers/Queue.h"
#include "NetworkSystem.generated.h"

struct FReciveData
{
	char buf[BUFSIZE];
	int len;
};

UCLASS()
class NETWORKTEST_API UNetworkSystem : public UObject
{
	GENERATED_BODY()
public:
	UNetworkSystem();
	~UNetworkSystem();
	UFUNCTION(BlueprintCallable)
	bool OnSteam();
	UFUNCTION(BlueprintCallable)
	bool OnServer();
	// ���� �����Ͱ� ����°�?
	bool IsRecvDataEmpty();
	// �����͸� �޾ƿ´�.
	void GetData(FReciveData& outData);
	void Send(const char* buf, int len);
	UINT64 GetSteamID();
	UFUNCTION(BlueprintCallable)
	FString GetSteamIDofString();

	void Init();
	void Shutdown();

	void SetSteamID_DUBGE(const UINT64& steamID);

	// Steam
	UTexture2D* GetSteamAvartar(const UINT64 steamID);
	UFUNCTION(BlueprintCallable)
	UTexture2D* GetSteamAvartar(const FString steamID);
	FString GetSteamName(const UINT64 steamID);
	UFUNCTION(BlueprintCallable)
	FString GetSteamName(const FString steamID);

private:
	// Steam
	bool InitSteam();
	void ShutdownSteam();
	bool bOnSteam;
	UINT64 defaultSteamID;

	// Socket
	bool InitSocket();
	void ShutdownSocket();
	bool bOnServer;
	SOCKET sock;

	// Client System
	static DWORD WINAPI RecvThread(LPVOID arg);
	// ������ ���� ����. �޸� ������ ���Ѱ�. ������ �ܿ��� ������� �ʴ´�.
	char recvBuf[BUFSIZE];
	HANDLE hThread;
	TQueue<FReciveData> recvQueue;
	std::mutex mt_queue;
};