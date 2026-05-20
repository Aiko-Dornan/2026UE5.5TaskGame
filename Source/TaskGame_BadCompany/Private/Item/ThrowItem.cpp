// Fill out your copyright notice in the Description page of Project Settings.


#include "Item/ThrowItem.h"
//#include "Kismet/GameplayStatics.h"
//#include "PlayerCharacter.h"

AThrowItem::AThrowItem()
{

    //Mesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Mesh"));
    //RootComponent = Mesh;

    Mesh->SetMobility(EComponentMobility::Movable);
    Mesh->SetSimulatePhysics(false);


    ProjectileMovement =
        CreateDefaultSubobject<UProjectileMovementComponent>(
            TEXT("ProjectileMovement")
        );
    ProjectileMovement->InitialSpeed = 0.f;
    ProjectileMovement->MaxSpeed = 9000.f;

   

    //PrimaryActorTick.bCanEverTick = true;

    /*APlayerController* PC = UGameplayStatics::GetPlayerController(GetWorld(), 0);

    if (PC)
    {
        APlayerCharacter* Player =
            Cast<APlayerCharacter>(PC->GetPawn());

        ProjectileMovement->InitialSpeed = Player->CurrentThrowPower;
        ProjectileMovement->MaxSpeed = Player->CurrentThrowPower;

        UE_LOG(LogTemp, Warning,
            TEXT("CurrentSpeedPower IN ITEM: %f"),
            Player->CurrentThrowPower);

    }
    else
    {
        UE_LOG(LogTemp, Warning,
            TEXT("ThrowPower No Initial: "));
    }*/
    ProjectileMovement->SetUpdatedComponent(Mesh);
    //Mesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);
    // 重力を持たせる
    ProjectileMovement->ProjectileGravityScale = ItemGravityScale;

    // 跳ねるか
    ProjectileMovement->bShouldBounce = bIsBounce;

    //壁とか床とかに当たった時の反発度
    ProjectileMovement->Bounciness = ItemBouncePower;

    // 投擲フラグが立つまでは停止
    //ProjectileMovement->Deactivate();
    //ProjectileMovement->SetUpdatedComponent(nullptr);

    

  
    
  

    UE_LOG(LogTemp, Warning, TEXT("Sim: %d"), ProjectileMovement->IsActive());
    
    UE_LOG(LogTemp, Warning, TEXT("UpdatedComponent: %s"),
        *GetNameSafe(ProjectileMovement->UpdatedComponent));

    

}

void AThrowItem::BeginPlay()
{
    Super::BeginPlay();

    ProjectileMovement->SetUpdatedComponent(Mesh);
}

//void AThrowItem::Tick(float DeltaTime)
//{
//    Super::Tick(DeltaTime);
//
//    if (bIsThrown)
//    {
//        ProjectileMovement->InitialSpeed = ThrowPower2;
//    }
//
//}

void AThrowItem::SetThrowPower(
    float NewPower
)
{
    ThrowPower2 = NewPower;
}

void AThrowItem::Throw(
    const FVector& Direction,float ThrowPower
    
)
{
   
    bIsThrown = true;

    if (!ProjectileMovement || !Mesh) return;

    ProjectileMovement->StopMovementImmediately();

    ProjectileMovement->SetUpdatedComponent(Mesh);

    FVector Velocity = Direction.GetSafeNormal() * ThrowPower;

    ProjectileMovement->Velocity = Velocity;
    ProjectileMovement->Activate(true);

    //ProjectileMovement->InitialSpeed = ThrowPower;

    UE_LOG(LogTemp, Warning,
        TEXT("ThrowPower IN ITEM: %f"),
        ThrowPower);

    UE_LOG(LogTemp, Warning,
        TEXT("InitialSpeedPower IN ITEM: %f"),
        ProjectileMovement->InitialSpeed);
    ThrowPower2 = ThrowPower;

    UE_LOG(LogTemp, Warning, TEXT("AFTER Velocity: %s"),
        *ProjectileMovement->Velocity.ToString());

}

void AThrowItem::SearchMine()
{



}

void AThrowItem::FireMine()
{



}

void AThrowItem::FireSound()
{



}

void AThrowItem::FireSmoke()
{



}

void AThrowItem::CreateFireWall()
{



}

void AThrowItem::WaitFireTime()
{



}

//void AThrowItem::UseItem(APlayerCharacter* Player)
//{
//    switch (ThrowType)
//    {
//    case EThrowItemType::MINE:
//        FireMine();
//        break;
//
//    case EThrowItemType::SOUND:
//        FireSound();
//        break;
//
//    case EThrowItemType::SMOKE:
//        FireSmoke();
//        break;
//    }
//}



