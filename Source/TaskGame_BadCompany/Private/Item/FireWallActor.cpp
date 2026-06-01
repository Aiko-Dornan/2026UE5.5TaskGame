// Fill out your copyright notice in the Description page of Project Settings.


#include "Item/FireWallActor.h"
#include "Components/BoxComponent.h"
#include "Components/StaticMeshComponent.h"
#include "NavModifierComponent.h"
#include "NavAreas/NavArea_Null.h"
#include "NavAreas/NavArea_Obstacle.h"
#include"EnemyAIController.h"

AFireWallActor::AFireWallActor()
{
    PrimaryActorTick.bCanEverTick = false;

    WallCollision =
        CreateDefaultSubobject<UBoxComponent>(
            TEXT("WallCollision"));
    //RootComponent = WallCollision;
    
    WallCollision->SetupAttachment(RootComponent);
    NavModifier =
        CreateDefaultSubobject<UNavModifierComponent>(
            TEXT("NavModifier"));

    WallMesh =
        CreateDefaultSubobject<UStaticMeshComponent>(
            TEXT("WallMesh"));
    

    WallMesh->SetupAttachment(RootComponent);

    // サイズ
    
    WallCollision->SetCanEverAffectNavigation(false);
   

    /*WallCollision->SetCollisionResponseToAllChannels(
        ECR_Ignore);*/

   /* WallCollision->SetCollisionResponseToChannel(
        ECC_Pawn,
        ECR_Overlap);*/

    // 通行不可
    
}

void AFireWallActor::BeginPlay()
{
    Super::BeginPlay();

    
}

void AFireWallActor::OnThrowItemHit(
    const FHitResult& Hit)
{
    Super::OnThrowItemHit(Hit);

    UE_LOG(LogTemp, Warning,
        TEXT("Fire Activated")
    );

    if (!FireWallAreaClass)
    {
        return;
    }
   
    FVector SpawnLocation =
        Hit.ImpactPoint;

    FRotator SpawnRotation =
        GetActorRotation();

    GetWorld()->SpawnActor<AFireWallArea>(
        FireWallAreaClass,
        SpawnLocation,
        SpawnRotation
    );

    Destroy();

    //// 投擲物停止
    //Mesh->SetSimulatePhysics(false);
    //Mesh->SetPhysicsLinearVelocity(FVector::ZeroVector);
    //Mesh->SetPhysicsAngularVelocityInDegrees(FVector::ZeroVector);

    //ActivateFireWall();
}

//void AFireWallActor::EndPlay(
//    const EEndPlayReason::Type EndPlayReason)
//{
//    Super::EndPlay(EndPlayReason);
//
//    RestoreEnemyMovement();
//}

void AFireWallActor::ActivateFireWall()
{

    if (bIsActivatedFire)
    {
        return;
    }

    bIsActivatedFire = true;

    Mesh->SetVisibility(false);
   /* Mesh->SetCollisionEnabled(
        ECollisionEnabled::NoCollision);*/

    

    WallCollision->SetBoxExtent(
        FVector(300.f, 300.f, 200.f));

    WallCollision->SetCollisionEnabled(
        ECollisionEnabled::NoCollision);

    WallCollision->SetCanEverAffectNavigation(true);

   /* WallCollision->SetCollisionEnabled(
        ECollisionEnabled::QueryOnly);

    WallCollision->SetCollisionEnabled(
        ECollisionEnabled::QueryAndPhysics);*/

   /* WallCollision->SetCollisionResponseToAllChannels(
        ECR_Block);*/

   

    

    NavModifier->SetAreaClass(
        UNavArea_Null::StaticClass());

    SetLifeSpan(LifeTime);

    UE_LOG(LogTemp, Warning,
        TEXT("Fire KieyukuInoti")
    );
}


//void AFireWallActor::RestoreEnemyMovement()
//{
//    TArray<AActor*> Actors;
//
//    UGameplayStatics::GetAllActorsOfClass(
//        GetWorld(),
//        AEnemyCharacter::StaticClass(),
//        Actors);
//
//    for (AActor* Actor : Actors)
//    {
//        AEnemyCharacter* Enemy =
//            Cast<AEnemyCharacter>(Actor);
//
//        if (!Enemy)
//        {
//            continue;
//        }
//
//        AEnemyAIController* AI =
//            Cast<AEnemyAIController>(
//                Enemy->GetController());
//
//        if (!AI)
//        {
//            continue;
//        }
//
//        // 復帰処理
//        AI->ResumeMovement();
//    }
//}