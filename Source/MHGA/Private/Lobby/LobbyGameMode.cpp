// Fill out your copyright notice in the Description page of Project Settings.


#include "Lobby/LobbyGameMode.h"

#include "MHGA.h"
#include "MHGAGameInstance.h"
#include "OnlineSessionSettings.h"
#include "GameFramework/GameStateBase.h"
#include "GameFramework/GameSession.h"
#include "GameFramework/PlayerState.h"
#include "Kismet/GameplayStatics.h"
#include "Lobby/LobbyGameState.h"
#include "OnlineSubsystem.h"
#include "Interfaces/OnlineSessionInterface.h"
#include "GameFramework/PlayerController.h"
#include "Lobby/LobbyPlayerState.h"
#include "Player/MHGACharacter.h"

ALobbyGameMode::ALobbyGameMode()
{
	ConstructorHelpers::FClassFinder<AMHGACharacter> ch(TEXT("/Script/Engine.Blueprint'/Game/Blueprints/Player/BP_Player.BP_Player_C'"));
	if (ch.Succeeded())
	{
		DefaultPawnClass = ch.Class;
		characterList.Add(ch.Class);
	}
	ConstructorHelpers::FClassFinder<AMHGACharacter> ch2(TEXT("/Script/Engine.Blueprint'/Game/Blueprints/Player/BP_Player2.BP_Player2_C'"));
	if (ch2.Succeeded())
		characterList.Add(ch2.Class);
	ConstructorHelpers::FClassFinder<AMHGACharacter> ch3(TEXT("/Script/Engine.Blueprint'/Game/Blueprints/Player/BP_Player3.BP_Player3_C'"));
	if (ch3.Succeeded())
		characterList.Add(ch3.Class);
	ConstructorHelpers::FClassFinder<AMHGACharacter> ch4(TEXT("/Script/Engine.Blueprint'/Game/Blueprints/Player/BP_Player4.BP_Player4_C'"));
	if (ch4.Succeeded())
		characterList.Add(ch4.Class);
	
	
	bUseSeamlessTravel = true;
	GameSessionClass = AGameSession::StaticClass();
	PlayerStateClass = ALobbyPlayerState::StaticClass();
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
	if (ALobbyGameState* LGS = GetGameState<ALobbyGameState>())
	{
		if (ALobbyPlayerState* PS = NewPlayer ? Cast<ALobbyPlayerState>(NewPlayer->PlayerState) : nullptr)
		{
			LGS->Server_AddPlayerName(PS->GetPlayerName());
			PS->ClientRPC_MakeReadyUI();
		}
	}
	else
	{
		PRINTINFO();
	}

	if (HasAuthority())
		UpdatePlayerCount();
}

void ALobbyGameMode::Logout(AController* Exiting)
{
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
	
	if (HasAuthority())
		UpdatePlayerCount();
}

APawn* ALobbyGameMode::SpawnDefaultPawnAtTransform_Implementation(AController* NewPlayer,
	const FTransform& SpawnTransform)
{
	FActorSpawnParameters SpawnInfo;
	SpawnInfo.Instigator = GetInstigator();
	SpawnInfo.ObjectFlags |= RF_Transient;	// We never want to save default player pawns into a map
	UClass* PawnClass = GetDefaultPawnClassForController(NewPlayer);

	//플레이어가 선택한 캐릭터를 pawnclass에 설정
	FString playerName = NewPlayer->PlayerState->GetPlayerName();
	int32 charracterIdx = GetGameInstance<UMHGAGameInstance>()->GetSelectCharacter(playerName);
	if (charracterIdx != -1)
	{
		PawnClass = characterList[charracterIdx];
	}
	
	APawn* ResultPawn = GetWorld()->SpawnActor<APawn>(PawnClass, SpawnTransform, SpawnInfo);
	if (!ResultPawn)
	{
		UE_LOG(LogGameMode, Warning, TEXT("SpawnDefaultPawnAtTransform: Couldn't spawn Pawn of type %s at %s"), *GetNameSafe(PawnClass), *SpawnTransform.ToHumanReadableString());
	}
	return ResultPawn;
}

void ALobbyGameMode::UpdatePlayerCount()
{
	// 1. GameInstance 가져오기 및 Custom GameInstance로 캐스팅
	UMHGAGameInstance* GameInstance = Cast<UMHGAGameInstance>(GetGameInstance());
	if (!GameInstance) return;
    
	// 2. 현재 활성화된 세션 이름 가져오기 (가장 중요한 부분!)
	FName SessionToUpdate = GameInstance->GetCurrentSessionName();
	if (SessionToUpdate.IsNone()) 
	{
		UE_LOG(LogTemp, Warning, TEXT("세션 이름이 유효하지 않아 업데이트 불가."));
		return;
	}

	IOnlineSubsystem* OnlineSubsystem = IOnlineSubsystem::Get();
	IOnlineSessionPtr SessionInterface = OnlineSubsystem->GetSessionInterface();
	int32 NewPlayerCount = GetNumPlayers(); 
    
	FNamedOnlineSession* CurrentSession = SessionInterface->GetNamedSession(SessionToUpdate);
	FOnlineSessionSettings& NewSettings = CurrentSession->SessionSettings;

	if (NewSettings.Settings.Contains(FName("PlayerCount")))
	{
		NewSettings.Settings[FName("PlayerCount")].Data.SetValue(NewPlayerCount);
	}

	SessionInterface->UpdateSession(SessionToUpdate,NewSettings,true);
}
