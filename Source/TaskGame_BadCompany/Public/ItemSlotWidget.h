// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "InventoryItemData.h"
#include "ItemSlotWidget.generated.h"

/**
 * 
 */

class UImage;
class UTextBlock;

UCLASS()
class TASKGAME_BADCOMPANY_API UItemSlotWidget : public UUserWidget
{
	GENERATED_BODY()

public:

    void SetItemData(const FInventoryItemData& ItemData);

    void ClearSlot();

    void SetSelected(bool bSelected);

    

protected:

    virtual void NativeConstruct() override;

    UPROPERTY(meta = (BindWidget))
    UImage* InventorySlot;

    UPROPERTY(EditDefaultsOnly, Category = "UI")
    UTexture2D* NormalTexture;

    UPROPERTY(EditDefaultsOnly, Category = "UI")
    UTexture2D* SelectedTexture;

    UPROPERTY(meta = (BindWidget))
    UImage* ItemIconImage;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    UTexture2D* EmptyTexture;

    UPROPERTY(meta = (BindWidget))
    UTextBlock* ItemNameText;

    UPROPERTY(meta = (BindWidget))
    UTextBlock* CountText;
	
   

};
