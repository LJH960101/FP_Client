// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "NetworkTest.h"
#include "UObject/NoExportTypes.h"
#include "RoomPlayer.generated.h"

/**
 * 
 */

UCLASS()
class NETWORKTEST_API URoomPlayer : public UObject {
	GENERATED_BODY()
public:
	URoomPlayer() {}
	~URoomPlayer() {}
	UPROPERTY()
		FString name;
	UPROPERTY()
		UTexture2D* image;
	UINT64 steamID;
};