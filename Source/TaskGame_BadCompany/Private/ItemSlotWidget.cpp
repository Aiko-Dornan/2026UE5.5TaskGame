#include "ItemSlotWidget.h"
#include "Components/Image.h"
#include "Components/TextBlock.h"

void UItemSlotWidget::NativeConstruct()
{
    Super::NativeConstruct();

    // 初期状態は空スロット
    /*if (ItemIconImage)
    {
        ItemIconImage->SetVisibility(ESlateVisibility::Hidden);
    }*/

    /*if (ItemNameText)
    {
        ItemNameText->SetText(FText::GetEmpty());
    }

    if (CountText)
    {
        CountText->SetText(FText::GetEmpty());
    }*/
}

void UItemSlotWidget::SetItemData(const FInventoryItemData& ItemData)
{
    if (!ItemIconImage)
    {
        UE_LOG(LogTemp, Error, TEXT("ItemIconImage NULL"));
    }

    if (!ItemNameText)
    {
        UE_LOG(LogTemp, Error, TEXT("ItemNameText NULL"));
    }

    if (!CountText)
    {
        UE_LOG(LogTemp, Error, TEXT("CountText NULL"));
    }


    if (ItemIconImage && ItemData.ItemIcon)
    {
        ItemIconImage->SetVisibility(ESlateVisibility::Visible);
        ItemIconImage->SetBrushFromTexture(ItemData.ItemIcon);
        
        UE_LOG(LogTemp, Error, TEXT("ItemIconImage Set"));
    }

    if (ItemNameText)
    {
        ItemNameText->SetText(ItemData.ItemName);
        UE_LOG(LogTemp, Error, TEXT("ItemNameText %s Set"),*ItemData.ItemName.ToString());
    }

    if (CountText)
    {
        CountText->SetText(FText::AsNumber(ItemData.Count));
        UE_LOG(LogTemp, Error, TEXT("CountText %d Set"),ItemData.Count);

        if (ItemData.Count > 0)
        {
            CountText->SetText(FText::AsNumber(ItemData.Count));
            //CountText->SetVisibility(ESlateVisibility::Visible);
        }
        else
        {
            // 1個なら非表示でもOK（好み）
            CountText->SetText(FText::FromString(TEXT("")));
        }
    }
}

void UItemSlotWidget::ClearSlot()
{
    if (ItemIconImage)
    {
        // 透明 or 空画像に戻す
        ItemIconImage->SetBrushFromTexture(EmptyTexture);
    }

    if (ItemNameText)
    {
        ItemNameText->SetText(FText::FromString(TEXT("")));
    }

    if (CountText)
    {
        CountText->SetText(FText::FromString(TEXT("")));
    }
}

void UItemSlotWidget::SetSelected(bool bSelected)
{
    if (!InventorySlot) return;

    if (bSelected && SelectedTexture)
    {
        InventorySlot->SetBrushFromTexture(SelectedTexture);
    }
    else if (NormalTexture)
    {
        InventorySlot->SetBrushFromTexture(NormalTexture);
    }
}





