// Fill out your copyright notice in the Description page of Project Settings.

#include "LobbyManager.h"
#include "FriendInfo.h"
#include "Steamworks/Steamv139/sdk/public/steam/steam_api.h"
#include "../NetworkModule/Serializer.h"
#include "../Core/NetworkGameInstance.h"

// Sets default values
ALobbyManager::ALobbyManager()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = false;
	slot1 = CreateDefaultSubobject<URoomPlayer>(TEXT("Slot1"));
	slot2 = CreateDefaultSubobject<URoomPlayer>(TEXT("Slot2"));
	slot3 = CreateDefaultSubobject<URoomPlayer>(TEXT("Slot3"));
	slot4 = CreateDefaultSubobject<URoomPlayer>(TEXT("Slot4"));

	// 초대 요청 창
	static ConstructorHelpers::FClassFinder<ULobbyWG> lobbyWG(TEXT("/Game/UI/Lobby/WB_Lobby.WB_Lobby_C"));
	if (lobbyWG.Succeeded()) {
		WG_Lobby_Class = lobbyWG.Class;
	}
	else
	{
		MYLOG_S(Error);
	}

	// 친구
	static ConstructorHelpers::FClassFinder<UUserWidget> friendWG(TEXT("/Game/UI/Lobby/WB_FriendPopUp.WB_FriendPopUp_C"));
	if (friendWG.Succeeded()) {
		WG_Friend_Class = friendWG.Class;
	}
	else
	{
		MYLOG_S(Error);
	}

	// 설정
	static ConstructorHelpers::FClassFinder<UUserWidget> settingWG(TEXT("/Game/UI/Lobby/WB_Setting.WB_Setting_C"));
	if (settingWG.Succeeded()) {
		WG_Setting_Class = settingWG.Class;
	}
	else
	{
		MYLOG_S(Error);
	}
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

	if (WG_Lobby == nullptr) {
		WG_Lobby = CreateWidget<ULobbyWG>(GetWorld(), WG_Lobby_Class);
		WG_Lobby->AddToViewport();
	}
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
	return  UNetworkGameInstance::UINT64ToFString(friendInfos->FriendInformations[number]->steamID);
}

UTexture2D* ALobbyManager::GetFriendImage(const int& number)
{
	if (GetFriendLen() <= number) return nullptr;
	return friendInfos->FriendInformations[number]->avatar;
}

void ALobbyManager::InviteFriend(FString steamID)
{
	// 서버에 요청한다.
	UINT64 uiSteamID = UNetworkGameInstance::FStringToUINT64(steamID);
	char uIntBuf[10], outBuf[20];
	int uIntLen = CSerializer::UInt64Serializer(uIntBuf, uiSteamID);
	int allLen = CSerializer::SerializeWithEnum(C_Match_InviteFriend_Request, uIntBuf, uIntLen, outBuf);
	gameInstance->Send(outBuf, allLen);
}

void ALobbyManager::AnswerInviteRequest(bool isYes, const FString& steamID)
{
	char buf_isAccept[5], buf_targetId[10], buf_all[15], outBuf[20];
	int boolLen = CSerializer::BoolSerialize(buf_isAccept, isYes);
	int intLen = CSerializer::UInt64Serializer(buf_targetId, UNetworkGameInstance::FStringToUINT64(steamID));
	memcpy(buf_all, buf_isAccept, boolLen);
	memcpy(buf_all + boolLen, buf_targetId, intLen);
	int allLen = CSerializer::SerializeWithEnum(C_Match_InviteFriend_Answer, buf_all, boolLen + intLen, outBuf);
	gameInstance->Send(outBuf, allLen);
}

UFriendInformation* ALobbyManager::GetFriendBySteamId(const UINT64& steamID)
{
	return friendInfos->GetFriendInformationBySteamID(steamID);
}

FString ALobbyManager::GetNameBySteamID(const UINT64 & steamID)
{
	if (gameInstance->OnSteam()) {
		std::string retval = SteamFriends()->GetPlayerNickname(steamID);
		return UTF8_TO_TCHAR(retval.c_str());
	}
	return FString();
}

void ALobbyManager::RefreshSlot(int slotNumber, const UINT64 & steamID)
{
	
	if (slotNumber == 1) {
		if (steamID == 0) {
			slot1->steamID = 0;
			slot1->name = "";
			slot1->image = nullptr;
		}
		else {
			slot1->steamID = steamID;
			slot1->name = GetNameBySteamID(steamID);
			slot1->image = friendInfos->GetSteamAvatar(steamID);
		}
	}
	else if (slotNumber == 2) {
		if (steamID == 0) {
			slot2->steamID = 0;
			slot2->name = "";
			slot2->image = nullptr;
		}
		else {
			slot2->steamID = steamID;
			slot2->name = GetNameBySteamID(steamID);
			slot2->image = friendInfos->GetSteamAvatar(steamID);
		}
	}
	else if (slotNumber == 3) {
		if (steamID == 0) {
			slot3->steamID = 0;
			slot3->name = "";
			slot3->image = nullptr;
		}
		else {
			slot3->steamID = steamID;
			slot3->name = GetNameBySteamID(steamID);
			slot3->image = friendInfos->GetSteamAvatar(steamID);
		}
	}
	else if (slotNumber == 4) {
		if (steamID == 0) {
			slot4->steamID = 0;
			slot4->name = "";
			slot4->image = nullptr;
		}
		else {
			slot4->steamID = steamID;
			slot4->name = GetNameBySteamID(steamID);
			slot4->image = friendInfos->GetSteamAvatar(steamID);
		}
	}
}

void ALobbyManager::RefreshLobby()
{
	WG_Lobby->SetPartySlot(1, slot1->name, slot1->image);
	WG_Lobby->SetPartySlot(2, slot2->name, slot2->image);
	WG_Lobby->SetPartySlot(3, slot3->name, slot3->image);
	WG_Lobby->SetPartySlot(4, slot4->name, slot4->image);
}

void ALobbyManager::ClickSetting()
{
	if (WG_Setting == nullptr) {
		WG_Setting = CreateWidget<UUserWidget>(GetWorld(), WG_Setting_Class);
		onSetting = false;
	}

	if (onSetting) {
		WG_Setting->RemoveFromParent();
		onSetting = false;
	}
	else {
		WG_Setting->AddToViewport();
		onSetting = true;
	}
}

void ALobbyManager::ClickFriend()
{
	if (WG_Friend == nullptr) {
		WG_Friend = CreateWidget<UUserWidget>(GetWorld(), WG_Friend_Class);
		onFriend = false;
	}

	if (onFriend) {
		WG_Friend->RemoveFromParent();
		onFriend = false;
	}
	else {
		WG_Friend->AddToViewport();
		onFriend = true;
	}
}

void ALobbyManager::OnReady(bool isOn)
{
}

void ALobbyManager::Kick(int playerNumber)
{
}
