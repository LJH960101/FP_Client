// Fill out your copyright notice in the Description page of Project Settings.

#include "CommonNetworkProcessor.h"
#include "Core/NetworkGameInstance.h"
#include "Core/Network/NetworkSystem.h"
#include "NetworkModule/Serializer.h"

// Sets default values
ACommonNetworkProcessor::ACommonNetworkProcessor()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = false;

}

UNetworkGameInstance* ACommonNetworkProcessor::GetGameInstance() {
	return gameInstance;
}

// Called when the game starts or when spawned
void ACommonNetworkProcessor::BeginPlay()
{
	Super::BeginPlay();

	gameInstance = Cast<UNetworkGameInstance>(AActor::GetGameInstance());
	if (gameInstance == nullptr) MYLOG(Warning, TEXT("Can't find gameInstance."));
}

void ACommonNetworkProcessor::RecvProc(FReciveData& data)
{
	int cursor = 0;
	while (cursor < data.len) {
		int bufLen = CSerializer::IntDeserialize(data.buf, cursor) - sizeof(EMessageType);
		//if (bufLen <= 0) return;
		EMessageType type = CSerializer::GetEnum(data.buf, cursor);
		switch (type)
		{
		case COMMON_ECHO:
		{
			FSerializableString res = CSerializer::StringDeserializer(data.buf, cursor);
			MYLOG(Warning, TEXT("%s"), res.buf);
			break;
		}
		case COMMON_PING:
		{
#ifdef DEBUG_RECV
			MYLOG(Warning, TEXT("recv : COMMON_PING"));
#endif
			char newBuf[sizeof(EMessageType)];
			int len = CSerializer::SerializeEnum(COMMON_PING, newBuf);
			gameInstance->GetNetworkSystem()->Send(newBuf, len);
			break;
		}
		case EMessageType::S_Common_RequestId:
		{
#ifdef DEBUG_RECV
			MYLOG(Warning, TEXT("recv : S_Common_RequestId"));
#endif
			UINT64 steamID = gameInstance->GetNetworkSystem()->GetSteamID();
			if (steamID == 0) return;
			char sendBuf[sizeof(EMessageType) + sizeof(UINT64)], idBuf[sizeof(UINT64)];
			int len = CSerializer::UInt64Serializer(idBuf, steamID);
			int totalLen = CSerializer::SerializeWithEnum(C_Common_AnswerId, idBuf, len, sendBuf);
			gameInstance->GetNetworkSystem()->Send(sendBuf, totalLen);
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