
#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameStateBase.h"
#include "LobbyGameState.generated.h"

class ALobbyBoard;

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnPlayerNamesChanged);

UCLASS()
class MHGA_API ALobbyGameState : public AGameStateBase
{
	GENERATED_BODY()
public:
	ALobbyGameState();

protected:
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

private:
	UPROPERTY(ReplicatedUsing=OnRep_PlayerNames)
	TArray<FString> PlayerNames;

	UPROPERTY()
	ALobbyBoard* LobbyBoard;
	
public:
	// 클라 바인딩용 델리게이트
	FOnPlayerNamesChanged OnPlayerNamesChanged;

protected:
	UFUNCTION()
	void OnRep_PlayerNames();

public:
	void Server_AddPlayerName(const FString& Name);
	void Server_RemovePlayerName(const FString& Name);

	UFUNCTION()
	TArray<FString>& GetPlayerNames() { return PlayerNames; }

	void SetBoard(ALobbyBoard* lb) {LobbyBoard = lb;}
	ALobbyBoard* GetBoard() { return LobbyBoard; }
};