// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#define DEBUG_RECV
#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "CommonNetworkProcessor.generated.h"

struct FReciveData;
class UNetworkGameInstance;

UCLASS()
class NETWORKTEST_API ACommonNetworkProcessor : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ACommonNetworkProcessor();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;
	virtual void RecvProc(FReciveData& data);
	UNetworkGameInstance* GetGameInstance();

private:
	UNetworkGameInstance* gameInstance;
};
