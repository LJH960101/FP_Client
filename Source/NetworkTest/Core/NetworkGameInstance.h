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

	// ���� �����Ͱ� ����°�?
	bool IsRecvDataEmpty();
	// �����͸� �޾ƿ´�.
	void GetData(FReciveData& outData);
	void Send(const char* buf, int len);
	UINT64 GetSteamID();

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
	// ������ ���� ����. �޸� ������ ���Ѱ�. ������ �ܿ��� ������� �ʴ´�.
	char recvBuf[BUFSIZE];
	HANDLE hThread;
	TQueue<FReciveData> recvQueue;
	std::mutex mt_queue;
};