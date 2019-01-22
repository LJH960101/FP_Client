// Fill out your copyright notice in the Description page of Project Settings.

#include "LobbyNetworkProcessor.h"
#include "Core/NetworkGameInstance.h"
#include "Core/Network/NetworkSystem.h"
#include "Lobby/LobbyManager.h"
#include "Lobby/FriendInfo.h"
#include "Blueprint/UserWidget.h"
#include "NetworkModule/Serializer.h"
#include "NetworkModule/MyTool.h"

using namespace MyTool;

// Sets default values
ALobbyNetworkProcessor::ALobbyNetworkProcessor()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
}

// Called when the game starts or when spawned
void ALobbyNetworkProcessor::BeginPlay()
{
	Super::BeginPlay();
	TArray<AActor*> actors;
	UGameplayStatics::GetAllActorsOfClass(GetWorld(), ALobbyManager::StaticClass(), actors);
	if (actors.Num() == 0) {
		MYLOG(Error, TEXT("Can't find lobbyManager"));
	}
	else if (actors.Num() > 1) {
		MYLOG(Error, TEXT("Too many lobbyManager"));
	}
	else {
		LobbyManager = (ALobbyManager*)actors[0];
	}
}

// Called every frame
void ALobbyNetworkProcessor::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	auto gameInstance = GetGameInstance();
	// 수신 데이터가 있음.
	if (gameInstance) {
		while(!gameInstance->GetNetworkSystem()->IsRecvDataEmpty())
		{
			FReciveData newData;
			gameInstance->GetNetworkSystem()->GetData(newData);
			RecvProc(newData);
		} 
	}
}

void ALobbyNetworkProcessor::RecvProc(FReciveData& data)
{
	Super::RecvProc(data);
	int cursor = 0;
	while (cursor < data.len) {
		int bufLen = CSerializer::IntDeserialize(data.buf, cursor) - sizeof(EMessageType);
		EMessageType type = CSerializer::GetEnum(data.buf, cursor);
		UINT64 sid = Cast<UNetworkGameInstance>(GetGameInstance())->GetNetworkSystem()->GetSteamID();
		MYLOG(Warning, TEXT("%llu : %d"), sid, type);
		switch (type)
		{
		case EMessageType::S_Room_Info:
		{
			// 4개의 UINT64를 받아, 슬롯에 차례대로 넣어준다.
			UINT64 slot;
			for (int i = 0; i < MAX_PLAYER; ++i) {
				slot = CSerializer::UInt64Deserializer(data.buf, cursor);
				LobbyManager->ChangeSlot(i, slot);
			}
			LobbyManager->RefreshLobby();
			break;
		}
		case EMessageType::S_Room_ChangeState:
		{
			// 1개의 int와 UINT를 받아 슬롯을 수정한다.
			int slot = CSerializer::IntDeserialize(data.buf, cursor);
			UINT64 steamID = CSerializer::IntDeserialize(data.buf, cursor);
			LobbyManager->ChangeSlot(slot, steamID);
			LobbyManager->RefreshLobby();
			break;
		}
		case EMessageType::S_Lobby_InviteFriend_Request:
		{
			UINT64 senderID = CSerializer::UInt64Deserializer(data.buf, cursor);
			FString stringSenderID = UINT64ToFString(senderID);
			FString name;
			if(LobbyManager->GetFriendBySteamId(senderID)) name = LobbyManager->GetFriendBySteamId(senderID)->name;
			else name = "Unknown";
			LobbyManager->OpenRequestInviteWG(stringSenderID, name);
			break;
		}
		case EMessageType::S_Lobby_InviteFriend_Failed:
		{
			FSerializableString msg = CSerializer::StringDeserializer(data.buf, cursor);
			LobbyManager->OpenLobbyFailedWG(msg.buf);
			break;
		}
		case EMessageType::S_Lobby_MatchAnswer:
		{
			bool isOn = CSerializer::BoolDeserialize(data.buf, cursor);
			int slot = CSerializer::IntDeserialize(data.buf, cursor);
			LobbyManager->SetReadyButton(slot, isOn);
			break;
		}
		case EMessageType::S_Lobby_GameStart:
			UGameplayStatics::OpenLevel(GetWorld(), "InGame");
			return;
		default:
		{
			cursor += bufLen;
			break;
		}
		}
	}
}

