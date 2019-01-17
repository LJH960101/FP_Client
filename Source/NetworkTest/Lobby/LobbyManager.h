// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "LobbyWG.h"
#include "Blueprint/UserWidget.h"
#include "RoomPlayer.h"
#include "LobbyManager.generated.h"

class UFriendInfo;
class UFriendInformation;
class UNetworkGameInstance;
class URoomPlayer;

UCLASS()
class NETWORKTEST_API ALobbyManager : public AActor
{
	GENERATED_BODY()
private:
	UPROPERTY()
	UFriendInfo* friendInfos;
	UNetworkGameInstance* gameInstance;
	UPROPERTY()
	URoomPlayer* slot1;
	UPROPERTY()
	URoomPlayer* slot2;
	UPROPERTY()
	URoomPlayer* slot3;
	UPROPERTY()
	URoomPlayer* slot4;


	// ½ÇÆÐ ¾È³» À§Á¬
	UPROPERTY()
		TSubclassOf<class ULobbyWG> WG_Lobby_Class;
	UPROPERTY()
		ULobbyWG* WG_Lobby;

	// ¼³Á¤ À§Á¬
	UPROPERTY()
	TSubclassOf<class UUserWidget> WG_Setting_Class;
	UPROPERTY()
	UUserWidget* WG_Setting;
	bool onSetting;

	// Ä£±¸ À§Á¬
	UPROPERTY()
	TSubclassOf<class UUserWidget> WG_Friend_Class;
	UPROPERTY()
	UUserWidget* WG_Friend;
	bool onFriend;

public:	
	// Sets default values for this actor's properties
	ALobbyManager();
	UFUNCTION(BlueprintCallable, Category = "Friend")
	void RefreshList();
	UFUNCTION(BlueprintCallable, Category = "Friend")
	int GetFriendLen();
	FString GetNameBySteamID(const UINT64& steamID);
	UFUNCTION(BlueprintCallable, Category = "Friend")
	FString GetFriendName(const int& number);
	UFUNCTION(BlueprintCallable, Category = "Friend")
	FString GetFriendSteamID(const int& number);
	UFUNCTION(BlueprintCallable, Category = "Friend")
	UTexture2D* GetFriendImage(const int& number);
	UFUNCTION(BlueprintCallable, Category = "InviteFriend")
	void InviteFriend(FString steamID);
	UFUNCTION(BlueprintCallable, Category = "InviteFriend")
	void AnswerInviteRequest(bool isYes, const FString& steamID);
	UFUNCTION(BlueprintCallable, Category = "Matching")
	void OnReady(bool isOn);
	UFUNCTION(BlueprintCallable, Category = "Kick")
	void Kick(int playerNumber);
	UFriendInformation* GetFriendBySteamId(const UINT64& steamID);
	void RefreshSlot(int slotNumber, const UINT64& steamID);
	void RefreshLobby();

	UFUNCTION(BlueprintCallable)
	void ClickSetting();

	UFUNCTION(BlueprintCallable)
	void ClickFriend();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

};
