#include "Counter/CounterPOS.h"

#include "MHGA.h"
#include "Components/WidgetComponent.h"
#include "Counter/CounterUI.h"


ACounterPOS::ACounterPOS()
{
	PrimaryActorTick.bCanEverTick = true;

	MeshComponent = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("StaticMeshComponent"));
	SetRootComponent(MeshComponent);
	ConstructorHelpers::FObjectFinder<UStaticMesh> mesh(TEXT("/Engine/BasicShapes/Cube.Cube"));
	if (mesh.Succeeded())
		MeshComponent->SetStaticMesh(mesh.Object);
	
	WidgetComponent = CreateDefaultSubobject<UWidgetComponent>(TEXT("WidgetComponent"));
	WidgetComponent->SetupAttachment(MeshComponent);
	ConstructorHelpers::FClassFinder<UCounterUI> cu(TEXT("/Script/UMGEditor.WidgetBlueprint'/Game/UI/WBP_Pos.WBP_Pos_C'"));
	if (cu.Succeeded())
		WidgetComponent->SetWidgetClass(cu.Class);
	
	WidgetComponent->SetRelativeLocation(FVector(50.0f, 0.0f, 110.0f));
	WidgetComponent->SetRelativeScale3D(FVector(0.1f));
	WidgetComponent->SetDrawSize(FVector2D(1920, 1080));
}

void ACounterPOS::BeginPlay()
{
	Super::BeginPlay();

	CounterUI = Cast<UCounterUI>(WidgetComponent->GetWidget());
}

void ACounterPOS::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}
