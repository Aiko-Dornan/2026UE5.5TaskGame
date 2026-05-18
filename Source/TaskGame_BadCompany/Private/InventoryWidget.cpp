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
    if (!InventoryRef) return;

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

    for (int32 i = 0; i < InventoryRef->Items.Num(); i++)
    {
        UItemSlotWidget* ItemSlot =
            CreateWidget<UItemSlotWidget>(
                GetOwningPlayer(),
                ItemSlotClass
            );

        if (!ItemSlot)
        {
            UE_LOG(LogTemp, Error, TEXT("Slot Create Failed"));
            continue;
        }

        ItemSlot->SetItemData(InventoryRef->Items[i]);
        const int32 ColumnCount = 5;
        int32 Row = i / ColumnCount;
        int32 Col = i % ColumnCount;

       /* UUniformGridSlot* GridSlot =
            ItemGrid->AddChildToUniformGrid(
                ItemSlot,
                Row,
                Col
            );

        if (GridSlot)
        {
            GridSlot->Padding=(FMargin(2.f));
        }*/

        ItemGrid->AddChildToUniformGrid(
            ItemSlot,
            i / 5,
            i % 5
        );
    }
}

//void UInventoryWidget::RefreshInventory()
//{
//    if (!InventoryRef || !ItemGrid) return;
//
//    ItemGrid->ClearChildren();
//
//    const int32 ColumnCount = 5;
//
//    for (int32 i = 0; i < InventoryRef->Items.Num(); i++)
//    {
//        const FInventoryItemData& Item = InventoryRef->Items[i];
//
//        UItemSlotWidget* ItemSlot =
//            CreateWidget<UItemSlotWidget>(
//                GetOwningPlayer(),
//                ItemSlotClass
//            );
//
//        if (ItemSlot)
//        {
//            ItemSlot->SetItemData(Item);
//
//            int32 Row = i / ColumnCount;
//            int32 Col = i % ColumnCount;
//
//            ItemGrid->AddChildToUniformGrid(
//                ItemSlot,
//                Row,
//                Col
//            );
//        }
//    }
//}
