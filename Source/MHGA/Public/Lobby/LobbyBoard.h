#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "LobbyBoard.generated.h"

class ALobbyGameState;
class ULobbyUI;
class UStaticMeshComponent;
class UWidgetComponent;

UCLASS()
class MHGA_API ALobbyBoard : public AActor
{
	GENERATED_BODY()

public:
	ALobbyBoard();

protected:
	virtual void BeginPlay() override;

protected:
	UPROPERTY(EditAnywhere)
	UStaticMeshComponent* MeshComponent;
	UPROPERTY(EditAnywhere)
	UWidgetComponent* WidgetComponent;

	UPROPERTY(EditAnywhere)
	ULobbyUI* LobbyUI;

	UPROPERTY(EditAnywhere)
	ALobbyGameState* LGS;
	
protected:
	UFUNCTION()
	void OnPlayerNameChange();

public:
	UFUNCTION(NetMulticast, Reliable)
	void MulticastRPC_Ready(int32 PlayerNum);
	UFUNCTION(NetMulticast, Reliable)
	void MulticastRPC_Run();
};
