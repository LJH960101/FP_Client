// Fill out your copyright notice in the Description page of Project Settings.

#include "LobbyManager.h"
#include "FriendInfo.h"
#include "../NetworkModule/Serializer.h"
#include "../Core/NetworkGameInstance.h"

// Sets default values
ALobbyManager::ALobbyManager()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = false;

}

void ALobbyManager::RefreshList()
{
	friendInfos->RefreshFriendInfo();
}

// Called when the game starts or when spawned
void ALobbyManager::BeginPlay()
{
	Super::BeginPlay();
	friendInfos = NewObject<UFriendInfo>(this);
	gameInstance = Cast<UNetworkGameInstance>(GetGameInstance());
	if (!gameInstance) MYLOG(Error, TEXT("Can't get gameInstance"));
}

int ALobbyManager::GetFriendLen()
{
	return friendInfos->FriendInformations.Num();
}

FString ALobbyManager::GetFriendName(const int& number)
{
	if (GetFriendLen() <= number) return "";
	return friendInfos->FriendInformations[number]->name;
}

FString ALobbyManager::GetFriendSteamID(const int& number)
{
	return FString::Printf(TEXT("%llu"), 
		friendInfos->FriendInformations[number]->steamID);
}

UTexture2D* ALobbyManager::GetFriendImage(const int& number)
{
	if (GetFriendLen() <= number) return nullptr;
	return friendInfos->FriendInformations[number]->avatar;
}

void ALobbyManager::InviteFriend(FString steamID)
{
	// 서버에 요청한다.
	UINT64 uiSteamID = FCString::Strtoui64(*steamID, NULL, 10);
	char uIntBuf[10], outBuf[20];
	int uIntLen = CSerializer::UInt64Serializer(uIntBuf, uiSteamID);
	int allLen = CSerializer::SerializeWithEnum(C_Match_InviteFriend_Request, uIntBuf, uIntLen, outBuf);
	gameInstance->Send(outBuf, allLen);
}

void ALobbyManager::AnswerInviteRequest(bool isYes)
{
}

void ALobbyManager::OnReady(bool isOn)
{
}

void ALobbyManager::Kick(int playerNumber)
{
}

UFriendInformation* ALobbyManager::GetFriendBySteamId(const UINT64& steamID)
{
	return friendInfos->GetFriendInformationBySteamID(steamID);
}
