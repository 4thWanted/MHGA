// Fill out your copyright notice in the Description page of Project Settings.


#include "StartUI/StartUI.h"

#include "Components/Button.h"
#include "Components/CanvasPanel.h"

void UStartUI::NativeConstruct()
{
	Super::NativeConstruct();

	Btn_SearchJob->OnClicked.AddDynamic(this, &UStartUI::OnClickSearchBtn);
	Btn_Refresh->OnClicked.AddDynamic(this, &UStartUI::OnClickRefreshBtn);
	Btn_MakeJob->OnClicked.AddDynamic(this, &UStartUI::OnClickMakeJobBtn);
	Btn_MakeJobExit->OnClicked.AddDynamic(this, &UStartUI::OnClickMakeJobExitBtn);
	Btn_RegisterJob->OnClicked.AddDynamic(this, &UStartUI::OnClickRegisterJobBtn);


	Canvas_Session->SetVisibility(ESlateVisibility::Hidden);
	Canvas_MakeJob->SetVisibility(ESlateVisibility::Hidden);
}

void UStartUI::OnClickSearchBtn()
{
	Canvas_Session->SetVisibility(ESlateVisibility::Visible);
	Canvas_Start->SetVisibility(ESlateVisibility::Hidden);

	OnClickRefreshBtn();
}

void UStartUI::OnClickRefreshBtn()
{
}

void UStartUI::OnClickMakeJobBtn()
{
	Canvas_MakeJob->SetVisibility(ESlateVisibility::Visible);
}

void UStartUI::OnClickMakeJobExitBtn()
{
	Canvas_MakeJob->SetVisibility(ESlateVisibility::Hidden);
}

void UStartUI::OnClickRegisterJobBtn()
{
	//이게 알바 만들는 것 - 방 만들기
}
