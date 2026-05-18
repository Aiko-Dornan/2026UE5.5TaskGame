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
	
}

bool UInventoryComponent::AddItem(const FInventoryItemData& NewItem)
{
    for (FInventoryItemData& Item : Items)
    {
        if (Item.ItemID == NewItem.ItemID)
        {
            Item.ItemName = NewItem.ItemName;
            Item.Count += NewItem.Count;

            // 更新通知
            OnInventoryUpdated.Broadcast();

            return true;
        }
    }

    Items.Add(NewItem);

    // 更新通知
    OnInventoryUpdated.Broadcast();

    return true;
}

// Called every frame
void UInventoryComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	// ...
}

