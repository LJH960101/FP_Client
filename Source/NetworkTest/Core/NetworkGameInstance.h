// Fill out your copyright notice in the Description page of Project Settings.

#pragma once
#include "NetworkTest.h"
#include "Engine/GameInstance.h"
#include "NetworkGameInstance.generated.h"

class UNetworkSystem;

UCLASS()
class NETWORKTEST_API UNetworkGameInstance : public UGameInstance
{
	GENERATED_BODY()
public:
	UNetworkGameInstance();
	~UNetworkGameInstance();
	UFUNCTION(BlueprintCallable)
	UNetworkSystem* GetNetworkSystem();

private:
	virtual void Init();
	virtual void Shutdown();
	UPROPERTY()
	UNetworkSystem* _networkSystem;
};