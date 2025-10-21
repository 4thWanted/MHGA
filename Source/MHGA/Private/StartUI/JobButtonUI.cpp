// Fill out your copyright notice in the Description page of Project Settings.


#include "StartUI/JobButtonUI.h"

#include "Components/Button.h"
#include "Components/TextBlock.h"

void UJobButtonUI::NativeConstruct()
{
	Super::NativeConstruct();

	Btn_Session->OnClicked.AddDynamic(this, &UJobButtonUI::OnClickSessionBtn);
}

void UJobButtonUI::OnClickSessionBtn()
{
}

void UJobButtonUI::SetImage()
{
}

void UJobButtonUI::SetText(FString Name, int32 Current, int32 Max)
{
	TEXT_Name->SetText(FText::FromString(Name));
	TEXT_Description->SetText(FText::FromString(FString::Printf(TEXT("모집인원 : %d / %d"), Current, Max)));
}