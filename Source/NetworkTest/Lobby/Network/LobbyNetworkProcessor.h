// �κ񿡼� ��Ʈ��ũ �޼����� �޴� ó���� �ϴ� Ŭ����.

#pragma once

#include "NetworkTest.h"
#include "GameFramework/Actor.h"
#include "Common/Network/CommonNetworkProcessor.h"
#include "LobbyNetworkProcessor.generated.h"

class ALobbyManager;

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

private:

	ALobbyManager* LobbyManager;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;
};
