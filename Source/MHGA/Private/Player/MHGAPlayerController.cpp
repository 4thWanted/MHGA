// Copyright Epic Games, Inc. All Rights Reserved.


#include "Player/MHGAPlayerController.h"
#include "EnhancedInputSubsystems.h"
#include "Engine/LocalPlayer.h"
#include "InputMappingContext.h"
#include "MHGACameraManager.h"

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
