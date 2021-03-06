// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "NetworkTest.h"
#include "UObject/NoExportTypes.h"
#include "FriendInfo.generated.h"

// 친구에 대한 정보를 담는 클래스
UCLASS()
class NETWORKTEST_API UFriendInformation : public UObject
{
	GENERATED_BODY()
public:
	UFriendInformation();
	~UFriendInformation();
	uint64 steamID;
	UPROPERTY()
	UTexture2D* avatar;
	FString name;
};

class CSteamID;
/**
 * 친구 목록을 담는 UObject
 */
UCLASS()
class NETWORKTEST_API UFriendInfo : public UObject
{
	GENERATED_BODY()
public:
	UFriendInfo();
	~UFriendInfo();
	void RefreshFriendInfo();
	UPROPERTY()
	TArray<UFriendInformation*> FriendInformations;
	UFriendInformation* GetFriendInformationBySteamID(const UINT64& id);
	void InitArray();
};
