// Fill out your copyright notice in the Description page of Project Settings.


#include "Lobby/LobbyGameMode.h"

#include "MHGA.h"
#include "MHGAGameInstance.h"
#include "GameFramework/GameStateBase.h"
#include "GameFramework/PlayerState.h"
#include "Kismet/GameplayStatics.h"
#include "Lobby/LobbyBoard.h"
#include "Lobby/LobbyGameState.h"
#include "OnlineSubsystem.h"
#include "OnlineSubsystemUtils.h"
#include "Interfaces/VoiceInterface.h"

namespace
{
	void UpdateRemoteVoiceRegistration_Lobby(AController* Controller, bool bRegister)
	{
		if (!Controller)
		{
			return;
		}

		UWorld* World = Controller->GetWorld();
		if (!World)
		{
			return;
		}

		IOnlineSubsystem* Subsystem = Online::GetSubsystem(World);
		if (!Subsystem)
		{
			return;
		}

		IOnlineVoicePtr VoiceInterface = Subsystem->GetVoiceInterface();
		if (!VoiceInterface.IsValid())
		{
			return;
		}

		APlayerState* PlayerState = Controller->PlayerState;
		if (!PlayerState)
		{
			return;
		}

		const FUniqueNetIdRepl& UniqueId = PlayerState->GetUniqueId();
		if (!UniqueId.IsValid())
		{
			return;
		}

		if (bRegister)
		{
			VoiceInterface->RegisterRemoteTalker(*UniqueId.GetUniqueNetId());
		}
		else
		{
			VoiceInterface->UnregisterRemoteTalker(*UniqueId.GetUniqueNetId());
		}
	}
}

FString ALobbyGameMode::InitNewPlayer(APlayerController* NewPlayerController, const FUniqueNetIdRepl& UniqueId,
	const FString& Options, const FString& Portal)
{
	FString Result = Super::InitNewPlayer(NewPlayerController, UniqueId, Options, Portal);

	FString Nick = UGameplayStatics::ParseOption(Options, TEXT("Nick"));
	if (NewPlayerController && NewPlayerController->PlayerState && !Nick.IsEmpty())
	{
		NewPlayerController->PlayerState->SetPlayerName(Nick);
	}
	return Result;
}

void ALobbyGameMode::PostLogin(APlayerController* NewPlayer)
{
	Super::PostLogin(NewPlayer);
	UpdateRemoteVoiceRegistration_Lobby(NewPlayer, true);

	PRINTINFO();

	if (ALobbyGameState* LGS = GetGameState<ALobbyGameState>())
	{
		if (APlayerState* PS = NewPlayer ? NewPlayer->PlayerState : nullptr)
			LGS->Server_AddPlayerName(PS->GetPlayerName());
	}
	else
	{
		PRINTINFO();
	}
}

void ALobbyGameMode::Logout(AController* Exiting)
{
	UpdateRemoteVoiceRegistration_Lobby(Exiting, false);

	if (ALobbyGameState* LGS = GetGameState<ALobbyGameState>())
	{
		if (APlayerState* PS = Exiting ? Exiting->PlayerState : nullptr)
			LGS->Server_RemovePlayerName(PS->GetPlayerName());
	}
	else
	{
		PRINTINFO();
	}

	Super::Logout(Exiting);
}
