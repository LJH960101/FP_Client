// Fill out your copyright notice in the Description page of Project Settings.

#pragma once
#include "NetworkModule/GameInfo.h"
#include "NetworkTest.h"
#include "GameFramework/Actor.h"
#include "LobbyWG.h"
#include "Blueprint/UserWidget.h"
#include "RoomPlayer.h"
#include "LobbyManager.generated.h"

class UFriendInfo;
class UFriendInformation;
class UNetworkGameInstance;
class URoomPlayer;
class ALobbyNetworkProcessor;

UCLASS()
class NETWORKTEST_API ALobbyManager : public AActor
{
	GENERATED_BODY()
private:
	UPROPERTY()
	UFriendInfo* friendInfos;

	UPROPERTY()
	UNetworkGameInstance* gameInstance;

	UPROPERTY()
	URoomPlayer* slots[MAX_PLAYER];

	// 실패 안내 위젯
	UPROPERTY()
	TSubclassOf<class ULobbyWG> WG_Lobby_Class;
	UPROPERTY()
	ULobbyWG* WG_Lobby;

	// 설정 위젯
	UPROPERTY()
	TSubclassOf<class UUserWidget> WG_Setting_Class;
	UPROPERTY()
	UUserWidget* WG_Setting;
	bool onSetting;

	// 친구 위젯
	UPROPERTY()
	TSubclassOf<class UUserWidget> WG_Friend_Class;
	UPROPERTY()
	UUserWidget* WG_Friend;
	bool onFriend;

	// 친구 수락창 위젯
	UPROPERTY()
	TSubclassOf<class UWidgetFriendRequest> WG_RequestInvite_Class;
	UPROPERTY()
	UWidgetFriendRequest* WG_RequestInvite;

	// 실패 안내 위젯
	UPROPERTY()
	TSubclassOf<class UFailedWG> WG_Failed_Class;
	UPROPERTY()
	UUserWidget* WG_Failed;

public:	
	// Sets default values for this actor's properties
	ALobbyManager();

	// 서버와의 통신 함수 ******
	UFUNCTION(BlueprintCallable, Category = "InviteFriend")
	void InviteFriend(FString steamID);
	UFUNCTION(BlueprintCallable, Category = "InviteFriend")
	void AnswerInviteRequest(bool isYes, const FString& steamID);
	UFUNCTION(BlueprintCallable, Category = "Matching")
	void OnReady(bool isOn);
	// 해당 파티원을 강퇴한다. (방장일때만 효력이 있음.)
	UFUNCTION(BlueprintCallable, Category = "Kick")
	void Kick(const int32& slot, const FString& steamID);
	// 파티창의 해당 파티원에게 파티장을 양도 요청 한다.
	UFUNCTION(BlueprintCallable, Category = "Party")
	void ChangePartyKing(const int32& slot, const FString& steamID);
	// 강제로 스팀 ID를 등록한다.
	UFUNCTION(BlueprintCallable, Category = "Debug")
	void SetSteamID_DEBUG(const FString& steamID);
	// 강제로 방에 접속하도록 요청한다.
	UFUNCTION(BlueprintCallable, Category = "Debug")
	void EnterToRoom_DEBUG();

	// 친구관련 내용을 랩핑한 함수들
	UFUNCTION(BlueprintCallable, Category = "Friend")
	void RefreshList();
	UFUNCTION(BlueprintCallable, Category = "Friend")
	int GetFriendLen();
	UFUNCTION(BlueprintCallable, Category = "Friend")
	FString GetFriendName(const int& number);
	UFUNCTION(BlueprintCallable, Category = "Friend")
	FString GetFriendSteamID(const int& number);
	UFUNCTION(BlueprintCallable, Category = "Friend")
	UTexture2D* GetFriendImage(const int& number);
	UFriendInformation* GetFriendBySteamId(const UINT64& steamID);

	// 파티창의 해당슬롯에 유저를 기입한다.
	void ChangeSlot(int slotNumber, const UINT64& steamID);
	// 로비 화면을 슬롯에 맞게 다시 그린다.
	void RefreshLobby();

	// Ready 버튼 상태를 바꾼다.
	void SetReadyButton(const int& slot, const bool& isOn);

	// Get Local Player Slot.
	UFUNCTION(BlueprintPure)
	int32 PlayerSlotNumber();

	// Setting 창을 클릭했을때의 로직
	UFUNCTION(BlueprintCallable)
	void OnClickSetting();

	// Friend 창을 클릭했을때의 로직
	UFUNCTION(BlueprintCallable)
	void OnClickFriend();

	void OpenRequestInviteWG(const FString& steamID, const FString& userName);
	void OpenLobbyFailedWG(const FString& msg = "알 수 없는 오류가 발생하였습니다.");

	UFUNCTION(BlueprintCallable)
		int32 GetMaxPlayer();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

};
