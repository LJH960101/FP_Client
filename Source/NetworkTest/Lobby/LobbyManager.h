// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "LobbyManager.generated.h"

class UFriendInfo;
class UFriendInformation;
class UNetworkGameInstance;

UCLASS()
class NETWORKTEST_API ALobbyManager : public AActor
{
	GENERATED_BODY()
private:
	UPROPERTY()
	UFriendInfo* friendInfos;
	UNetworkGameInstance* gameInstance;

public:	
	// Sets default values for this actor's properties
	ALobbyManager();
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
	UFUNCTION(BlueprintCallable, Category = "InviteFriend")
	void InviteFriend(FString steamID);
	UFUNCTION(BlueprintCallable, Category = "InviteFriend")
	void AnswerInviteRequest(bool isYes);
	UFUNCTION(BlueprintCallable, Category = "Matching")
	void OnReady(bool isOn);
	UFUNCTION(BlueprintCallable, Category = "Kick")
	void Kick(int playerNumber);
	UFriendInformation* GetFriendBySteamId(const UINT64& steamID);

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

};
