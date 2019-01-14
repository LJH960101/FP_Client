// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "LobbyManager.generated.h"

class UFriendInfo;
UCLASS()
class NETWORKTEST_API ALobbyManager : public AActor
{
	GENERATED_BODY()
private:
	UPROPERTY()
	UFriendInfo* friendInfos;

public:	
	// Sets default values for this actor's properties
	ALobbyManager();
	UFUNCTION(BlueprintCallable, Category = "Friend")
	void RefreshList();
	UFUNCTION(BlueprintCallable, Category = "Friend")
	int GetFriendLen();
	UFUNCTION(BlueprintCallable, Category = "Friend")
	FString GetFriendName(int number);
	UFUNCTION(BlueprintCallable, Category = "Friend")
	UTexture2D* GetFriendImage(int number);

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

};
