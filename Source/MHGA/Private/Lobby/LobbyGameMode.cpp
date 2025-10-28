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

ALobbyGameMode::ALobbyGameMode()
{
	bUseSeamlessTravel = true;
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

	PRINTINFO();

	if (UMHGAGameInstance* GI = GetGameInstance<UMHGAGameInstance>())
	{
		if (IOnlineSubsystem* Subsystem = Online::GetSubsystem(GetWorld()))
		{
			if (IOnlineSessionPtr SessionInterface = Subsystem->GetSessionInterface(); SessionInterface.IsValid())
			{
				if (APlayerState* PS = NewPlayer ? NewPlayer->PlayerState : nullptr)
				{
					const FUniqueNetIdRepl& UniqueId = PS->GetUniqueId();
					if (UniqueId.IsValid())
					{
						if (const TSharedPtr<const FUniqueNetId> NetId = UniqueId.GetUniqueNetId())
						{
							if (!GI->GetCurrentSessionName().IsNone())
							{
								SessionInterface->RegisterPlayer(GI->GetCurrentSessionName(), *NetId, /*bWasInvited*/ false);
							}
						}
					}
				}
			}
		}
	}

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
	if (UMHGAGameInstance* GI = GetGameInstance<UMHGAGameInstance>())
	{
		if (IOnlineSubsystem* Subsystem = Online::GetSubsystem(GetWorld()))
		{
			if (IOnlineSessionPtr SessionInterface = Subsystem->GetSessionInterface(); SessionInterface.IsValid())
			{
				if (APlayerState* PS = Exiting ? Exiting->PlayerState : nullptr)
				{
					const FUniqueNetIdRepl& UniqueId = PS->GetUniqueId();
					if (UniqueId.IsValid())
					{
						if (const TSharedPtr<const FUniqueNetId> NetId = UniqueId.GetUniqueNetId())
						{
							if (!GI->GetCurrentSessionName().IsNone())
							{
								SessionInterface->UnregisterPlayer(GI->GetCurrentSessionName(), *NetId);
							}
						}
					}
				}
			}
		}
	}

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
