// �ΰ��ӿ��� ��Ʈ��ũ �޼����� �޾� ó���ϴ� Ŭ����.

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
	// ��Ʈ��ũ �ۼ��� ��û �Լ�
#ifdef DEBUG_STEAM
	// ������ ���� �뿡 ������ ��û�Ѵ�.
	void EnterToGame_DEBUG();
	// ������ ���� ID�� ����Ѵ�.
	void SetSteamID_DEBUG(const FString& steamID);
#endif
};
