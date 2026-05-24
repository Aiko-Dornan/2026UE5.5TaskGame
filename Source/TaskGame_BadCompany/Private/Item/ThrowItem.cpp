// Fill out your copyright notice in the Description page of Project Settings.


#include "Item/ThrowItem.h"
#include "Kismet/GameplayStatics.h"
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

    if (ThrowType == EThrowItemType::MINE)
    {
        SearchSphere =
            CreateDefaultSubobject<USphereComponent>(
                TEXT("SearchSphere")
            );

        SearchSphere->SetupAttachment(RootComponent);

        SearchSphere->SetSphereRadius(SearchRadius);

        /*SearchSphere->SetCollisionEnabled(
            ECollisionEnabled::QueryOnly
        );*/
    }
    else
    {
        /*SearchSphere->SetCollisionEnabled(
            ECollisionEnabled::NoCollision
        );*/
    }

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
            if (bMineArmed)
            {
                FireMine();
            }
            
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

void AThrowItem::OnSearchSphereOverlap(
    UPrimitiveComponent* OverlappedComponent,
    AActor* OtherActor,
    UPrimitiveComponent* OtherComp,
    int32 OtherBodyIndex,
    bool bFromSweep,
    const FHitResult& SweepResult
)
{
    if (!OtherActor) return;

    AEnemyCharacter* Enemy =
        Cast<AEnemyCharacter>(OtherActor);

    if (Enemy)
    {
        UE_LOG(LogTemp, Warning,
            TEXT("Enemy Found : %s"),
            *Enemy->GetName()
        );
        bMineArmed = true;
        // ここで好きな処理
        
    }
}

void AThrowItem::SearchMine()
{
    TArray<AActor*> OverlappedActors;

    TArray<TEnumAsByte<EObjectTypeQuery>> ObjectTypes;
    ObjectTypes.Add(
        UEngineTypes::ConvertToObjectType(ECC_Pawn)
    );

    TArray<AActor*> IgnoreActors;
    IgnoreActors.Add(this);

    UKismetSystemLibrary::SphereOverlapActors(
        GetWorld(),
        GetActorLocation(),
        SearchRadius,
        ObjectTypes,
        AEnemyCharacter::StaticClass(),
        IgnoreActors,
        OverlappedActors
    );

    for (AActor* Actor : OverlappedActors)
    {
        AEnemyCharacter* Enemy =
            Cast<AEnemyCharacter>(Actor);

        if (Enemy)
        {
            UE_LOG(LogTemp, Warning,
                TEXT("Mine Detect Enemy : %s"),
                *Enemy->GetName()
            );
        }
    }


}

void AThrowItem::FireMine()
{

    UE_LOG(LogTemp, Warning,
        TEXT("Mine Exprosion!!"));

    Destroy();

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



