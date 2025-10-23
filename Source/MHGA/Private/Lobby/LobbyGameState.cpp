// Fill out your copyright notice in the Description page of Project Settings.


#include "Lobby/LobbyGameState.h"

#include "Net/UnrealNetwork.h"

ALobbyGameState::ALobbyGameState()
{
	bReplicates = true;
}

void ALobbyGameState::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(ALobbyGameState, PlayerNames);
}

void ALobbyGameState::OnRep_PlayerNames()
{
	OnPlayerNamesChanged.Broadcast();
}

void ALobbyGameState::Server_AddPlayerName(const FString& Name)
{
	if (!HasAuthority()) return;
	
	PlayerNames.Add(Name);
	OnPlayerNamesChanged.Broadcast();
}

void ALobbyGameState::Server_RemovePlayerName(const FString& Name)
{
	if (!HasAuthority()) return;
	if (PlayerNames.Remove(Name) > 0)
		OnPlayerNamesChanged.Broadcast();
}
