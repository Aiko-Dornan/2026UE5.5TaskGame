#include "Item/BaseItem.h"
#include "Components/StaticMeshComponent.h"
#include "PlayerCharacter.h"

ABaseItem::ABaseItem()
{
    PrimaryActorTick.bCanEverTick = false;

    Mesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Mesh"));
    RootComponent = Mesh;

    Mesh->SetSimulatePhysics(true);
    Mesh->SetCollisionProfileName(TEXT("PhysicsActor"));
}

void ABaseItem::OnPickedUp(APlayerCharacter* Player)
{
    // 基本は消えるだけ
    Destroy();
}

void ABaseItem::SetHighlight(bool bEnable)
{
    if (Mesh)
    {
        Mesh->SetRenderCustomDepth(bEnable);
        Mesh->SetCustomDepthStencilValue(1); // ステンシル値（固定でOK）
    }
}