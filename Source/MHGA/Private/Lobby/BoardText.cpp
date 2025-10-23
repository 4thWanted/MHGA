
#include "Lobby/BoardText.h"

#include "Components/TextBlock.h"

void UBoardText::Init(FString txt)
{
	TEXT_Content->SetText(FText::FromString(*txt));
}
