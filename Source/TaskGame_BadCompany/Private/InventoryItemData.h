#pragma once

#include "CoreMinimal.h"
#include "InventoryItemData.generated.h"

UENUM()
enum class IDItemType : uint8
{
    DEBUG,
    TRAPPUT,
    TRAPTHROW,
    AID,
};

USTRUCT(BlueprintType)
struct FInventoryItemData
{
    GENERATED_BODY()

public:
    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    IDItemType ItemType;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    FName ItemID;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    FText ItemName;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    int32 Count = 1;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    TObjectPtr<UTexture2D> ItemIcon;//アイテム画像

};