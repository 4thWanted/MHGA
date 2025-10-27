
#include "Props/WrapperBox.h"

#include "WrappingPaper.h"
#include "Engine/TargetPoint.h"
#include "Kismet/GameplayStatics.h"


AWrapperBox::AWrapperBox()
{
	PrimaryActorTick.bCanEverTick = true;

	Mesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Mesh"));
	Mesh->SetupAttachment(GetRootComponent());
}

void AWrapperBox::BeginPlay()
{
	Super::BeginPlay();

	WrapperPoint = Cast<ATargetPoint>(UGameplayStatics::GetActorOfClass(GetWorld(), ATargetPoint::StaticClass()));
	if (WrapperPoint == nullptr)
		UE_LOG(LogTemp, Error, TEXT("FUCK"))
}

void AWrapperBox::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

void AWrapperBox::SpawnWrapper()
{
	UE_LOG(LogTemp, Warning, TEXT("Spawn Wrapper"))
	GetWorld()->SpawnActor<AWrappingPaper>(WrapperClass, WrapperPoint->GetActorLocation(), FRotator(0));
}

