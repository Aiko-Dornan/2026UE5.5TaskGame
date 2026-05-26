#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "InventoryItemData.h"
#include "BaseItem.generated.h"

UCLASS()
class TASKGAME_BADCOMPANY_API ABaseItem : public AActor
{
    GENERATED_BODY()

public:
    ABaseItem();
    void SetHighlight(bool bEnable);
protected:

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
    UStaticMeshComponent* Mesh;
    

public:
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Item")
    FInventoryItemData ItemData;

    bool ImpossiblePick = false;

    UPROPERTY(EditAnywhere, BlueprintReadWrite ,Category= "Item")
    FText ItemName = FText::FromString("Nameless");
    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    FText InteractText = FText::FromString(":Eで拾う");
    // 拾われたときの処理（派生でオーバーライド可能）
    virtual void OnPickedUp(class APlayerCharacter* Player);

    //virtual void UseItem(APlayerCharacter* Player);
};
