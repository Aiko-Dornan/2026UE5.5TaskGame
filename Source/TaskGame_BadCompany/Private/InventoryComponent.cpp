// Fill out your copyright notice in the Description page of Project Settings.


#include "InventoryComponent.h"

// Sets default values for this component's properties
UInventoryComponent::UInventoryComponent()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;

	// ...
}

//bool UInventoryComponent::AddItem(const FInventoryItemData& NewItem)
//{
//    // 同じIDのアイテムを探す
//    for (FInventoryItemData& Item : Items)
//    {
//        if (Item.ItemID == NewItem.ItemID)
//        {
//            Item.ItemName = NewItem.ItemName;
//            Item.Count += NewItem.Count;
//            return true;
//        }
//    }
//
//    // 新規追加
//    Items.Add(NewItem);
//
//    return true;
//}

// Called when the game starts
void UInventoryComponent::BeginPlay()
{
	Super::BeginPlay();

	// ...
    Items.SetNum(MaxSlotCount);

    for (FInventoryItemData& Item : Items)
    {
        Item = FInventoryItemData();
        Item.bIsEmpty = true;
        Item.Count = 0;
    }

}

bool UInventoryComponent::AddItem(const FInventoryItemData& NewItem)
{
    // =========================
    // 1. 既存スタックへ追加
    // =========================

    for (FInventoryItemData& Item : Items)
    {
        if (!Item.bIsEmpty &&
            Item.ItemID == NewItem.ItemID)
        {
            // 空きスタック数
            int32 SpaceLeft =
                Item.MaxStackCount - Item.Count;

            if (SpaceLeft > 0)
            {
                int32 AddCount =
                    FMath::Min(SpaceLeft, NewItem.Count);

                Item.Count += AddCount;

                OnInventoryUpdated.Broadcast();

                return true;
            }
        }
    }

    // =========================
    // 2. 空スロット探索
    // =========================

    for (FInventoryItemData& Item : Items)
    {
        if (Item.bIsEmpty)
        {
            Item = NewItem;
            Item.bIsEmpty = false;

            OnInventoryUpdated.Broadcast();

            return true;
        }
    }

    // =========================
    // 3. 満杯
    // =========================

    return false;
}

bool UInventoryComponent::UseItem(int32 Index)//アイテム消費
{
    if (!Items.IsValidIndex(Index))
        return false;

    FInventoryItemData& Item = Items[Index];

    if (Item.bIsEmpty)
        return false;

    // 1個消費
    Item.Count--;

    // 0なら空スロット化
    if (Item.Count <= 0)
    {
        Item = FInventoryItemData(); // 初期化
        Item.bIsEmpty = true;
        Item.Count = 0;
    }

    OnInventoryUpdated.Broadcast();

    return true;
}

//bool UInventoryComponent::AddItem(const FInventoryItemData& NewItem)
//{
//    for (FInventoryItemData& Item : Items)
//    {
//        if (Item.ItemID == NewItem.ItemID)
//        {
//            Item.ItemName = NewItem.ItemName;
//            Item.Count += NewItem.Count;
//
//            // 更新通知
//            OnInventoryUpdated.Broadcast();
//
//            return true;
//        }
//    }
//
//    Items.Add(NewItem);
//
//    // 更新通知
//    OnInventoryUpdated.Broadcast();
//
//    return true;
//}

// Called every frame
void UInventoryComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	// ...
}

