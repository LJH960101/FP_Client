// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "NetworkTest.h"
#include "UObject/NoExportTypes.h"
#include "RoomPlayer.generated.h"

/**
 * 방 안의 플레이어에 대한 정보를 담는 클래스
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
	UPROPERTY()
	FString steamID;

	UPROPERTY()
	bool onReady;
};