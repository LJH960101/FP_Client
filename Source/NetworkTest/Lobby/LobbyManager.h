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

	// ���� �ȳ� ����
	UPROPERTY()
	TSubclassOf<class ULobbyWG> WG_Lobby_Class;
	UPROPERTY()
	ULobbyWG* WG_Lobby;

	// ���� ����
	UPROPERTY()
	TSubclassOf<class UUserWidget> WG_Setting_Class;
	UPROPERTY()
	UUserWidget* WG_Setting;
	bool onSetting;

	// ģ�� ����
	UPROPERTY()
	TSubclassOf<class UUserWidget> WG_Friend_Class;
	UPROPERTY()
	UUserWidget* WG_Friend;
	bool onFriend;

	// ģ�� ����â ����
	UPROPERTY()
	TSubclassOf<class UWidgetFriendRequest> WG_RequestInvite_Class;
	UPROPERTY()
	UWidgetFriendRequest* WG_RequestInvite;

	// ���� �ȳ� ����
	UPROPERTY()
	TSubclassOf<class UFailedWG> WG_Failed_Class;
	UPROPERTY()
	UUserWidget* WG_Failed;

public:	
	// Sets default values for this actor's properties
	ALobbyManager();

	// �������� ��� �Լ� ******
	UFUNCTION(BlueprintCallable, Category = "InviteFriend")
	void InviteFriend(FString steamID);
	UFUNCTION(BlueprintCallable, Category = "InviteFriend")
	void AnswerInviteRequest(bool isYes, const FString& steamID);
	UFUNCTION(BlueprintCallable, Category = "Matching")
	void OnReady(bool isOn);
	// �ش� ��Ƽ���� �����Ѵ�. (�����϶��� ȿ���� ����.)
	UFUNCTION(BlueprintCallable, Category = "Kick")
	void Kick(const int32& slot, const FString& steamID);
	// ��Ƽâ�� �ش� ��Ƽ������ ��Ƽ���� �絵 ��û �Ѵ�.
	UFUNCTION(BlueprintCallable, Category = "Party")
	void ChangePartyKing(const int32& slot, const FString& steamID);
	// ������ ���� ID�� ����Ѵ�.
	UFUNCTION(BlueprintCallable, Category = "Debug")
	void SetSteamID_DEBUG(const FString& steamID);
	// ������ �濡 �����ϵ��� ��û�Ѵ�.
	UFUNCTION(BlueprintCallable, Category = "Debug")
	void EnterToRoom_DEBUG();

	// ģ������ ������ ������ �Լ���
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

	// ��Ƽâ�� �ش罽�Կ� ������ �����Ѵ�.
	void ChangeSlot(int slotNumber, const UINT64& steamID);
	// �κ� ȭ���� ���Կ� �°� �ٽ� �׸���.
	void RefreshLobby();

	// Ready ��ư ���¸� �ٲ۴�.
	void SetReadyButton(const int& slot, const bool& isOn);

	// Get Local Player Slot.
	UFUNCTION(BlueprintPure)
	int32 PlayerSlotNumber();

	// Setting â�� Ŭ���������� ����
	UFUNCTION(BlueprintCallable)
	void OnClickSetting();

	// Friend â�� Ŭ���������� ����
	UFUNCTION(BlueprintCallable)
	void OnClickFriend();

	void OpenRequestInviteWG(const FString& steamID, const FString& userName);
	void OpenLobbyFailedWG(const FString& msg = "�� �� ���� ������ �߻��Ͽ����ϴ�.");

	UFUNCTION(BlueprintCallable)
		int32 GetMaxPlayer();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

};
