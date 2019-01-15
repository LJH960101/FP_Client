// Fill out your copyright notice in the Description page of Project Settings.

#include "LobbyNetworkProcessor.h"
#include "../Core/NetworkGameInstance.h"
#include "../NetworkModule/Serializer.h"
#include "LobbyManager.h"
#include "Blueprint/UserWidget.h"
#include "WidgetFriendRequest.h"
#include "FriendInfo.h"

// Sets default values
ALobbyNetworkProcessor::ALobbyNetworkProcessor()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	static ConstructorHelpers::FClassFinder<UWidgetFriendRequest> inviteRequestWG(TEXT("/Game/UI/Lobby/WB_FriendRequest.WB_FriendRequest_C"));
	if (inviteRequestWG.Succeeded()) {
		WG_RequestInvite_Class = inviteRequestWG.Class;
	}
	else
	{
		GEngine->AddOnScreenDebugMessage(-1, 15.0f, FColor::Red, "inviteRequestWG not found !");
	}
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
		while(!gameInstance->IsRecvDataEmpty())
		{
			FReciveData newData;
			gameInstance->GetData(newData);
			RecvProc(newData);
		} 
	}
}

void ALobbyNetworkProcessor::RecvProc(FReciveData& data)
{
	Super::RecvProc(data);
	int pos = 0;
	while (pos < data.len) {
		EMessageType type = CSerializer::GetEnum(data.buf + pos);
		pos += sizeof(EMessageType);
		switch (type)
		{
		case EMessageType::S_Match_InviteFriend_Answer:
		{
			break;
		}
		case EMessageType::S_Match_InviteFriend_Request:
		{
			UINT64 senderID = CSerializer::UInt64Deserializer(data.buf + pos);
			FString stringSenderID = FString::Printf(TEXT("%llu"), senderID);
			// 초대 요청 창을 띄운당.
			if (WG_RequestInvite == nullptr) WG_RequestInvite = CreateWidget<UWidgetFriendRequest>(GetWorld(), WG_RequestInvite_Class);
			WG_RequestInvite->AddToViewport();
			WG_RequestInvite->SetSteamID(stringSenderID);
			WG_RequestInvite->SetUserName(LobbyManager->GetFriendBySteamId(senderID)->name);
			pos += sizeof(UINT64);
			break;
		}
		case EMessageType::S_Match_FriendKick_Notification:
		{
			break;
		}
		case EMessageType::S_Match_Start_Answer:
		{
			break;
		}
		case EMessageType::S_Match_Success_Notification:
		{
			break;
		}
		default:
		{
			pos += CSerializer::GetSizeNextOfEnum(type, data.buf + pos);
			break;
		}
		}
	}
}

