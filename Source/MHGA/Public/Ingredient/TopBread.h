#pragma once

#include "CoreMinimal.h"
#include "IngredientBase.h"
#include "TopBread.generated.h"

UCLASS()
class MHGA_API ATopBread : public AIngredientBase
{
	GENERATED_BODY()

public:
	ATopBread();

protected:
	virtual void BeginPlay() override;

public:
	virtual void Tick(float DeltaTime) override;
};
