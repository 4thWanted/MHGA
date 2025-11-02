#include "Ingredient/LiquidSauce.h"

#include "MHGA.h"
#include "MHGAGameInstance.h"
#include "Components/StaticMeshComponent.h"
#include "Components/DecalComponent.h"
#include "NiagaraComponent.h"
#include "NiagaraFunctionLibrary.h"
#include "Player/MHGACharacter.h"

ALiquidSauce::ALiquidSauce()
{
	PrimaryActorTick.bCanEverTick = true;
	
	p_LiquidParticle = CreateDefaultSubobject<UNiagaraComponent>(TEXT("LiquidParticle"));
	Mesh->SetMassOverrideInKg(NAME_None, 0.5f, true);  // 가벼운 액체

	if (p_LiquidParticle && Mesh)
	{
		p_LiquidParticle->SetupAttachment(Mesh);
		p_LiquidParticle->bAutoActivate = false;
	}

	bReplicates = true;
	AActor::SetReplicateMovement(true);
}

void ALiquidSauce::BeginPlay()
{
	Super::BeginPlay();

	GI = GetGameInstance<UMHGAGameInstance>();

	if (Mesh)
	{
		Mesh->OnComponentHit.AddDynamic(this, &ALiquidSauce::OnLanded);
	}

	if (!InitialVelocity.IsNearlyZero() && Mesh && Mesh->IsSimulatingPhysics())
	{
		Mesh->SetPhysicsLinearVelocity(InitialVelocity);
	}

	PlayLiquidParticles();
}

void ALiquidSauce::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	// 바닥에 도착하지 않았을 때만 점성 적용
	if (!b_Landed && Mesh && Mesh->IsSimulatingPhysics())
	{
		ApplyViscosity(DeltaTime);
	}

	if (b_Landed)
	{
		f_TimeSinceLanded += DeltaTime;

		if (f_TimeSinceLanded > f_MaxLifeTime)
		{
			Destroy();
		}
	}
}

void ALiquidSauce::SetInitialVelocity(FVector Velocity)
{
	InitialVelocity = Velocity;
}

void ALiquidSauce::OnGrabbed(AMHGACharacter* Player)
{
	Super::OnGrabbed(Player);

	b_Grabbed = true;

	// 집혔을 때 물리 * 파티클 정지
	if (Mesh)
	{
		Mesh->SetPhysicsLinearVelocity(FVector::ZeroVector);
		Mesh->SetPhysicsAngularVelocityInDegrees(FVector::ZeroVector);
	}

	if (p_LiquidParticle)
	{
		p_LiquidParticle->Deactivate();
	}
}

void ALiquidSauce::OnPut()
{
	Super::OnPut();

	b_Grabbed = false;

	// 내려놓았을 때 물리 활성화
	if (Mesh && !b_Landed)
	{
		Mesh->SetSimulatePhysics(true);

		PlayLiquidParticles();
	}
}

void ALiquidSauce::OnLanded(UPrimitiveComponent* hitcomponent, AActor* otheractor, UPrimitiveComponent* othercomp, FVector normalimpulse, const FHitResult& hit)
{
	if (b_Landed)
	{
		return;  // 이미 착지
	}

	b_Landed = true;

	if (Mesh)
	{
		// 물리 정지
		Mesh->SetPhysicsLinearVelocity(FVector::ZeroVector);
		Mesh->SetPhysicsAngularVelocityInDegrees(FVector::ZeroVector);
		Mesh->SetSimulatePhysics(false);

		// UI 간섭 방지
		Mesh->SetCollisionResponseToChannel(ECC_WorldDynamic, ECR_Ignore);
	}

	if (p_LiquidParticle)
	{
		p_LiquidParticle->Deactivate();
	}

	CreateSauceDecal();
}

void ALiquidSauce::ApplyViscosity(float DeltaTime)
{
	if (!Mesh || !Mesh->IsSimulatingPhysics())
	{
		return;
	}

	FVector CurrentVelocity = Mesh->GetPhysicsLinearVelocity();

	// Viscosity = 0.92 → 매 프레임 92% 유지 → 천천히 감소
	FVector DampedVelocity = CurrentVelocity * f_Viscosity;

	Mesh->SetPhysicsLinearVelocity(DampedVelocity);

	// 회전도 감소
	FVector AngularVelocity = Mesh->GetPhysicsAngularVelocityInDegrees();
	Mesh->SetPhysicsAngularVelocityInDegrees(AngularVelocity * f_Viscosity);

	// 속도가 매우 느려지면 착지
	if (DampedVelocity.Length() < 5.0f && GetActorLocation().Z < 500.0f)
	{
		b_Landed = true;
		Mesh->SetPhysicsLinearVelocity(FVector::ZeroVector);
		Mesh->SetPhysicsAngularVelocityInDegrees(FVector::ZeroVector);
		Mesh->SetSimulatePhysics(false);
		Mesh->SetCollisionResponseToChannel(ECC_WorldDynamic, ECR_Ignore);

		if (p_LiquidParticle)
		{
			p_LiquidParticle->Deactivate();
		}

		CreateSauceDecal();
	}
}

void ALiquidSauce::PlayLiquidParticles()
{
	if (!p_LiquidParticle)
	{
		return;
	}

	if (p_LiquidParticle->GetAsset())
	{
		p_LiquidParticle->Activate(true);
	}
}

void ALiquidSauce::CreateSauceDecal()
{
	FVector DecalLocation = GetActorLocation() + FVector(0, 0, 1);

	if (p_SauceDecalMaterial && GetWorld())
	{
		FVector DecalSize = FVector(20, 20, 10);

		TObjectPtr<UDecalComponent> Decal = NewObject<UDecalComponent>(this);
		if (Decal)
		{
			Decal->RegisterComponent();
			Decal->SetWorldLocation(DecalLocation);
			Decal->SetDecalMaterial(p_SauceDecalMaterial);
			Decal->DecalSize = DecalSize;
			Decal->SetFadeOut(10.0f, f_MaxLifeTime - 5.0f);  // 5초 전부터 페이드아웃
		}
	}
}