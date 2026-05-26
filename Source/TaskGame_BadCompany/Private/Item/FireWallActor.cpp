// Fill out your copyright notice in the Description page of Project Settings.


#include "Item/FireWallActor.h"
#include "Components/BoxComponent.h"
#include "Components/StaticMeshComponent.h"
#include "NavModifierComponent.h"
#include "NavAreas/NavArea_Null.h"

AFireWallActor::AFireWallActor()
{
    PrimaryActorTick.bCanEverTick = false;

    WallCollision =
        CreateDefaultSubobject<UBoxComponent>(
            TEXT("WallCollision"));

    //RootComponent = WallCollision;
    WallCollision->SetupAttachment(RootComponent);

    WallMesh =
        CreateDefaultSubobject<UStaticMeshComponent>(
            TEXT("WallMesh"));

    NavModifier =
        CreateDefaultSubobject<UNavModifierComponent>(
            TEXT("NavModifier"));

    WallMesh->SetupAttachment(RootComponent);

    WallCollision->SetCollisionEnabled(
        ECollisionEnabled::QueryOnly);

    WallCollision->SetCollisionResponseToAllChannels(
        ECR_Ignore);

    WallCollision->SetCollisionResponseToChannel(
        ECC_Pawn,
        ECR_Overlap);

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

    ActivateFireWall();
}

void AFireWallActor::ActivateFireWall()
{

    if (bIsActivatedFire)
    {
        return;
    }

    bIsActivatedFire = true;

    Mesh = nullptr;

    WallCollision->SetCollisionEnabled(
        ECollisionEnabled::QueryOnly);

    WallCollision->SetCollisionEnabled(
        ECollisionEnabled::QueryAndPhysics);

    WallCollision->SetCollisionResponseToAllChannels(
        ECR_Block);

    // サイズ
    WallCollision->SetBoxExtent(
        FVector(50.f, 300.f, 200.f));

    WallCollision->SetCanEverAffectNavigation(true);

    NavModifier->SetAreaClass(
        UNavArea_Null::StaticClass());

    SetLifeSpan(LifeTime);

    UE_LOG(LogTemp, Warning,
        TEXT("Fire KieyukuInoti")
    );
}
