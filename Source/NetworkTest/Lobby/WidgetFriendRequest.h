// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "WidgetFriendRequest.generated.h"

/**
 * 친구 창 속 친구에 해당하는 클래스
 */
UCLASS()
class NETWORKTEST_API UWidgetFriendRequest : public UUserWidget
{
	GENERATED_BODY()
public:
	UFUNCTION(BlueprintImplementableEvent, Category = "Steam")
	void SetUserName(const FString& name);
	void SetSteamID(FString steamID);
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Steam")
	FString _steamID;
};
