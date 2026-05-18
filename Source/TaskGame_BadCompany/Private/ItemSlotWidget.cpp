#include "ItemSlotWidget.h"
#include "Components/Image.h"
#include "Components/TextBlock.h"

void UItemSlotWidget::NativeConstruct()
{
    Super::NativeConstruct();
}

void UItemSlotWidget::SetItemData(const FInventoryItemData& ItemData)
{
    if (ItemIconImage && ItemData.ItemIcon)
    {
        ItemIconImage->SetBrushFromTexture(ItemData.ItemIcon);
    }

    if (ItemNameText)
    {
        ItemNameText->SetText(ItemData.ItemName);
    }

    if (CountText)
    {
        CountText->SetText(FText::AsNumber(ItemData.Count));
    }
}