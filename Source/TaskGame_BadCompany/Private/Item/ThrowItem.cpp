// Fill out your copyright notice in the Description page of Project Settings.


#include "Item/ThrowItem.h"
//#include "Kismet/GameplayStatics.h"
//#include "PlayerCharacter.h"

AThrowItem::AThrowItem()
{

    //Mesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Mesh"));
    //RootComponent = Mesh;

    Mesh->SetMobility(EComponentMobility::Movable);
    //Mesh->SetSimulatePhysics(true);
    Mesh->SetNotifyRigidBodyCollision(true);

    Mesh->SetCollisionEnabled(
        ECollisionEnabled::QueryAndPhysics
    );

    Mesh->SetCollisionObjectType(
        ECC_GameTraceChannel1
    );

    Mesh->SetCollisionResponseToAllChannels(
        ECR_Block
    );

    Mesh->SetCollisionResponseToChannel(
        ECC_Pawn,
        ECR_Ignore
    );

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

    //滑る力
    ProjectileMovement->Friction = ItemFrictionPower;
    
    //速度が一定以下なら停止してくれる
    ProjectileMovement
        ->BounceVelocityStopSimulatingThreshold = ItemStopSpeed;

    UE_LOG(LogTemp, Warning, TEXT("Bind Hit Event"));

    UE_LOG(LogTemp, Warning, TEXT("Sim: %d"), ProjectileMovement->IsActive());
    
    UE_LOG(LogTemp, Warning, TEXT("UpdatedComponent: %s"),
        *GetNameSafe(ProjectileMovement->UpdatedComponent));

    

}

void AThrowItem::BeginPlay()
{
    Super::BeginPlay();

    ProjectileMovement->SetUpdatedComponent(Mesh);

    Mesh->SetNotifyRigidBodyCollision(true);

    

    /*Mesh->OnComponentHit.AddDynamic(
        this,
        &AThrowItem::OnMeshHit
    );

    ProjectileMovement->OnProjectileBounce.AddDynamic(
        this,
        &AThrowItem::OnProjectileBounce
    );

    Mesh->OnComponentHit.AddDynamic(
        this,
        &AThrowItem::OnHit
    );*/

    /*if (ProjectileMovement)
    {

        ProjectileMovement->OnProjectileBounce.RemoveDynamic(
            this,
            &AThrowItem::OnProjectileBounce
        );

        ProjectileMovement->OnProjectileBounce.AddDynamic(
            this,
            &AThrowItem::OnProjectileBounce
        );

    }
    else
    {
        UE_LOG(LogTemp, Warning,TEXT("No Bounce: ") );
    }*/

    if (Mesh)
    {
        Mesh->OnComponentHit.RemoveDynamic(
            this,
            &AThrowItem::OnMeshHit
        );

        Mesh->OnComponentHit.AddDynamic(
            this,
            &AThrowItem::OnMeshHit
        );
    }

    //if (ProjectileMovement)
    //{
    //    ProjectileMovement->OnProjectileStop.RemoveDynamic(this, &AThrowItem::OnThrowItemStop);//地面とかに当たった時の関数

    //    ProjectileMovement->OnProjectileStop.AddDynamic(this, &AThrowItem::OnThrowItemStop);//地面とかに当たった時の関数
    //}

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

//void AThrowItem::SetThrowPower(
//    float NewPower
//)
//{
//    ThrowPower2 = NewPower;
//}

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

    //UE_LOG(LogTemp, Warning,
    //    TEXT("ThrowPower IN ITEM: %f"),
    //    ThrowPower);

    //UE_LOG(LogTemp, Warning,
    //    TEXT("InitialSpeedPower IN ITEM: %f"),
    //    ProjectileMovement->InitialSpeed);
    //ThrowPower2 = ThrowPower;

    //UE_LOG(LogTemp, Warning, TEXT("AFTER Velocity: %s"),
    //    *ProjectileMovement->Velocity.ToString());

}

//void AThrowItem::OnThrowItemStop(
//    const FHitResult& ImpactResult
//)
//{
//    UE_LOG(LogTemp, Warning,
//        TEXT("Throw Item Stop! Hit: %s"),
//        *GetNameSafe(ImpactResult.GetActor())
//    );
//
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
//
//    case EThrowItemType::FIREWALL:
//        CreateFireWall();
//        break;
//    }
//
//    // 例:
//    // 投擲終了フラグ
//    bIsThrown = false;
//}

void AThrowItem::OnMeshHit(
    UPrimitiveComponent* HitComponent,
    AActor* OtherActor,
    UPrimitiveComponent* OtherComp,
    FVector NormalImpulse,
    const FHitResult& Hit
)
{
    UE_LOG(LogTemp, Warning,
        TEXT("Hit : %s"),
        *GetNameSafe(OtherActor)
    );

    ProjectileMovement->Bounciness =
        FMath::Clamp(
            ProjectileMovement->Bounciness - 0.2f,
            0.0f,
            1.0f
        );

    if (!bIsTouchObject)
    {
        switch (ThrowType)
        {
        case EThrowItemType::MINE:
            FireMine();
            break;

        case EThrowItemType::SOUND:
            FireSound();
            break;

        case EThrowItemType::SMOKE:
            FireSmoke();
            break;

        case EThrowItemType::FIREWALL:
            CreateFireWall();
            break;
        }

        bIsTouchObject = true;

        
    }
    
    GetWorldTimerManager().SetTimer(
        TouchFlagTimerHandle,
        this,
        &AThrowItem::TouchObject,
        0.1f,
        false
    );
    

    // 例:
    // 投擲終了フラグ
    bIsThrown = false;

}

void AThrowItem::TouchObject()
{
    bIsTouchObject = false;
}

//void AThrowItem::OnProjectileBounce(
//    const FHitResult& ImpactResult,
//    const FVector& ImpactVelocity
//)
//{
//    UE_LOG(LogTemp, Warning,
//        TEXT("Bounce! Hit Actor : %s"),
//        *GetNameSafe(ImpactResult.GetActor())
//    );
//
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
//
//    case EThrowItemType::FIREWALL:
//        CreateFireWall();
//        break;
//    }
//}
//
//void AThrowItem::OnHit(UPrimitiveComponent* HitComp, AActor* OtherActor,
//    UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit)
//{
//    UE_LOG(LogTemp, Warning, TEXT("Hit Actor: %s (comp: %s)"),
//        *OtherActor->GetName(),
//        *OtherComp->GetName());
//
//    if (OtherActor && OtherActor != this)
//    {
//        UE_LOG(LogTemp, Warning, TEXT("Hit!!"));
//
//      
//
//
//        Destroy();
//    }
//}

void AThrowItem::SearchMine()
{



}

void AThrowItem::FireMine()
{



}

void AThrowItem::FireSound()
{
    UE_LOG(LogTemp, Warning, TEXT("Sound Fire"));

    UAISense_Hearing::ReportNoiseEvent(
        GetWorld(),
        GetActorLocation(),
        ItemSoundLoudness,
        this,
        ItemSoundRange,
        FName("GunShot")
    );
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



