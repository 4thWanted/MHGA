#include "Ingredient/Patty.h"
#include "Net/UnrealNetwork.h"


APatty::APatty()
{
	PrimaryActorTick.bCanEverTick = false;
	bReplicates = true;
	// 종합 상태 초기화
	IngType = EIngredient::RawPatty;
	CookState = EPattyCookState::Raw;

	// 각 면 상태 초기화
	bIsFrontCooked = false;
	bIsBackCooked = false;
	bIsFrontOverCooked = false;
	bIsBackOverCooked = false;

	// 기타 변수 초기화
	bIsFrontSideDown = true; // 기본값 : 앞면이 아래
	bIsCooking = false;
}

void APatty::BeginPlay()
{
	Super::BeginPlay();
}

void APatty::GetLifetimeReplicatedProps(TArray<class FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(APatty, CookState);
	DOREPLIFETIME(APatty, bIsFrontCooked);
	DOREPLIFETIME(APatty, bIsBackCooked);
	DOREPLIFETIME(APatty, bIsFrontOverCooked);
	DOREPLIFETIME(APatty, bIsBackOverCooked);
	DOREPLIFETIME(APatty, bIsFrontSideDown);
	DOREPLIFETIME(APatty, bIsCooking);
}

EPattyCookState APatty::GetCookState() const
{
	return CookState;
}

void APatty::StartCook()
{
	
}

void APatty::ShutdownCook()
{
}

void APatty::OnRep_CookStateChanged()
{
}


void APatty::StartCooking()
{
}

void APatty::StopCooking()
{
}

void APatty::Flip()
{
}

void APatty::Server_StartCooking_Implementation()
{
}

void APatty::Server_StopCooking_Implementation()
{
}

void APatty::Server_Flip_Implementation()
{
}

void APatty::StartCookTimer()
{
}

void APatty::StopAllTimer()
{
}

void APatty::CookCurrentSide()
{
}

void APatty::OverCookCurrentSide()
{
}

void APatty::UpdateCookState()
{
}

void APatty::UpdateMaterial()
{
	TObjectPtr<UStaticMeshComponent> pMesh = GetMeshComp();
	bool bShouldChangeMat = false;
	FLinearColor newcolor;
	if (pMesh != nullptr)
	{
		switch (CookState)
		{
		case EPattyCookState::Raw:
			bShouldChangeMat = false;
			break;

		case EPattyCookState::Cooked:
			bShouldChangeMat = true;
			newcolor = FLinearColor(0.125,0.038,0.023);
			break;

		case EPattyCookState::Overcooked:
			bShouldChangeMat = true;
			newcolor = FLinearColor::Black;
			break;
		}

		if (!bShouldChangeMat) return;
		UMaterialInstanceDynamic* DynamicMaterial =
		 UMaterialInstanceDynamic::Create(pMesh->GetMaterial(0), this);

		if (!DynamicMaterial) return;

		DynamicMaterial->SetVectorParameterValue(TEXT("CookingLevel"), newcolor);

		pMesh->SetMaterial(0, DynamicMaterial);
	}
}