// 인게임에서 네트워크 메세지를 받아 처리하는 클래스.

#pragma once

#include "NetworkModule/GameInfo.h"
#include "NetworkTest.h"
#include "Common/Network/CommonNetworkProcessor.h"
#include "InGameNetworkProcessor.generated.h"

/**
 * 
 */
UCLASS()
class NETWORKTEST_API AInGameNetworkProcessor : public ACommonNetworkProcessor
{
	GENERATED_BODY()

private:
	class AInGameManager* inGameManager;
	class UNetworkSystem* networkSystem;
	class UNetworkGameInstance* gameInstance;

public:
	// Sets default values for this actor's properties
	AInGameNetworkProcessor();
	// Called every frame
	virtual void Tick(float DeltaTime) override;

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;
	virtual void RecvProc(FReciveData& data) override;

private:
	// 네트워크 송수신 요청 함수
#ifdef DEBUG_STEAM
	// 강제로 게임 룸에 들어가도록 요청한다.
	void EnterToGame_DEBUG();
	// 강제로 스팀 ID를 등록한다.
	void SetSteamID_DEBUG(const FString& steamID);
#endif
};
