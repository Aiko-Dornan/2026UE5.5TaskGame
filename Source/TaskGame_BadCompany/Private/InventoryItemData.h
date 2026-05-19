#pragma once

#include "CoreMinimal.h"
#include "InventoryItemData.generated.h"

//UENUM()
//enum class IDItemType : uint8
//{
//    DEBUG,
//    TRAPPUT,
//    TRAPTHROW,
//    AID,
//};

class ABaseItem;

UENUM(BlueprintType)
enum class EItemType : uint8
{
    DEBUG      UMETA(DisplayName = "Debug"),
    TRAPPUT    UMETA(DisplayName = "TrapPut"),
    TRAPTHROW  UMETA(DisplayName = "TrapThrow"),
    AID        UMETA(DisplayName = "Aid"),
};

USTRUCT(BlueprintType)
struct FInventoryItemData
{
    GENERATED_BODY()

public:
    /*UPROPERTY(EditAnywhere, BlueprintReadWrite)
    IDItemType ItemType;*/

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    EItemType ItemType;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    FName ItemID;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    FText ItemName;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    int32 Count = 1;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    int32 MaxStackCount = 99;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    float ItemReUseCoolDown = 10.0f;//アイテムのクールダウンに使う予定

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    TObjectPtr<UTexture2D> ItemIcon;//アイテム画像

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    bool bIsEmpty = true;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    TSubclassOf<ABaseItem> ItemClass;

};