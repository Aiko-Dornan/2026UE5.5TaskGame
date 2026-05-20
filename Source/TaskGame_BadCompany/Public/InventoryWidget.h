// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include"ItemSlotWidget.h"
#include "Components/UniformGridPanel.h"
#include"Components/UniformGridSlot.h"
#include "InventoryWidget.generated.h"

/**
 * 
 */

class UItemSlotWidget;
class UUniformGridSlot;

UCLASS()
class TASKGAME_BADCOMPANY_API UInventoryWidget : public UUserWidget
{
	GENERATED_BODY()

public:

    UFUNCTION(BlueprintCallable)
    void InitializeInventory(UInventoryComponent* Inventory);

    UFUNCTION()
    void RefreshInventory();

    UFUNCTION()
    void MoveSelection(int32 Direction);

    UFUNCTION()
    void UseSelectedItem();

    EItemType GetSelectedItemType() const;

private:

    UPROPERTY()
    UInventoryComponent* InventoryRef;
	
public:

    UPROPERTY(meta = (BindWidget))
    UUniformGridPanel* ItemGrid;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    TSubclassOf<UItemSlotWidget> ItemSlotClass;

    int32 SelectedIndex = 0;
};
