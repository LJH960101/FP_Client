// Fill out your copyright notice in the Description page of Project Settings.

#include "InGameNetworkProcessor.h"
#include "Core/NetworkGameInstance.h"
#include "Core/Network/NetworkSystem.h"
#include "InGame/InGameManager.h"
#include "NetworkModule/Serializer.h"
#include "NetworkModule/MyTool.h"

using namespace MyTool;

// Sets default values
AInGameNetworkProcessor::AInGameNetworkProcessor()
{
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
}

void AInGameNetworkProcessor::BeginPlay()
{
	Super::BeginPlay();

	// 인게임 매니저를 찾는다.
	TArray<AActor*> actors;
	UGameplayStatics::GetAllActorsOfClass(GetWorld(), AInGameManager::StaticClass(), actors);
	if (actors.Num() == 0) {
		MYLOG(Error, TEXT("Can't find AInGameManager"));
	}
	else if (actors.Num() > 1) {
		MYLOG(Error, TEXT("Too many AInGameManager"));
	}
	else {
		inGameManager = (AInGameManager*)actors[0];
	}

	gameInstance = GetGameInstance();
	if (!gameInstance) {
		MYLOG(Error, TEXT("Can't Find GameInstance"));
		return;
	}

	// 서버는 붙지만, 스팀이 꺼져있다면, 강제 방접속을 시도한다.
	networkSystem = gameInstance->GetNetworkSystem();
	if (networkSystem && networkSystem->OnServer() && !networkSystem->OnSteam()) {
		UINT64 randID = rand() % 100000;
		SetSteamID_DEBUG(FString::Printf(TEXT("%llu"), randID));
		EnterToGame_DEBUG();
		MYLOG(Error, TEXT("Try Force Game Join..."));
	}
}

void AInGameNetworkProcessor::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	if (!networkSystem || !networkSystem->OnServer()) return;

	// 수신 데이터가 있음.
	while (!networkSystem->IsRecvDataEmpty())
	{
		FReciveData newData;
		networkSystem->GetData(newData);
		RecvProc(newData);
	}
}

void AInGameNetworkProcessor::RecvProc(FReciveData & data)
{
	Super::RecvProc(data);
	int cursor = 0;
	while (cursor < data.len) {
		int bufLen = CSerializer::IntDeserialize(data.buf, cursor) - sizeof(EMessageType);
		EMessageType type = CSerializer::GetEnum(data.buf, cursor);
		UINT64 sid = networkSystem->GetSteamID();
		MYLOG(Warning, TEXT("%llu : %d"), sid, type);
		switch (type)
		{
		case EMessageType::C_Debug_GameStart:
		{
			break;
		}
		default:
		{
			cursor += bufLen;
			break;
		}
		}
	}
}

void AInGameNetworkProcessor::EnterToGame_DEBUG()
{
	char buf[sizeof(EMessageType)];
	CSerializer::SerializeEnum(C_Debug_GameStart, buf);
	networkSystem->Send(buf, sizeof(EMessageType));
}

void AInGameNetworkProcessor::SetSteamID_DEBUG(const FString & steamID)
{
	networkSystem->SetSteamID_DUBGE(FStringToUINT64(steamID));
}