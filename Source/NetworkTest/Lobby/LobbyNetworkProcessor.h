// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "../Common/CommonNetworkProcessor.h"
#include "LobbyNetworkProcessor.generated.h"

UCLASS()
class NETWORKTEST_API ALobbyNetworkProcessor : public ACommonNetworkProcessor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ALobbyNetworkProcessor();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;
	virtual void RecvProc(FReciveData& data) override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;
};
