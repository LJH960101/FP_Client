// Fill out your copyright notice in the Description page of Project Settings.

#include "NetworkSystem.h"
#include "Steamworks/Steamv139/sdk/public/steam/steam_api.h"
#include "NetworkModule/Serializer.h"
#include "NetworkModule/MyTool.h"
#include <string>
#include <fstream>

using namespace MyTool;
using std::mutex;

UNetworkSystem::UNetworkSystem()
{
}
UNetworkSystem::~UNetworkSystem()
{

}
bool UNetworkSystem::OnSteam()
{
	if (!bOnSteam)
	{
		return false;
	}
	else bOnSteam = SteamAPI_IsSteamRunning();

	return bOnSteam;
}

UTexture2D* UNetworkSystem::GetSteamAvartar(const UINT64 steamID)
{
	uint32 Width;
	uint32 Height;

	if (OnSteam())
	{
		CSteamID PlayerRawID(steamID);
		//Getting the PictureID from the SteamAPI and getting the Size with the ID
		int Picture = SteamFriends()->GetMediumFriendAvatar(PlayerRawID);
		if (Picture == 0) return nullptr;
		SteamUtils()->GetImageSize(Picture, &Width, &Height);


		if (Width > 0 && Height > 0)
		{
			//Creating the buffer "oAvatarRGBA" and then filling it with the RGBA Stream from the Steam Avatar
			BYTE *oAvatarRGBA = new BYTE[Width * Height * 4];


			//Filling the buffer with the RGBA Stream from the Steam Avatar and creating a UTextur2D to parse the RGBA Steam in
			SteamUtils()->GetImageRGBA(Picture, (uint8*)oAvatarRGBA, 4 * Height * Width * sizeof(char));

			//Swap R and B channels because for some reason the games whack
			for (uint32 i = 0; i < (Width * Height * 4); i += 4)
			{
				uint8 Temp = oAvatarRGBA[i + 0];
				oAvatarRGBA[i + 0] = oAvatarRGBA[i + 2];
				oAvatarRGBA[i + 2] = Temp;
			}

			UTexture2D* Avatar = UTexture2D::CreateTransient(Width, Height, PF_B8G8R8A8);

			//MAGIC!
			uint8* MipData = (uint8*)Avatar->PlatformData->Mips[0].BulkData.Lock(LOCK_READ_WRITE);
			FMemory::Memcpy(MipData, (void*)oAvatarRGBA, Height * Width * 4);
			Avatar->PlatformData->Mips[0].BulkData.Unlock();

			//Setting some Parameters for the Texture and finally returning it
			Avatar->PlatformData->NumSlices = 1;
			Avatar->NeverStream = true;
			//Avatar->CompressionSettings = TC_EditorIcon;

			Avatar->UpdateResource();
			delete[] oAvatarRGBA;
			return Avatar;
		}
		return nullptr;
	}
	return nullptr;
}

UTexture2D* UNetworkSystem::GetSteamAvartar(const FString steamID)
{
	return GetSteamAvartar(FStringToUINT64(steamID));
}

FString UNetworkSystem::GetSteamName(const UINT64 steamID)
{
	if (OnSteam()) {
		if (SteamFriends()->GetFriendPersonaName(steamID) == NULL) return "???";
		std::string retval = SteamFriends()->GetFriendPersonaName(steamID);
		return UTF8_TO_TCHAR(retval.c_str());
	}
	else {
		return FString::Printf(TEXT("%llu"), steamID);
	}
}

FString UNetworkSystem::GetSteamName(const FString steamID)
{
	return GetSteamName(FStringToUINT64(steamID));
}

bool UNetworkSystem::OnServer()
{
	return bOnServer;
}

bool UNetworkSystem::IsRecvDataEmpty()
{
	bool isEmpty;
	mt_queue.lock();
	isEmpty = recvQueue.IsEmpty();
	mt_queue.unlock();
	return isEmpty;
}

void UNetworkSystem::GetData(FReciveData& outData)
{
	FReciveData queueData;
	mt_queue.lock();
	recvQueue.Dequeue(queueData);
	mt_queue.unlock();
	outData = queueData;
}

bool UNetworkSystem::InitSteam()
{
	if (SteamAPI_Init()) {
		MYLOG(Warning, TEXT("Steam is Running!"));
		bOnSteam = true;
	}
	else {
		MYLOG(Warning, TEXT("Steam not working."));
		bOnSteam = false;
	}
	return bOnSteam;
}

void UNetworkSystem::ShutdownSteam()
{
	if (bOnSteam) {
		MYLOG(Warning, TEXT("Steam shutdown."));
		SteamAPI_Shutdown();
		bOnSteam = false;
	}
}

bool UNetworkSystem::InitSocket()
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
	optval = false; // 네이글 알고리즘을 끈다.
	retval = setsockopt(client_sock, IPPROTO_TCP, TCP_NODELAY, (char*)&optval, sizeof(optval));
	if (retval == SOCKET_ERROR) {
		return false;
	}

	if (connect(client_sock, (SOCKADDR *)&serveraddr, sizeof(serveraddr)) == INVALID_SOCKET) return false;

	sock = client_sock;
	bOnServer = true;

	DWORD threadID;
	hThread = CreateThread(NULL, 0, UNetworkSystem::RecvThread, this, 0, &threadID);
	MYLOG(Warning, TEXT("Socket Init Success"));
	return true;
}

void UNetworkSystem::ShutdownSocket()
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

void UNetworkSystem::Send(const char * buf, int len)
{
	if (OnServer()) {
		MyTool::Send(sock, buf, len, 0);
	}
}

UINT64 UNetworkSystem::GetSteamID()
{
	if (OnSteam()) {
		return SteamUser()->GetSteamID().ConvertToUint64();
	}
	else {
		return defaultSteamID;
	}
}

FString UNetworkSystem::GetSteamIDofString()
{
	return UINT64ToFString(GetSteamID());
}

DWORD WINAPI UNetworkSystem::RecvThread(LPVOID arg)
{
	UNetworkSystem* owner = (UNetworkSystem*)arg;
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

void UNetworkSystem::Init() {
	if (!InitSteam()) {
		// 스팀 실패 처리
	}
	if (!InitSocket()) {
		MYLOG(Warning, TEXT("Socket Init Failed"));
	}
}

void UNetworkSystem::Shutdown()
{
	MYLOG(Warning, TEXT("SHUTDOWN"));
	ShutdownSteam();
	ShutdownSocket();
}

void UNetworkSystem::SetSteamID_DUBGE(const UINT64 & steamID)
{
	defaultSteamID = steamID;
	ShutdownSteam();

	// 변경된 정보 송신
	if (steamID == 0) return;
	char sendBuf[sizeof(UINT64) + sizeof(EMessageType)], idBuf[sizeof(UINT64)];
	int len = CSerializer::UInt64Serializer(idBuf, steamID);
	int totalLen = CSerializer::SerializeWithEnum(C_Common_AnswerId, idBuf, len, sendBuf);
	Send(sendBuf, totalLen);
}
