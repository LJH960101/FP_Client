// Fill out your copyright notice in the Description page of Project Settings.

#pragma once
#include "NetworkTest.h"
#include "Engine/GameInstance.h"
#include "AllowWindowsPlatformTypes.h"
#include <WinSock2.h>
#include "HideWindowsPlatformTypes.h"
#include <mutex>
#include "Containers/Queue.h"
#include "NetworkGameInstance.generated.h"

#define BUFSIZE 1024
#define SERVER_IP "127.0.0.1"
#define SERVER_PORT 15470
#define IP_TEXT_NAME "ip.txt"

struct FReciveData
{
	char buf[BUFSIZE];
	int len;
};

UCLASS()
class NETWORKTEST_API UNetworkGameInstance : public UGameInstance
{
	GENERATED_BODY()
public:
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

	static FString UINT64ToFString(const UINT64 val);
	static UINT64 FStringToUINT64(const FString val);

private:
	virtual void Init();
	virtual void Shutdown();

	// Steam
	void InitSteam();
	void ShutdownSteam();
	bool bOnSteam;

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