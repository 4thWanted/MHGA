// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameOverWidget.h"
#include "GameFramework/PlayerController.h"
#include "Interfaces/VoiceInterface.h"
#include "MHGAPlayerController.generated.h"

class ALobbyBoard;
class ACounterPOS;
class UInputMappingContext;
struct FUniqueNetIdRepl;
class APlayerState;

UCLASS()
class MHGA_API AMHGAPlayerController : public APlayerController
{
	GENERATED_BODY()
	
public:
	AMHGAPlayerController();

protected:
	UPROPERTY(EditAnywhere, Category="Input|Input Mappings")
	UInputMappingContext* IMC;

	/** Gameplay initialization */
	virtual void BeginPlay() override;

	/** Input mapping context setup */
	virtual void SetupInputComponent() override;
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;

	
	UPROPERTY(EditAnywhere)
	ACounterPOS* CounterPos;

	//Lobby
	UPROPERTY(EditAnywhere)
	ALobbyBoard* LobbyBoard;

public:
	//In Game CounterPos
	UFUNCTION(Server, Reliable)
	void ServerRPC_OnClickCustomerBtn();
	UFUNCTION(Server, Reliable)
	void ServerRPC_OnClickMenuBtn();
	UFUNCTION(Server, Reliable)
	void ServerRPC_OrderMenuBtn();
	UFUNCTION(Server, Reliable)
	void ServerRPC_DeleteListBtn();
	UFUNCTION(Server, Reliable)
	void ServerRPC_OnMenuReadyBtn();
	UFUNCTION(Server, Reliable)
	void ServerRPC_AddMenuToList(const EBurgerMenu MenuName);
	UFUNCTION(Server, Reliable)
	void ServerRPC_CustomerOrderedMenu(int32 CustomerNum);

	//Lobby
	UFUNCTION(Server, Reliable)
	void ServerRPC_Ready(int32 PlayerNum);
	UFUNCTION(Server, Reliable)
	void ServerRPC_Run();

private:
	void InitializeVoiceChatMonitoring();
	bool ShouldManageVoiceChat() const;
	void RefreshRemoteTalkers();
	void RegisterRemoteTalker(APlayerState* TargetPlayerState);
	void UnregisterRemoteTalker(const FString& PlayerIdKey);
	void ClearRemoteTalkers();
	IOnlineVoicePtr GetVoiceInterface() const;

	TMap<FString, FUniqueNetIdRepl> RegisteredRemoteTalkers;
	FTimerHandle RemoteVoiceRefreshHandle;
	
public:
	// GameOver
	void Client_HandleGameOver();
};
