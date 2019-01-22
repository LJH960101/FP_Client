// Fill out your copyright notice in the Description page of Project Settings.

#define  _CRT_SECURE_NO_WARNINGS
#include "LobbyManager.h"
#include "FriendInfo.h"
#include "Steamworks/Steamv139/sdk/public/steam/steam_api.h"
#include "NetworkModule/Serializer.h"
#include "Core/NetworkGameInstance.h"
#include "Core/Network/NetworkSystem.h"
#include "Common/UI/FailedWG.h"
#include "WidgetFriendRequest.h"
#include "Network/LobbyNetworkProcessor.h"
#include "NetworkModule/MyTool.h"

using namespace MyTool;

// Sets default values
ALobbyManager::ALobbyManager()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = false;

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

	// 초대 요청 창
	static ConstructorHelpers::FClassFinder<UWidgetFriendRequest> inviteRequestWG(TEXT("/Game/UI/Lobby/WB_FriendRequest.WB_FriendRequest_C"));
	if (inviteRequestWG.Succeeded()) {
		WG_RequestInvite_Class = inviteRequestWG.Class;
	}
	else
	{
		MYLOG_S(Error);
	}

	// 초대 실패
	static ConstructorHelpers::FClassFinder<UFailedWG> failedWG(TEXT("/Game/UI/Common/WB_Failed.WB_Failed_C"));
	if (failedWG.Succeeded()) {
		WG_Failed_Class = failedWG.Class;
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
	friendInfos->RefreshFriendInfo();
	gameInstance = Cast<UNetworkGameInstance>(GetGameInstance());
	if (!gameInstance) MYLOG(Error, TEXT("Can't get gameInstance"));

	if (WG_Lobby == nullptr) {
		WG_Lobby = CreateWidget<ULobbyWG>(GetWorld(), WG_Lobby_Class);
		WG_Lobby->AddToViewport();
	}

	// 4개의 슬롯을 생성하고 초기화함
	for (int i = 0; i < MAX_PLAYER; ++i) {
		URoomPlayer* slot = NewObject<URoomPlayer>(URoomPlayer::StaticClass());
		slots[i] = slot;
		ChangeSlot(i, 0);
	}
	RefreshLobby();

	// 네트워크 프로세서가 없다면 스폰시킨다.
	TArray<AActor*> outActors;
	UGameplayStatics::GetAllActorsOfClass(GetWorld(), ALobbyNetworkProcessor::StaticClass(), outActors);
	if (outActors.Num() <= 0) {
		GetWorld()->SpawnActor(ALobbyNetworkProcessor::StaticClass());
	}

#ifdef DEBUG_STEAM
	// 서버는 켜져있지만, 스팀에 접속하지 않았다면,
	if (gameInstance->GetNetworkSystem()->OnServer() && !gameInstance->GetNetworkSystem()->OnSteam()) {
		// 가짜 스팀 접속을 시행한다.
		UINT64 randID = rand() % 100000;
		SetSteamID_DEBUG(FString::Printf(TEXT("%llu"), randID));
	}
#endif
}

int ALobbyManager::GetFriendLen()
{
	return friendInfos->FriendInformations.Num();
}

FString ALobbyManager::GetFriendName(const int& number)
{
	if (GetFriendLen() <= number) return "0";
	return friendInfos->FriendInformations[number]->name;
}

FString ALobbyManager::GetFriendSteamID(const int& number)
{
	return  UINT64ToFString(friendInfos->FriendInformations[number]->steamID);
}

UTexture2D* ALobbyManager::GetFriendImage(const int& number)
{
	if (GetFriendLen() <= number) return nullptr;
	return friendInfos->FriendInformations[number]->avatar;
}

void ALobbyManager::InviteFriend(FString steamID)
{
	// 서버에 요청한다.
	UINT64 uiSteamID = FStringToUINT64(steamID);
	char uIntBuf[10], outBuf[20];
	int uIntLen = CSerializer::UInt64Serializer(uIntBuf, uiSteamID);
	int allLen = CSerializer::SerializeWithEnum(C_Lobby_InviteFriend_Request, uIntBuf, uIntLen, outBuf);
	gameInstance->GetNetworkSystem()->Send(outBuf, allLen);
}

void ALobbyManager::AnswerInviteRequest(bool isYes, const FString& steamID)
{
	char buf_isAccept[5], buf_targetId[10], buf_all[15], outBuf[20];
	int boolLen = CSerializer::BoolSerialize(buf_isAccept, isYes);
	int intLen = CSerializer::UInt64Serializer(buf_targetId, FStringToUINT64(steamID));
	memcpy(buf_all, buf_isAccept, boolLen);
	memcpy(buf_all + boolLen, buf_targetId, intLen);
	int allLen = CSerializer::SerializeWithEnum(C_Lobby_InviteFriend_Answer, buf_all, boolLen + intLen, outBuf);
	gameInstance->GetNetworkSystem()->Send(outBuf, allLen);
}

UFriendInformation* ALobbyManager::GetFriendBySteamId(const UINT64& steamID)
{
	return friendInfos->GetFriendInformationBySteamID(steamID);
}

void ALobbyManager::ChangeSlot(int slotNumber, const UINT64 & steamID)
{
	// 퇴장한 슬롯은 비워준다.
	if (steamID == 0) {
		slots[slotNumber]->name = "";
		slots[slotNumber]->steamID = "0";
		slots[slotNumber]->image = nullptr;
		slots[slotNumber]->onReady = false;
	}
	else {
		slots[slotNumber]->steamID = UINT64ToFString(steamID);
		slots[slotNumber]->name = gameInstance->GetNetworkSystem()->GetSteamName(steamID);
		slots[slotNumber]->image = gameInstance->GetNetworkSystem()->GetSteamAvartar(steamID);
		slots[slotNumber]->onReady = false;
	}
}

void ALobbyManager::RefreshLobby()
{
	bool bAllReady = true;
	// 모든 슬롯을 갱신해준다.
	for (int i = 0; i < MAX_PLAYER; ++i) {
		WG_Lobby->SetPartySlot(i, slots[i]->name, slots[i]->image, slots[i]->steamID, slots[i]->onReady);
		if (slots[i]->onReady == false && slots[i]->steamID != "0") bAllReady = false;
	}
	WG_Lobby->OnMatching(bAllReady);
}

void ALobbyManager::SetReadyButton(const int& slot, const bool& isOn)
{
	if (slot >= MAX_PLAYER || slot < 0) return;

	// 나의 상태라면 버튼을 바꾼다.
	if(FStringToUINT64(slots[slot]->steamID) == gameInstance->GetNetworkSystem()->GetSteamID()) 
		WG_Lobby->OnReady(isOn);
	slots[slot]->onReady = isOn;
	RefreshLobby();
}

void ALobbyManager::OnClickSetting()
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

void ALobbyManager::OpenRequestInviteWG(const FString& steamID, const FString& userName)
{
	if (WG_RequestInvite == nullptr) WG_RequestInvite = CreateWidget<UWidgetFriendRequest>(GetWorld(), WG_RequestInvite_Class);
	WG_RequestInvite->AddToViewport();
	WG_RequestInvite->SetSteamID(steamID);
	WG_RequestInvite->SetUserName(userName);
}

void ALobbyManager::OpenLobbyFailedWG(const FString& msg)
{
	if (WG_Failed == nullptr) WG_Failed = CreateWidget<UFailedWG>(GetWorld(), WG_Failed_Class);
	WG_Failed->AddToViewport();
}

int32 ALobbyManager::GetMaxPlayer()
{
	return MAX_PLAYER;
}

void ALobbyManager::SetSteamID_DEBUG(const FString & steamID)
{
	gameInstance->GetNetworkSystem()->SetSteamID_DUBGE(FStringToUINT64(steamID));
}

void ALobbyManager::EnterToRoom_DEBUG()
{
	char buf[sizeof(EMessageType)];
	CSerializer::SerializeEnum(C_Debug_RoomStart, buf);
	gameInstance->GetNetworkSystem()->Send(buf, sizeof(EMessageType));
}

void ALobbyManager::ChangePartyKing(const int32& slot, const FString& steamID)
{
	int slotNumb = static_cast<int>(slot);
	UINT64 myId = gameInstance->GetNetworkSystem()->GetSteamID();
	UINT64 uintSteamID = FStringToUINT64(steamID);
	// 파티장일때만 처리
	if (slots[0]->steamID != "0" && myId != FStringToUINT64(slots[0]->steamID)) {
		OpenLobbyFailedWG("파티장만 파티를 양도 할 수 있습니다.");
	}
	// 자기 자신은 타깃이 될 수 없다.
	if (myId == uintSteamID) {
		OpenLobbyFailedWG("오이오이... 너는 이미 파티장이라구.");
	}

	char allBuf[sizeof(EMessageType) + sizeof(int)], intBuf[sizeof(int)];
	CSerializer::IntSerialize(intBuf, slotNumb);
	CSerializer::SerializeWithEnum(C_Lobby_Set_PartyKing, intBuf, sizeof(int), allBuf);
	gameInstance->GetNetworkSystem()->Send(allBuf, sizeof(EMessageType) + sizeof(int));
}

void ALobbyManager::OnClickFriend()
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
	if (slots[0] == nullptr) {
		OpenLobbyFailedWG("서버에 접속 할 수 없습니다.");
		return;
	}
	char allBuf[sizeof(bool) + sizeof(EMessageType)], boolBuf[sizeof(bool)];
	CSerializer::BoolSerialize(boolBuf, isOn);
	int allLen = CSerializer::SerializeWithEnum(C_Lobby_MatchRequest, boolBuf, sizeof(bool), allBuf);
	gameInstance->GetNetworkSystem()->Send(allBuf, allLen);
}

int32 ALobbyManager::PlayerSlotNumber() 
{
	FString myId = UINT64ToFString(gameInstance->GetNetworkSystem()->GetSteamID());
	for (int i = 0; i < MAX_PLAYER; ++i) {
		if (slots[i]->steamID == myId) {
			return i;
		}
	}
	return -1;
}

void ALobbyManager::Kick(const int32& slot, const FString& steamID)
{
	int targetSlot = static_cast<int>(slot);
	UINT64 myId = gameInstance->GetNetworkSystem()->GetSteamID();
	UINT64 uintSteamID = FStringToUINT64(steamID);
	// 자기 자신을 강퇴하려고 하거나, 파티장일때만 처리
	if (!(myId == uintSteamID || (slots[0]->steamID != "0" && myId == FStringToUINT64(slots[0]->steamID)))) {
		OpenLobbyFailedWG("파티장만 다른 파티원을 강퇴 할 수 있습니다.");
		return;
	}

	char allBuf[sizeof(EMessageType) + sizeof(int)], intBuf[sizeof(int)];
	CSerializer::IntSerialize(intBuf, targetSlot);
	CSerializer::SerializeWithEnum(C_Lobby_FriendKick_Request, intBuf, sizeof(int), allBuf);
	gameInstance->GetNetworkSystem()->Send(allBuf, sizeof(EMessageType) + sizeof(int));
}
