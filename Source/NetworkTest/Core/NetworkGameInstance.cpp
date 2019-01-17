// Fill out your copyright notice in the Description page of Project Settings.

#include "NetworkGameInstance.h"
#include "Steamworks/Steamv139/sdk/public/steam/steam_api.h"
#include <string>
#include <fstream>

using std::mutex;

bool UNetworkGameInstance::OnSteam()
{
	if (!bOnSteam)
	{
		InitSteam();
	}
	else bOnSteam = SteamAPI_IsSteamRunning();

	return bOnSteam;
}

bool UNetworkGameInstance::OnServer()
{
	if (!bOnServer)
	{
		InitSocket();
	}

	return bOnServer;
}

bool UNetworkGameInstance::IsRecvDataEmpty()
{
	bool isEmpty;
	mt_queue.lock();
	isEmpty = recvQueue.IsEmpty();
	mt_queue.unlock();
	return isEmpty;
}

void UNetworkGameInstance::GetData(FReciveData& outData)
{
	FReciveData queueData;
	mt_queue.lock();
	recvQueue.Dequeue(queueData);
	mt_queue.unlock();
	outData = queueData;
}

void UNetworkGameInstance::InitSteam()
{
	if (SteamAPI_Init()) {
		MYLOG(Warning, TEXT("Steam is Running!"));
		bOnSteam = true;
	}
	else {
		MYLOG(Warning, TEXT("Steam not working."));
		bOnSteam = false;
	}
}

void UNetworkGameInstance::ShutdownSteam()
{
	if (bOnSteam) {
		MYLOG(Warning, TEXT("Steam shutdown."));
		SteamAPI_Shutdown();
	}
}

bool UNetworkGameInstance::InitSocket()
{
	if (bOnServer) return true;

	sock = NULL;
	bOnServer = false;

	WSAData wsa;
	if (WSAStartup(MAKEWORD(2, 2), &wsa) != 0) return false;
	
	// Addr Define
	SOCKADDR_IN serveraddr;
	ZeroMemory(&serveraddr, sizeof(serveraddr));
	serveraddr.sin_family = AF_INET;

	// IP가 적힌 텍스트 파일이 있다면, 불러온다.
	std::ifstream ipText("C:\\ip.txt");
	if (ipText) {
		char buf[100];
		ipText >> buf;
		serveraddr.sin_addr.s_addr = inet_addr(buf);
		MYLOG(Warning, TEXT("%s"), buf);
		
	}
	else serveraddr.sin_addr.s_addr = inet_addr(SERVER_IP);

	serveraddr.sin_port = htons(SERVER_PORT);

	SOCKET client_sock = socket(AF_INET, SOCK_STREAM, 0);
	if (client_sock == INVALID_SOCKET) return false;

	int optval = 60000; // 타임아웃은 1분으로 지정한다.
	int retval = setsockopt(client_sock, SOL_SOCKET, SO_RCVTIMEO, (char*)&optval, sizeof(optval));
	if (retval == SOCKET_ERROR) {
		return false;
	}
	optval = false; // 네이클 알고리즘을 끈다.
	retval = setsockopt(client_sock, IPPROTO_TCP, TCP_NODELAY, (char*)&optval, sizeof(optval));
	if (retval == SOCKET_ERROR) {
		return false;
	}

	if (connect(client_sock, (SOCKADDR *)&serveraddr, sizeof(serveraddr)) == INVALID_SOCKET) return false;

	sock = client_sock;
	bOnServer = true;

	DWORD threadID;
	hThread = CreateThread(NULL, 0, UNetworkGameInstance::RecvThread, this, 0, &threadID);
	MYLOG(Warning, TEXT("Socket Init Success"));
	return true;
}

void UNetworkGameInstance::ShutdownSocket()
{
	if (OnServer()) {
		TerminateThread(hThread, 0);
		CloseHandle(hThread);
		hThread = NULL;
		closesocket(sock);
		WSACleanup();
		MYLOG(Warning, TEXT("Socket Shutdown Success"));
	}
}

void UNetworkGameInstance::Send(const char * buf, int len)
{
	if (OnServer()) {
		send(sock, buf, len, 0);
	}
}

UINT64 UNetworkGameInstance::GetSteamID()
{
	if (OnSteam()) {
		return SteamUser()->GetSteamID().ConvertToUint64();
	}
	else return 0;
}

FString UNetworkGameInstance::UINT64ToFString(const UINT64 val)
{
	return FString::Printf(TEXT("%llu"), val);
}

UINT64 UNetworkGameInstance::FStringToUINT64(const FString val)
{
	return FCString::Strtoui64(*val, NULL, 10);
}

DWORD WINAPI UNetworkGameInstance::RecvThread(LPVOID arg)
{
	UNetworkGameInstance* owner = (UNetworkGameInstance*)arg;
	if (owner == nullptr) {
		MYLOG(Warning, TEXT("Can't cast in Recv Thread"));
		return 0;
	}
	while (true) {
		int retval = recv(owner->sock, owner->recvBuf, BUFSIZE, 0);
		if (retval == SOCKET_ERROR) continue; // 소켓 에러는 그냥 무시하고 다음껄 받도록 시킨다.
		if (retval == 0) return 0; // 소켓 종료
		FReciveData newData;
		if (retval >= BUFSIZE) {
			MYLOG(Error, TEXT("Recv : Too long buf size.."));
			continue;
		}
		memcpy(newData.buf, owner->recvBuf, retval);
		newData.len = retval;
		owner->mt_queue.lock();
		owner->recvQueue.Enqueue(newData);
		owner->mt_queue.unlock();
	}
	return 0;
}

void UNetworkGameInstance::Init() {
	Super::Init();
	InitSteam();
	if(!InitSocket()) MYLOG(Warning, TEXT("Socket Init Failed"));
}

void UNetworkGameInstance::Shutdown()
{
	Super::Shutdown();
	MYLOG(Warning, TEXT("SHUTDOWN"));
	ShutdownSteam();
	ShutdownSocket();
}