#pragma once

#include "CoreMinimal.h"
#include "OnlineSubsystem.h"
#include "Engine/GameInstance.h"
#include "Interfaces/OnlineSessionInterface.h"
#include "Interfaces/VoiceInterface.h"
#include "MHGAGameInstance.generated.h"

DECLARE_DELEGATE_OneParam(FFindComplete, TArray<FOnlineSessionSearchResult>&)

UCLASS()
class MHGA_API UMHGAGameInstance : public UGameInstance
{
	GENERATED_BODY()

protected:
	//begin play
	virtual void Init() override;
	virtual void Shutdown() override;

public:
	//세션 찾았을때 담당 정보 넘기기 delegate
	FFindComplete FindCompleteDelegate;

protected:
	FString NickName = FString(TEXT("Player"));

	//세션 모든 처리진행하는 객체
	IOnlineSessionPtr SessionInterface;

	//현재 세션 이름
	FName CurrentSessionName;
	bool bIsHostingSession = false;
	bool bPendingReturnToStart = false;

	//세션 조회할 때 사용하는 객체
	TSharedPtr<FOnlineSessionSearch> SessionSearch;

	//보이스 채팅 제어용 멤버
	IOnlineVoicePtr VoiceInterface;
	FDelegateHandle PostLoadMapHandle;
	bool bVoiceChatDesired = true;
	bool bVoiceChatActive = false;
	int32 LocalVoiceUserNum = 0;

	void HandlePostLoadMap(class UWorld* LoadedWorld);
	void UpdateVoiceChatForWorld(class UWorld* LoadedWorld);
	bool ShouldEnableVoiceChatForWorld(const class UWorld* LoadedWorld) const;
	bool InitializeVoiceInterface(class UWorld* ForWorld);
	void StartVoiceChat(class UWorld* ForWorld);
	void StopVoiceChat();

	//세션 생성 완료 함수
	void OnCreateSessionComplete(FName sessionName, bool bWasSuccessful);
	//세션 조회 완료 함수
	void OnFindSessionComplete(bool bWasSuccessful);
	//세션 참여 완료 함수
	void OnJoinSessionComplete(FName sessionName, EOnJoinSessionCompleteResult::Type result);
	void OnEndSessionComplete(FName sessionName, bool bWasSuccessful);
	void OnDestroySessionComplete(FName sessionName, bool bWasSuccessful);

	void TravelBackToStartLevel();

public:
	//세션 생성 관련
	//세션 생성 함수
	UFUNCTION(BlueprintCallable)
	void CreateMySession(FString displayName, int32 playerCount);

	//세션 조회 관련
	//세션 조회 요청 함수
	UFUNCTION(BlueprintCallable)
	void FindOtherSession();

	//세션 참여 관련
	//세션 참여 함수
	UFUNCTION(BlueprintCallable)
	void JoinOtherSession(int32 sessionIdx);

	UFUNCTION(BlueprintCallable)
	void LeaveSessionAndReturnToStart();

	//유저네임 설정
	void SetPlayerName(FString name) { NickName = name; }
	FString GetPlayerName() { return NickName; }

	//문자열을 UTF-8 -> base64로 Encode하는 함수
	FString StringBase64Encode(FString str);
	//문자열을 base64 -> UTF-8으로 Decode하는 함수
	FString StringBase64Decode(FString str);

	//보이스 채팅 활성/비활성 설정
	UFUNCTION(BlueprintCallable, Category="Voice")
	void SetVoiceChatEnabled(bool bEnable);

	//보이스 채팅 동작 여부 확인
	UFUNCTION(BlueprintPure, Category="Voice")
	bool IsVoiceChatActive() const { return bVoiceChatActive; }
};
