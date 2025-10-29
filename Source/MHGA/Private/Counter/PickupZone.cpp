// Fill out your copyright notice in the Description page of Project Settings.


#include "Counter/PickupZone.h"
#include "AI/CustomerManager.h" // 여기에 include 추가
#include "Components/BoxComponent.h"
#include "Kismet/GameplayStatics.h"

// Sets default values
APickupZone::APickupZone()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	
	boxCollision = CreateDefaultSubobject<UBoxComponent>(TEXT("BoxCollision"));
	SetRootComponent(boxCollision);
	boxCollision->SetBoxExtent(FVector(100.f, 100.f, 20.f));
	boxCollision->SetCollisionProfileName(TEXT("Trigger"));

	boxCollision->OnComponentBeginOverlap.AddDynamic(this, &APickupZone::OnOverlapBegin);
	boxCollision->OnComponentEndOverlap.AddDynamic(this, &APickupZone::OnOverlapEnd);
}

// Called when the game starts or when spawned
void APickupZone::BeginPlay()
{
	Super::BeginPlay();
	manager = Cast<ACustomerManager>(UGameplayStatics::GetActorOfClass(GetWorld(), ACustomerManager::StaticClass()));
	if (!IsValid(manager))
	{
		UE_LOG(LogTemp, Error, TEXT("PickupZone이 Manager를 찾을 수 없습니다!"));
	}
}

// Called every frame
void APickupZone::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

void APickupZone::OnOverlapBegin(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	// 겹친 액터가 햄버거인지 확인
	AHamburger* food = Cast<AHamburger>(OtherActor);
	
	if (IsValid(food))
	{
		curHamburgers.AddUnique(food);
		UE_LOG(LogTemp, Log, TEXT("음식이 픽업 존에 놓였습니다: %s (현재 총 %d개)"), *food->GetBurgerName(), curHamburgers.Num());
		//curHamburger = food;
	}
}

void APickupZone::OnOverlapEnd(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp,
	int32 OtherBodyIndex)
{
	AHamburger* food = Cast<AHamburger>(OtherActor);
	if (IsValid(food))
	{
		// 배열에서 나간 햄버거 제거
		curHamburgers.Remove(food);
		UE_LOG(LogTemp, Log, TEXT("음식이 픽업존에서 제거됨: %s (현재 총 %d개)"), *food->GetBurgerName(), curHamburgers.Num());
		
	}
	// if (OtherActor == curHamburger)
	// {
	// 	curHamburger = nullptr;
	// }
}

// AHamburger* APickupZone::TakeFood()
// {
// 	AHamburger* FoodToReturn = curHamburger;
// 	curHamburger = nullptr;
// 	return FoodToReturn;
// }
//
// bool APickupZone::HasFood() const
// {
// 	return IsValid(curHamburger);
// }

int32 APickupZone::GetFoodCountByType(const FString& menuName)
{
	int32 count = 0;
	for (AHamburger* food : curHamburgers)
	{
		if (IsValid(food) && food->GetBurgerName() == menuName)
		{
			count++;
		}
	}
	return count;
}

int32 APickupZone::GetTotalFoodCount() const
{
	return curHamburgers.Num();
}

void APickupZone::TakeFoodByType(const FString& menuName, int32 menuCount)
{
	if (!HasAuthority()) return;

	int32 countToRemove = menuCount;
	// 배열을 뒤에서부터 순회하면 제거해야 안전
	for (int32 i = curHamburgers.Num() - 1; i >= 0; i--)
	{
		AHamburger* food = curHamburgers[i];
		if (IsValid(food) && food->GetBurgerName() == menuName)
		{
			curHamburgers.RemoveAt(i);
			food->Destroy();
			countToRemove--;

			if (countToRemove <= 0) break;
		}
	}
}

void APickupZone::ClearAllFood()
{
	if (!HasAuthority()) return;

	for (AHamburger* food : curHamburgers)
	{
		if (IsValid(food))
		{
			food->Destroy();
		}
	}
	curHamburgers.Empty();
}
