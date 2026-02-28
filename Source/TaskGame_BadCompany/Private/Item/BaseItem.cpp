#include "Item/BaseItem.h"
#include "Components/StaticMeshComponent.h"
#include "PlayerCharacter.h"

ABaseItem::ABaseItem()
{
    PrimaryActorTick.bCanEverTick = false;

    Mesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Mesh"));
    RootComponent = Mesh;

    // ===== 物理設定 =====
    Mesh->SetCollisionProfileName(TEXT("Custom"));
   // Mesh->SetSimulatePhysics(false);
    Mesh->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);

    // ★ Itemチャンネルに変更
    Mesh->SetCollisionObjectType(ECC_GameTraceChannel1);

    // 全部Block
    Mesh->SetCollisionResponseToAllChannels(ECR_Block);

    // PawnだけIgnore
    Mesh->SetCollisionResponseToChannel(ECC_Pawn, ECR_Ignore);

    //Mesh->SetSimulatePhysics(true);
    //Mesh->SetCollisionProfileName(TEXT("PhysicsActor"));
    ////Mesh->SetSimulatePhysics(false);

    //Mesh->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
    //Mesh->SetCollisionObjectType(ECC_WorldDynamic);
    // // ★ Pawnとの当たり判定だけ無効化
    //Mesh->SetCollisionResponseToAllChannels(ECR_Block);
    //Mesh->SetCollisionResponseToChannel(ECC_Pawn, ECR_Ignore);
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