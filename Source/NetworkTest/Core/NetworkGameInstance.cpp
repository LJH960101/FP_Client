// Fill out your copyright notice in the Description page of Project Settings.

#include "NetworkGameInstance.h"
#include "Network/NetworkSystem.h"

UNetworkGameInstance::UNetworkGameInstance() {
	_networkSystem = CreateDefaultSubobject<UNetworkSystem>(TEXT("NetworkSystem"));
}
UNetworkGameInstance::~UNetworkGameInstance() {}
UNetworkSystem* UNetworkGameInstance::GetNetworkSystem(){
	return _networkSystem;
}

void UNetworkGameInstance::Init() {
	Super::Init();
	if (_networkSystem) _networkSystem->Init();
	else MYLOG(Error, TEXT("Can't create NetworkSystem"));
}

void UNetworkGameInstance::Shutdown()
{
	Super::Shutdown();
	if (_networkSystem) _networkSystem->Shutdown();
}