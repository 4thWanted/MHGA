#include "Hamburger.h"

#include "BurgerData.h"
#include "Net/UnrealNetwork.h"

AHamburger::AHamburger()
{
	PrimaryActorTick.bCanEverTick = true;
	
	bReplicates = true;

	BurgerName.Empty();

	Mesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Mesh"));
	SetRootComponent(Mesh);
	Mesh->SetSimulatePhysics(true);
	Mesh->SetIsReplicated(true);
	SetReplicateMovement(true);
	Mesh->SetCollisionProfileName(TEXT("BlockAllDynamic"));

}

void AHamburger::BeginPlay()
{
	Super::BeginPlay();

	TObjectPtr<UStaticMeshComponent> pMesh = GetMeshComp();
	if (pMesh)
	{
		burgerMaterial = UMaterialInstanceDynamic::Create(pMesh->GetMaterial(0), this);
		pMesh->SetMaterial(0, burgerMaterial);
	}
}

void AHamburger::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (bShowLog)
		PrintLog();
}

void AHamburger::GetLifetimeReplicatedProps(TArray<class FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(AHamburger, BurgerName);
}

void AHamburger::SetLocation(FVector Loc)
{
	SetActorLocation(Loc);
}

void AHamburger::PrintLog()
{
	if (!BurgerName.IsEmpty())
	{
		FString Name = FString::Printf(TEXT("Burger Name : %s"), *BurgerName);
		DrawDebugString(GetWorld(), GetActorLocation(), Name, nullptr, FColor::Yellow, 0);
	}
	
}

void AHamburger::ServerSetName_Implementation(const FString& Name)
{
	BurgerName = Name;
}

void AHamburger::SetName(FString Name)
{
	if (HasAuthority())
		BurgerName = Name;
	else
		ServerSetName(Name);
}

void AHamburger::MultiRPC_SetMat_Implementation(const FString& Name)
{
	SetMat(Name);
}

void AHamburger::SetMat(const FString& Name)
{
	if (Name == "WrongBurger")
	{
		burgerMaterial->SetTextureParameterValue(TEXT("BurgerName"), wrongTexture);
	}
	else if (Name == "Shrimp")
	{
		burgerMaterial->SetTextureParameterValue(TEXT("BurgerName"), shrimpTexture);
	}
	else if (Name == "Shanghai")
	{
		burgerMaterial->SetTextureParameterValue(TEXT("BurgerName"), shanghaiTexture);
	}
	else if (Name == "BTD")
	{
		burgerMaterial->SetTextureParameterValue(TEXT("BurgerName"), btdTexture);
	}
	else if (Name == "QPC")
	{
		burgerMaterial->SetTextureParameterValue(TEXT("BurgerName"), qpcTexture);
	}
	else if (Name == "BigMac")
	{
		burgerMaterial->SetTextureParameterValue(TEXT("BurgerName"), bigMacTexture);
	}
}

FString AHamburger::GetBurgerName()
{
	return BurgerName;
}
