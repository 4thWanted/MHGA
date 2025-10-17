// Fill out your copyright notice in the Description page of Project Settings.


#include "AI/CustomerManager.h"

#include "AI/CustomerAI.h"

// Sets default values
ACustomerManager::ACustomerManager()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

}

// Called when the game starts or when spawned
void ACustomerManager::BeginPlay()
{
	Super::BeginPlay();
	// 스폰 시간 랜덤으로 지정
	float spawnTime = FMath::RandRange(minTime, maxTime);
	// 스폰 타이머 설정
	GetWorld()->GetTimerManager().SetTimer(spawnTimer, this, &ACustomerManager::SpawnCustomer, spawnTime);
	
}

// Called every frame
void ACustomerManager::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void ACustomerManager::SpawnCustomer()
{
	auto transform = spawnPoint->GetActorTransform();
	// 손님 스폰
	GetWorld()->SpawnActor<ACustomerAI>(spawnFactory, transform.GetLocation(), transform.Rotator());

	// 스폰 시간 랜덤으로 다시 지정
	float spawnTime = FMath::RandRange(minTime, maxTime);
	// 스폰 타이머 재설정
	GetWorld()->GetTimerManager().SetTimer(spawnTimer, this, &ACustomerManager::SpawnCustomer, spawnTime);
}

