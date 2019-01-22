// Fill out your copyright notice in the Description page of Project Settings.

#include "InGameManager.h"
#include "Network/InGameNetworkProcessor.h"

// Sets default values
AInGameManager::AInGameManager()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

}

// Called when the game starts or when spawned
void AInGameManager::BeginPlay()
{
	Super::BeginPlay();
	
	// 네트워크 프로세서가 없으면 만들어줌.
	TArray<AActor*> outActors;
	UGameplayStatics::GetAllActorsOfClass(GetWorld(), AInGameNetworkProcessor::StaticClass(), outActors);
	if (outActors.Num() <= 0) {
		GetWorld()->SpawnActor(AInGameNetworkProcessor::StaticClass());
	}
}

// Called every frame
void AInGameManager::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

