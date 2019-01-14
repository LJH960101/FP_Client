// Fill out your copyright notice in the Description page of Project Settings.

#include "CommonNetworkProcessor.h"
#include "../Core/NetworkGameInstance.h"
#include "../NetworkModule/Serializer.h"

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
	int pos = 0;
	while (pos < data.len) {
		EMessageType type = CSerializer::GetEnum(data.buf);
		pos += sizeof(EMessageType);
		switch (type)
		{
		case COMMON_ECHO:
		{
			FSerializableString res = CSerializer::StringDeserializer(data.buf + pos);
			MYLOG(Warning, TEXT("%s"), res.buf);
			pos += res.len;
			break;
		}
		case COMMON_PING:
		{
#ifdef DEBUG_RECV
			MYLOG(Warning, TEXT("recv : COMMON_PING"));
#endif
			char newBuf[10];
			int len = CSerializer::SerializeWithEnum(COMMON_PING, nullptr, 0, newBuf);
			gameInstance->Send(newBuf, len);
			break;
		}
		case EMessageType::S_Common_RequestId:
		{
#ifdef DEBUG_RECV
			MYLOG(Warning, TEXT("recv : S_Common_RequestId"));
#endif
			UINT64 steamID = gameInstance->GetSteamID();
			if (steamID == 0) return;
			char sendBuf[50], idBuf[30];
			int len = CSerializer::UInt64Serializer(idBuf, steamID);
			int totalLen = CSerializer::SerializeWithEnum(C_Common_AnswerId, idBuf, len, sendBuf);
			gameInstance->Send(sendBuf, totalLen);
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