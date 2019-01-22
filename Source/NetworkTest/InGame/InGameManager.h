// 인게임에서 

#pragma once

#include "NetworkTest.h"
#include "GameFramework/Actor.h"
#include "InGameManager.generated.h"

UCLASS()
class NETWORKTEST_API AInGameManager : public AActor
{
	GENERATED_BODY()


public:	
	// Sets default values for this actor's properties
	AInGameManager();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

};
