#include "Item/InstallationItem.h"
#include "PlayerCharacter.h"


AInstallationItem::AInstallationItem()
{
    Mesh->SetSimulatePhysics(false);

    Mesh->SetCollisionEnabled(
        ECollisionEnabled::QueryAndPhysics
    );
}

//bool AInstallationItem::InstallItem(
//    APlayerCharacter* Player)
//{
//    if (!Player)
//    {
//        return false;
//    }
//
//    FVector Start;
//    FRotator ViewRot;
//
//    Player->GetController()->GetPlayerViewPoint(
//        Start,
//        ViewRot);
//
//    FVector End =
//        Start +
//        ViewRot.Vector() * InstallationDistance;
//
//    FHitResult Hit;
//
//    FCollisionQueryParams Params;
//    Params.AddIgnoredActor(Player);
//
//    bool bHit =
//        GetWorld()->LineTraceSingleByChannel(
//            Hit,
//            Start,
//            End,
//            ECC_Visibility,
//            Params);
//
//    if (!bHit)
//    {
//        return false;
//    }
//
//    // 地面以外は設置不可
//    if (Hit.ImpactNormal.Z < 0.7f)
//    {
//        return false;
//    }
//
//    SpawnInstalledActor(Hit);
//
//    return true;
//}
//void AInstallationItem::SpawnInstalledActor(
//    const FHitResult& Hit)
//{
//
//
//
//}