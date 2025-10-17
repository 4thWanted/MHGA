#pragma once

#include "CoreMinimal.h"
#include "GrabableProps.h"
#include "GameFramework/Actor.h"
#include "Hamburger.generated.h"

UCLASS()
class MHGA_API AHamburger : public AActor, public IGrabableProps
{
	GENERATED_BODY()

/* Method */
public:
	AHamburger();

	virtual void Tick(float DeltaTime) override;

	virtual void OnGrabbed(AMHGACharacter* Player) override;
	virtual void OnPut() override;
	virtual void OnUse() override;

	void SetName(FString Name);
	
protected:
	virtual void BeginPlay() override;

private:
	void PrintLog();



/* Field */
public:
	UPROPERTY(VisibleAnywhere)
	class UStaticMeshComponent* Mesh;

	UPROPERTY(EditAnywhere)
	bool bShowLog = true;
private:
	FString BurgerName;
	
};
