// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Door.generated.h"

UCLASS()
class MHGA_API ADoor : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ADoor();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;
	virtual void GetLifetimeReplicatedProps(TArray<class FLifetimeProperty>& OutLifetimeProps) const override;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Door")
	class USceneComponent* RootComp;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Door")
	class UStaticMeshComponent* DoorFrame;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Door")
	class UStaticMeshComponent* LeftDoor;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Door")
	class UStaticMeshComponent* RightDoor;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Door")
	class UBoxComponent* TriggerVolume;

	// --- 문 열림 설정 ---

	UPROPERTY(EditAnywhere, Category = "Door")
	float OpenAngle = 90.0f;

	// 문이 열리고 닫히는 속도 (높을수록 빠름)
	UPROPERTY(EditAnywhere, Category = "Door")
	float DoorAnimSpeed = 5.0f;

	// 닫힌 상태의 문 회전 (BeginPlay에서 자동 저장됨)
	FRotator ClosedRotation;

	// --- 멀티플레이 & 상태 ---

	UPROPERTY(ReplicatedUsing = OnRep_IsOpen)
	bool bIsOpen;

	UPROPERTY(Replicated)
	bool bSwingForward;

	UFUNCTION()
	void OnRep_IsOpen();

	// --- 오버랩 (트리거) 로직 ---

	UFUNCTION()
	void OnOverlapBegin(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

	UFUNCTION()
	void OnOverlapEnd(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex);

	bool IsActorInFrontOfDoor(AActor* Actor) const;

	// --- 애니메이션 (Tick) ---
    
	// 현재 각 문이 도달해야 할 '목표' Yaw 각도
	float TargetYawLeft = 0.0f;
	float TargetYawRight = 0.0f;

	// 애니메이션을 부드럽게 하기 위한 함수
	void UpdateDoorRotation(float DeltaTime);

};
