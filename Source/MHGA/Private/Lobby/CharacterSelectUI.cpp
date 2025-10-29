#include "Lobby/CharacterSelectUI.h"

#include "Components/Button.h"

void UCharacterSelectUI::NativeConstruct()
{
	Super::NativeConstruct();

	BTN_P1->OnClicked.AddDynamic(this, &UCharacterSelectUI::OnClicked_P1);
	BTN_P2->OnClicked.AddDynamic(this, &UCharacterSelectUI::OnClicked_P2);
	BTN_P3->OnClicked.AddDynamic(this, &UCharacterSelectUI::OnClicked_P3);
	BTN_P4->OnClicked.AddDynamic(this, &UCharacterSelectUI::OnClicked_P4);
}

void UCharacterSelectUI::OnClicked_P1()
{
}

void UCharacterSelectUI::OnClicked_P2()
{
}

void UCharacterSelectUI::OnClicked_P3()
{
}

void UCharacterSelectUI::OnClicked_P4()
{
}
