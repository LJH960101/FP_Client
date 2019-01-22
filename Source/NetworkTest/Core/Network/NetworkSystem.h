/*
	네트워크 시스템을 관할하는 게임인스턴스 클래스
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
	// 수신 데이터가 비었는가?
	bool IsRecvDataEmpty();
	// 데이터를 받아온다.
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
	// 쓰레드 전용 버퍼. 메모리 관리를 위한것. 쓰레드 외에는 사용하지 않는다.
	char recvBuf[BUFSIZE];
	HANDLE hThread;
	TQueue<FReciveData> recvQueue;
	std::mutex mt_queue;
};