#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "WrapperBox.generated.h"

UCLASS()
class MHGA_API AWrapperBox : public AActor
{
	GENERATED_BODY()

/* Method */
public:
	AWrapperBox();

	virtual void Tick(float DeltaTime) override;

	void SpawnWrapper();

	
protected:
	virtual void BeginPlay() override;


private:


	
/* Field */
public:
	UPROPERTY(VisibleAnywhere)
	TObjectPtr<UStaticMeshComponent> Mesh;

	UPROPERTY(EditAnywhere)
	TSubclassOf<class AWrappingPaper> WrapperClass;

	UPROPERTY(EditAnywhere)
	TObjectPtr<class AWrappingPaper> Wrapper;

	UPROPERTY(EditAnywhere)
	TObjectPtr<class ATargetPoint> WrapperPoint;

	
private:



	
	
};
