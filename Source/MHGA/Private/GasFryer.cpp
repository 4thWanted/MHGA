// Fill out your copyright notice in the Description page of Project Settings.


#include "GasFryer.h"

// Sets default values
AGasFryer::AGasFryer()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	fryer = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Fryer"));
	SetRootComponent(fryer);
	basketL = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("basket1"));
	basketL->SetupAttachment(fryer);
	basketR = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("basket2"));
	basketR->SetupAttachment(fryer);
}

// Called when the game starts or when spawned
void AGasFryer::BeginPlay()
{
	Super::BeginPlay();

	upPosL = basketL->GetRelativeLocation();
	upPosR = basketR->GetRelativeLocation();

	SetActorTickEnabled(false);
}

// Called every frame
void AGasFryer::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	float curTime = GetWorld()->GetTimeSeconds();
	float elapsedTime = curTime - movementStartTime;

	float a = FMath::Clamp(elapsedTime / moveDuration, 0, 1);

	if (bIsMovingDonw)
	{
		FVector newLocL = FMath::Lerp(upPosL, downPosL, a);
		FVector newLocR = FMath::Lerp(upPosR, downPosR, a);
		basketL->SetRelativeLocation(newLocL);
		basketR->SetRelativeLocation(newLocR);
		
		if (FMath::IsNearlyEqual(a, 1))
		{
			bIsMovingDonw = false;
			SetActorTickEnabled(false);

			GetWorldTimerManager().SetTimer(cookTimer, this, &AGasFryer::OnCookingFinished, cookTime, false);
		}
	}
	if (bIsMovingUp)
	{
		FVector newLocL = FMath::Lerp(downPosL, upPosL, a);
		FVector newLocR = FMath::Lerp(downPosR, upPosR, a);
		basketL->SetRelativeLocation(newLocL);
		basketR->SetRelativeLocation(newLocR);
		
		if (FMath::IsNearlyEqual(a, 1))
		{
			bIsMovingUp = false;
			SetActorTickEnabled(false);
		}
	}
}

void AGasFryer::StartCooking()
{
	if (bIsMovingDonw || bIsMovingUp || GetWorld()->GetTimerManager().IsTimerActive(cookTimer))
	{
		return;
	}

	bIsMovingDonw = true;
	bIsMovingUp = false;
	movementStartTime = GetWorld()->GetTimeSeconds();

	SetActorTickEnabled(true);
}

void AGasFryer::OnCookingFinished()
{
	bIsMovingDonw = false;
	bIsMovingUp = true;
	movementStartTime = GetWorld()->GetTimeSeconds();

	SetActorTickEnabled(true);
}



