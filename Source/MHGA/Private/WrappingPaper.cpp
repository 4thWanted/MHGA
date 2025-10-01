
#include "WrappingPaper.h"

#include "Components/BoxComponent.h"


AWrappingPaper::AWrappingPaper()
{
	PrimaryActorTick.bCanEverTick = true;

	DetectRange = 500.f;

	Collision = CreateDefaultSubobject<UBoxComponent>(TEXT("Collision"));
	Collision->SetupAttachment(RootComponent);
	Collision->SetCollisionEnabled(ECollisionEnabled::Type::QueryOnly);
}

void AWrappingPaper::BeginPlay()
{
	Super::BeginPlay();
	Collision->OnComponentBeginOverlap.AddDynamic(this, )
	
}

EBurgerMenu AWrappingPaper::CheckIngredients()
{
	// 내 일정 범위내에 존재하는 모든 재료액터를 탐색
	
	OnArea.Add()
	// BurgerData Table을 조회해서 일치하는 버거이름 탐색

	EBurgerMenu Result = EBurgerMenu::BigMac;
	
	// 해당 버거 이름을 EBurgerMenu로 return
	return Result;
}

void AWrappingPaper::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

