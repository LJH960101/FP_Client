// Fill out your copyright notice in the Description page of Project Settings.

#include "LobbyNetworkProcessor.h"
#include "../Core/NetworkGameInstance.h"
#include "../NetworkModule/Serializer.h"

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
		EMessageType type = CSerializer::GetEnum(data.buf);
		pos += sizeof(EMessageType);
		switch (type)
		{
		case EMessageType::S_Match_InviteFriend_Answer:
		{
			break;
		}
		case EMessageType::S_Match_InviteFriend_Request:
		{
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

