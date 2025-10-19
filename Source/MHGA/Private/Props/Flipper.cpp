#include "Props/Flipper.h"

#include "MHGA.h"
#include "Camera/CameraComponent.h"
#include "Player/InteractComponent.h"
#include "Player/MHGACharacter.h"


AFlipper::AFlipper()
{
	PrimaryActorTick.bCanEverTick = false;

	Mesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Mesh"));
	SetRootComponent(Mesh);
	ConstructorHelpers::FObjectFinder<UStaticMesh> mesh(TEXT("/Script/Engine.StaticMesh'/Game/Asset/Mesh/Kitchen_Equipment/SM_Burger_Flipper/StaticMeshes/SM_Burger_Flipper.SM_Burger_Flipper'"));
	if (mesh.Succeeded())
		Mesh->SetStaticMesh(mesh.Object);
	Mesh->SetSimulatePhysics(true);
	Mesh->SetCollisionProfileName(TEXT("BlockAllDynamic"));

	Mesh->SetSimulatePhysics(true);
	Mesh->SetEnableGravity(true);

	bReplicates = true;
	AActor::SetReplicateMovement(true);
}

void AFlipper::BeginPlay()
{
	Super::BeginPlay();
	
}

void AFlipper::OnGrabbed(AMHGACharacter* Player)
{
	if (GrabCharacter != Player && GrabCharacter != nullptr)
	{
		PRINTINFO();
		GrabCharacter->GetInteractComponent()->ServerRPC_PutProps();
	}

	GrabCharacter = Player;

	SetActorRotation(Player->GetFirstPersonCameraComponent()->GetComponentRotation());
	PRINTLOG(TEXT("GRAB!"));
}


void AFlipper::OnPut()
{
	GrabCharacter = nullptr;
	PRINTLOG(TEXT("PUT!"));
}

void AFlipper::OnUse()
{
	FVector Start = GrabCharacter->GetFirstPersonCameraComponent()->GetComponentLocation();
	FVector End = Start + GrabCharacter->GetFirstPersonCameraComponent()->GetForwardVector() * 200;

	FCollisionShape Sphere = FCollisionShape::MakeSphere(20.f);
	FHitResult Hit;
	FCollisionQueryParams Params;
	Params.AddIgnoredActor(this);
	Params.AddIgnoredActor(GrabCharacter);

	if (GetWorld()->SweepSingleByChannel(Hit, Start, End, FQuat::Identity,
		ECollisionChannel::ECC_PhysicsBody, Sphere, Params))
	{
		if (Hit.GetComponent()->IsSimulatingPhysics())
		{
			IGrabableProps* GrabInterface = Cast<IGrabableProps>(Hit.GetActor());
			if (GrabInterface == nullptr)
				return;

			FRotator Rot = GrabCharacter->GetFirstPersonCameraComponent()->GetComponentRotation();
			Rot.Roll += 180.f;
			Hit.GetActor()->SetActorRotation(Rot);
		}
	}
	
	
	PRINTLOG(TEXT("USE!"));
}


