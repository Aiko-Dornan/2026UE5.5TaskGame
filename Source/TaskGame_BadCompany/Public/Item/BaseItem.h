#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "BaseItem.generated.h"

UCLASS()
class TASKGAME_BADCOMPANY_API ABaseItem : public AActor
{
    GENERATED_BODY()

public:
    ABaseItem();

protected:

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
    UStaticMeshComponent* Mesh;

public:

    // 拾われたときの処理（派生でオーバーライド可能）
    virtual void OnPickedUp(class APlayerCharacter* Player);
};
