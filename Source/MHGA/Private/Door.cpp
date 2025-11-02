// Fill out your copyright notice in the Description page of Project Settings.


#include "Door.h"

#include "AI/CustomerAI.h"
#include "Components/BoxComponent.h"
#include "Net/UnrealNetwork.h"

// Sets default values
ADoor::ADoor()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	bReplicates = true;

	RootComp = CreateDefaultSubobject<USceneComponent>(TEXT("RootComp"));
	SetRootComponent(RootComp);

	DoorFrame = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("DoorFrame"));
	DoorFrame->SetupAttachment(RootComp);

	LeftDoor = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("LeftDoor"));
	LeftDoor->SetupAttachment(RootComp);

	RightDoor = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("RightDoor"));
	RightDoor->SetupAttachment(RootComp);

	TriggerVolume = CreateDefaultSubobject<UBoxComponent>(TEXT("TriggerVolume"));
	TriggerVolume->SetupAttachment(RootComp);

	bIsOpen = false;
	bSwingForward = true;
}

// Called when the game starts or when spawned
void ADoor::BeginPlay()
{
	Super::BeginPlay();
	
	if (HasAuthority())
	{
		TriggerVolume->OnComponentBeginOverlap.AddDynamic(this, &ADoor::OnOverlapBegin);
		TriggerVolume->OnComponentEndOverlap.AddDynamic(this, &ADoor::OnOverlapEnd);
	}

	// 닫힌 상태의 회전값 저장
	ClosedRotation = LeftDoor->GetRelativeRotation();

	// 초기 목표 각도를 닫힌 상태로 설정
	TargetYawLeft = ClosedRotation.Yaw;
	TargetYawRight = ClosedRotation.Yaw;
}

// Called every frame
void ADoor::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	
	// 문 회전 로직 호출
	UpdateDoorRotation(DeltaTime);
}

void ADoor::GetLifetimeReplicatedProps(TArray<class FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	DOREPLIFETIME(ADoor, bIsOpen);
	DOREPLIFETIME(ADoor, bSwingForward);
}
// --- 핵심 로직: 오버랩 (서버 전용) ---

void ADoor::OnOverlapBegin(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
    if (OtherActor && OtherActor->IsA(ACustomerAI::StaticClass()))
    {
        if (!bIsOpen)
        {
            // 1. 방향 설정
            bSwingForward = IsActorInFrontOfDoor(OtherActor);
            // 2. 상태 변경 (RepNotify 발동)
            bIsOpen = true;
            // 3. 서버에서 RepNotify 수동 호출
            OnRep_IsOpen();
        }
    }
}

void ADoor::OnOverlapEnd(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
    if (OtherActor && OtherActor->IsA(ACustomerAI::StaticClass()))
    {
        TArray<AActor*> OverlappingActors;
        TriggerVolume->GetOverlappingActors(OverlappingActors, ACustomerAI::StaticClass());

        if (OverlappingActors.Num() == 0 && bIsOpen)
        {
            // 1. 상태 변경 (RepNotify 발동)
            bIsOpen = false;
            // 2. 서버에서 RepNotify 수동 호출
            OnRep_IsOpen();
        }
    }
}

// --- 핵심 로직: RepNotify (서버 + 모든 클라이언트) ---

void ADoor::OnRep_IsOpen()
{
    // 이 함수는 애니메이션을 직접 실행하는 대신 '목표'만 설정합니다.
    // 실제 애니메이션은 Tick에서 처리됩니다.

    if (bIsOpen)
    {
        // bSwingForward 값에 따라 목표 Yaw 각도를 결정
        TargetYawLeft = bSwingForward ? -OpenAngle : OpenAngle;
        TargetYawRight = bSwingForward ? OpenAngle : -OpenAngle;
    }
    else
    {
        // 닫힐 때의 목표 (초기 닫힌 각도)
        TargetYawLeft = ClosedRotation.Yaw;
        TargetYawRight = ClosedRotation.Yaw;
    }
}

// --- 핵심 로직: 애니메이션 (Tick에서 호출됨) ---

void ADoor::UpdateDoorRotation(float DeltaTime)
{
    // 1. 현재 문의 Yaw 각도를 가져옵니다.
    const float CurrentYawLeft = LeftDoor->GetRelativeRotation().Yaw;
    const float CurrentYawRight = RightDoor->GetRelativeRotation().Yaw;

    // 2. 현재 각도와 목표 각도가 거의 같다면 Tick 연산을 건너뜁니다 (최적화)
    if (FMath::IsNearlyEqual(CurrentYawLeft, TargetYawLeft) && FMath::IsNearlyEqual(CurrentYawRight, TargetYawRight))
    {
        return;
    }

    // 3. FInterpTo를 사용해 현재 각도에서 목표 각도로 부드럽게 보간합니다.
    const float NewYawLeft = FMath::FInterpTo(CurrentYawLeft, TargetYawLeft, DeltaTime, DoorAnimSpeed);
    const float NewYawRight = FMath::FInterpTo(CurrentYawRight, TargetYawRight, DeltaTime, DoorAnimSpeed);

    // 4. 문에 새로운 회전값을 적용합니다.
    LeftDoor->SetRelativeRotation(FRotator(0.f, NewYawLeft, 0.f));
    RightDoor->SetRelativeRotation(FRotator(0.f, NewYawRight, 0.f));
}

// --- (중요) 문 앞/뒤 구분 ---

bool ADoor::IsActorInFrontOfDoor(AActor* Actor) const
{
    if (!Actor) return false;

    const FVector DirectionToActor = (Actor->GetActorLocation() - GetActorLocation()).GetSafeNormal();
    const FVector DoorForward = GetActorForwardVector();
    const float Dot = FVector::DotProduct(DoorForward, DirectionToActor);
    
    return Dot > 0.f; // 0보다 크면 '앞'
}