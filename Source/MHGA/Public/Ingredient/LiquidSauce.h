#pragma once

#include "CoreMinimal.h"
#include "IngredientBase.h"
#include "LiquidSauce.generated.h"

class UNiagaraComponent;
class UDecalComponent;

UCLASS()
class MHGA_API ALiquidSauce : public AIngredientBase
{
	GENERATED_BODY()

/* Method */
public:
	ALiquidSauce();

	virtual void BeginPlay() override;
	virtual void Tick(float DeltaTime) override;
	
	void SetInitialVelocity(FVector Velocity);

protected:
	virtual void OnGrabbed(AMHGACharacter* Player) override;
	virtual void OnPut() override;

private:
	// 바닥에 닿으면 호출
	UFUNCTION()
	void OnLanded(UPrimitiveComponent* hitcomponent, AActor* otheractor, UPrimitiveComponent* othercomp, FVector normalimpulse, const FHitResult& hit);

	// 점성 효과 적용
	void ApplyViscosity(float DeltaTime);

	// 입자 효과 재생
	void PlayLiquidParticles();

	// 바닥에 흔적 남기기
	void CreateSauceDecal();

/* Field */
private:
	// 점성 계수 (0~1, 높을수록 느림)
	UPROPERTY(EditAnywhere, Category = "Liquid")
	float f_Viscosity = 0.92f;

	// 파티클
	UPROPERTY(EditAnywhere, Category = "Liquid")
	TObjectPtr<UNiagaraComponent> p_LiquidParticle;

	// 데칼
	UPROPERTY(EditAnywhere, Category = "Liquid")
	TObjectPtr<class UMaterialInterface> p_SauceDecalMaterial;

	// 상태 플래그
	UPROPERTY()
	bool b_Landed = false;

	UPROPERTY()
	bool b_Grabbed = false;

	// 시작 속도
	FVector InitialVelocity = FVector::ZeroVector;

	// 바닥 도착 후 경과 시간
	UPROPERTY()
	float f_TimeSinceLanded = 0.0f;
	
	UPROPERTY(EditAnywhere, Category = "Liquid")
	float f_MaxLifeTime = 30.0f;
};