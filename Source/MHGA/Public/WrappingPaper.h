#pragma once

#include "CoreMinimal.h"
#include "BurgerData.h"
#include "GameFramework/Actor.h"
#include "WrappingPaper.generated.h"

UCLASS()
class MHGA_API AWrappingPaper : public AActor
{
	GENERATED_BODY()

/* Method */
public:
	AWrappingPaper();
	virtual void Tick(float DeltaTime) override;

	
protected:
	virtual void BeginPlay() override;

private:
	EBurgerMenu CheckIngredients();

	
/* Field */
public:


protected:


private:
	UPROPERTY()
	class UBoxComponent* Collision;
	float DetectRange;

	UPROPERTY()
	TMap<EIngredient, int32> OnArea;
	
};
