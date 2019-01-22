// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "WidgetFriendRequest.generated.h"

/**
 * ģ�� â �� ģ���� �ش��ϴ� Ŭ����
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
