// Fill out your copyright notice in the Description page of Project Settings.


#include "InventoryWidget.h"
#include "InventoryComponent.h"

void UInventoryWidget::InitializeInventory(UInventoryComponent* Inventory)
{
    if (!Inventory) return;

    InventoryRef = Inventory;

    // Inventory更新イベントを購読
    InventoryRef->OnInventoryUpdated.AddDynamic(
        this,
        &UInventoryWidget::RefreshInventory
    );

    // 初回更新
    RefreshInventory();
}

void UInventoryWidget::RefreshInventory()
{
    //if (!InventoryRef) return;



    if (!ItemGrid)
    {
        UE_LOG(LogTemp, Error, TEXT("ItemGrid NULL"));
        return;
    }

    if (!ItemSlotClass)
    {
        UE_LOG(LogTemp, Error, TEXT("ItemSlotClass NULL"));
        return;
    }

    ItemGrid->ClearChildren();

    const int32 MaxSlotCount = 5;
    const int32 ColumnCount = 5;

    for (int32 i = 0; i < MaxSlotCount; i++)
    {
        UItemSlotWidget* ItemSlot =
            CreateWidget<UItemSlotWidget>(
                GetOwningPlayer(),
                ItemSlotClass
            );

        if (!ItemSlot)
        {
            UE_LOG(LogTemp, Error, TEXT("NanteKotoDA!"));
            continue;
        }

        if (InventoryRef->Items.IsValidIndex(i) &&
            !InventoryRef->Items[i].bIsEmpty)
        {
            ItemSlot->SetItemData(InventoryRef->Items[i]);
        }
        else
        {
            ItemSlot->ClearSlot();
        }

        // アイテムが存在する場合のみデータ設定
        if (InventoryRef->Items.IsValidIndex(i))
        {
            UE_LOG(LogTemp, Error, TEXT("ARUYAN!"));
            ItemSlot->SetItemData(InventoryRef->Items[i]);
        }

        int32 Row = i / ColumnCount;
        int32 Col = i % ColumnCount;

        UUniformGridSlot* GridSlot =
            ItemGrid->AddChildToUniformGrid(
                ItemSlot,
                Row,
                Col
            );

        ItemSlot->SetSelected(i == SelectedIndex);

       /* if (GridSlot)
        {
            GridSlot->SetPadding(FMargin(0.f));
        }*/
    }

}

void UInventoryWidget::MoveSelection(int32 Direction)
{
    const int32 MaxSlot = 5;

    SelectedIndex += Direction;

    if (SelectedIndex < 0)
        SelectedIndex = MaxSlot - 1;

    if (SelectedIndex >= MaxSlot)
        SelectedIndex = 0;

    RefreshInventory();
}

void UInventoryWidget::UseSelectedItem()
{
    if (!InventoryRef) return;

    APlayerController* PC = GetOwningPlayer();

    if (PC)
    {
        APlayerCharacter* Player =
            Cast<APlayerCharacter>(PC->GetPawn());

        if (Player)
        {
            InventoryRef->UseItem(SelectedIndex, Player);
        }
    }

    RefreshInventory();

    /*InventoryRef->UseItem(SelectedIndex);

    */
}