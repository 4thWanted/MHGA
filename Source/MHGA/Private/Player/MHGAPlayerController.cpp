// Copyright Epic Games, Inc. All Rights Reserved.


#include "Player/MHGAPlayerController.h"

#include "Counter/CounterPOS.h"
#include "EnhancedInputSubsystems.h"
#include "Engine/LocalPlayer.h"
#include "GameFramework/GameStateBase.h"
#include "GameFramework/PlayerState.h"
#include "InputMappingContext.h"
#include "Lobby/LobbyBoard.h"
#include "Lobby/LobbyGameMode.h"
#include "Lobby/LobbyGameState.h"
#include "MHGACameraManager.h"
#include "MHGAGameState.h"
#include "Misc/PackageName.h"
#include "OnlineSubsystem.h"
#include "OnlineSubsystemUtils.h"
#include "TimerManager.h"

AMHGAPlayerController::AMHGAPlayerController()
{
	PlayerCameraManagerClass = AMHGACameraManager::StaticClass();

	ConstructorHelpers::FObjectFinder<UInputMappingContext> imc(TEXT("/Script/EnhancedInput.InputMappingContext'/Game/Input/IMC_Player.IMC_Player'"));
	if (imc.Succeeded())
		IMC = imc.Object;
}

void AMHGAPlayerController::BeginPlay()
{
	Super::BeginPlay();
	
	if (AMHGAGameState* gs = Cast<AMHGAGameState>(GetWorld()->GetGameState()))
		CounterPos = gs->GetCounter();	

	if (ALobbyGameState* lgs = GetWorld()->GetGameState<ALobbyGameState>())
		LobbyBoard = lgs->GetBoard();

	InitializeVoiceChatMonitoring();
}

void AMHGAPlayerController::SetupInputComponent()
{
	Super::SetupInputComponent();

	if (IsLocalPlayerController())
	{
		if (UEnhancedInputLocalPlayerSubsystem* Subsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(GetLocalPlayer()))
			Subsystem->AddMappingContext(IMC, 0);
	}
	
}

void AMHGAPlayerController::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	if (RemoteVoiceRefreshHandle.IsValid())
	{
		GetWorldTimerManager().ClearTimer(RemoteVoiceRefreshHandle);
		RemoteVoiceRefreshHandle.Invalidate();
	}

	ClearRemoteTalkers();

	Super::EndPlay(EndPlayReason);
}

void AMHGAPlayerController::ServerRPC_OnClickCustomerBtn_Implementation()
{
	CounterPos->MulticastRPC_OnClickCustomerBtn();
}

void AMHGAPlayerController::ServerRPC_OnClickMenuBtn_Implementation()
{
	CounterPos->MulticastRPC_OnClickMenuBtn();
}

void AMHGAPlayerController::ServerRPC_OrderMenuBtn_Implementation()
{
	CounterPos->MulticastRPC_OrderMenuBtn();
}

void AMHGAPlayerController::ServerRPC_DeleteListBtn_Implementation()
{
	CounterPos->MulticastRPC_DeleteListBtn();
}

void AMHGAPlayerController::ServerRPC_OnMenuReadyBtn_Implementation()
{
	CounterPos->MulticastRPC_OnMenuReadyBtn();
}

void AMHGAPlayerController::ServerRPC_AddMenuToList_Implementation(const EBurgerMenu MenuName)
{
	CounterPos->MulticastRPC_AddMenuToList(MenuName);
}

void AMHGAPlayerController::ServerRPC_CustomerOrderedMenu_Implementation(int32 CustomerNum)
{
	CounterPos->MulticastRPC_CustomerOrderedMenu(CustomerNum);
}


void AMHGAPlayerController::ServerRPC_Ready_Implementation(int32 PlayerNum)
{
	LobbyBoard->MulticastRPC_Ready(PlayerNum);
}

void AMHGAPlayerController::ServerRPC_Run_Implementation()
{
	LobbyBoard->MulticastRPC_Run();
}

void AMHGAPlayerController::InitializeVoiceChatMonitoring()
{
	if (!IsLocalController() || HasAuthority())
	{
		return;
	}

	if (!ShouldManageVoiceChat())
	{
		return;
	}

	RefreshRemoteTalkers();

	if (!RemoteVoiceRefreshHandle.IsValid())
	{
		GetWorldTimerManager().SetTimer(RemoteVoiceRefreshHandle, this, &AMHGAPlayerController::RefreshRemoteTalkers, 2.0f, true);
	}
}

bool AMHGAPlayerController::ShouldManageVoiceChat() const
{
	const UWorld* World = GetWorld();
	if (!World || !World->IsGameWorld())
	{
		return false;
	}

	const FString ShortName = FPackageName::GetShortName(World->GetMapName());
	return ShortName.Contains(TEXT("Lobby")) || ShortName.Contains(TEXT("Main"));
}

void AMHGAPlayerController::RefreshRemoteTalkers()
{
	if (!IsLocalController() || HasAuthority())
	{
		return;
	}

	if (!ShouldManageVoiceChat())
	{
		ClearRemoteTalkers();
		return;
	}

	UWorld* World = GetWorld();
	if (!World)
	{
		return;
	}

	AGameStateBase* GameState = World->GetGameState();
	if (!GameState)
	{
		return;
	}

	TSet<FString> SeenIds;

	APlayerState* LocalPlayerState = GetPlayerState<APlayerState>();

	for (APlayerState* OtherPlayerState : GameState->PlayerArray)
	{
		if (!OtherPlayerState || OtherPlayerState == LocalPlayerState)
		{
			continue;
		}

		const FUniqueNetIdRepl& UniqueId = OtherPlayerState->GetUniqueId();
		if (!UniqueId.IsValid())
		{
			continue;
		}

		FUniqueNetIdPtr NetId = UniqueId.GetUniqueNetId();
		if (!NetId.IsValid())
		{
			continue;
		}

		const FString IdKey = NetId->ToString();
		SeenIds.Add(IdKey);

		if (!RegisteredRemoteTalkers.Contains(IdKey))
		{
			RegisterRemoteTalker(OtherPlayerState);
		}
	}

	TArray<FString> KeysToRemove;
	for (const TPair<FString, FUniqueNetIdRepl>& Entry : RegisteredRemoteTalkers)
	{
		if (!SeenIds.Contains(Entry.Key))
		{
			KeysToRemove.Add(Entry.Key);
		}
	}

	for (const FString& Key : KeysToRemove)
	{
		UnregisterRemoteTalker(Key);
	}
}

void AMHGAPlayerController::RegisterRemoteTalker(APlayerState* TargetPlayerState)
{
	if (!TargetPlayerState)
	{
		return;
	}

	const FUniqueNetIdRepl& UniqueId = TargetPlayerState->GetUniqueId();
	if (!UniqueId.IsValid())
	{
		return;
	}

	IOnlineVoicePtr VoiceInterface = GetVoiceInterface();
	if (!VoiceInterface.IsValid())
	{
		return;
	}

	if (FUniqueNetIdPtr NetId = UniqueId.GetUniqueNetId())
	{
		const FString IdKey = NetId->ToString();
		VoiceInterface->RegisterRemoteTalker(*NetId);
		RegisteredRemoteTalkers.Add(IdKey, UniqueId);
	}
}

void AMHGAPlayerController::UnregisterRemoteTalker(const FString& PlayerIdKey)
{
	IOnlineVoicePtr VoiceInterface = GetVoiceInterface();

	if (FUniqueNetIdRepl* Entry = RegisteredRemoteTalkers.Find(PlayerIdKey))
	{
		if (VoiceInterface.IsValid() && Entry->IsValid())
		{
			if (FUniqueNetIdPtr NetId = Entry->GetUniqueNetId())
			{
				VoiceInterface->UnregisterRemoteTalker(*NetId);
			}
		}
	}

	RegisteredRemoteTalkers.Remove(PlayerIdKey);
}

void AMHGAPlayerController::ClearRemoteTalkers()
{
	TArray<FString> Keys;
	RegisteredRemoteTalkers.GenerateKeyArray(Keys);
	for (const FString& Key : Keys)
	{
		UnregisterRemoteTalker(Key);
	}
	RegisteredRemoteTalkers.Empty();
}

IOnlineVoicePtr AMHGAPlayerController::GetVoiceInterface() const
{
	const UWorld* World = GetWorld();
	if (!World)
	{
		return nullptr;
	}

	if (IOnlineSubsystem* Subsystem = Online::GetSubsystem(World))
	{
		return Subsystem->GetVoiceInterface();
	}

	return nullptr;
}
