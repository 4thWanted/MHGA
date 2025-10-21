#pragma once

#include "CoreMinimal.h"
#include "Engine/GameInstance.h"
#include "Interfaces/OnlineSessionInterface.h"
#include "MHGAGameInstance.generated.h"

UCLASS()
class MHGA_API UMHGAGameInstance : public UGameInstance
{
	GENERATED_BODY()
	
public:
	//begin play
	virtual void Init() override;

	//세션 모든 처리를 진행하는 객체
	IOnlineSessionPtr SessionInterface;

	//세션 생성 관련
	//현재 세션 이름
	FName CurrentSessionName;
	//세션 생성 함수
	UFUNCTION(BlueprintCallable)
	void CreateMySession(FString displayName, int32 playerCount);
	//세선 생성 완료 함수
	void OnCreateSessionComplete(FName sessionName, bool bWasSuccessful);

	//세션 조회 관련
	//세션 조회할 때 사용하는 객체
	TSharedPtr<FOnlineSessionSearch> SessionSearch;
	//세션 조회 요청 함수
	UFUNCTION(BlueprintCallable)
	void FindOtherSession();
	//세션 조회 완료 함수
	void OnFindSessionComplete(bool bWasSuccessful);

	//세션 참여 관련
	//세션 참여 함수
	UFUNCTION(BlueprintCallable)
	void JoinOtherSession(int32 sessionIdx);
	//세션 참여 완료 함수
	void OnJoinSessionComplete(FName sessionName, EOnJoinSessionCompleteResult::Type result);
};

