#include "InteractWidget.h"
#include "Components/TextBlock.h"

void UInteractWidget::SetInteractText(const FString& NewText)
{
    if (InteractText)
    {
        InteractText->SetText(FText::FromString(NewText));
    }
}