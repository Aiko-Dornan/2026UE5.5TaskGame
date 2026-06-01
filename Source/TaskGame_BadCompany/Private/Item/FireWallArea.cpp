// Fill out your copyright notice in the Description page of Project Settings.


#include "Item/FireWallArea.h"

#include "Components/BoxComponent.h"
#include "NavModifierComponent.h"
#include "NavAreas/NavArea_Null.h"


AFireWallArea::AFireWallArea()
{
    PrimaryActorTick.bCanEverTick = false;

    NavBlockBox =
        CreateDefaultSubobject<UBoxComponent>(
            TEXT("NavBlockBox"));

    RootComponent = NavBlockBox;

    NavBlockBox->SetBoxExtent(
        FVector(300.f, 300.f, 200.f));

    NavBlockBox->SetCollisionEnabled(
        ECollisionEnabled::NoCollision);

    NavBlockBox->SetCanEverAffectNavigation(true);

    NavModifier =
        CreateDefaultSubobject<UNavModifierComponent>(
            TEXT("NavModifier"));

    NavModifier->SetAreaClass(
        UNavArea_Null::StaticClass());
}

void AFireWallArea::BeginPlay()
{
    Super::BeginPlay();

    TArray<AActor*> OverlapActors;

    UKismetSystemLibrary::BoxOverlapActors(
        GetWorld(),
        GetActorLocation(),
        FVector(300.f, 300.f, 200.f),
        TArray<TEnumAsByte<EObjectTypeQuery>>(),
        AEnemyCharacter::StaticClass(),
        TArray<AActor*>(),
        OverlapActors
    );

    for (AActor* Actor : OverlapActors)
    {
        AEnemyCharacter* Enemy =
            Cast<AEnemyCharacter>(Actor);

        if (!Enemy)
        {
            continue;
        }

        AEnemyAIController* AI =
            Cast<AEnemyAIController>(
                Enemy->GetController());

        if (!AI)
        {
            continue;
        }

        AI->StopMovement();

        StoppedEnemies.Add(AI);
    }

    SetLifeSpan(LifeTime);
}

// Called every frame
void AFireWallArea::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void AFireWallArea::EndPlay(
    const EEndPlayReason::Type EndPlayReason)
{
    Super::EndPlay(EndPlayReason);

    RestoreEnemyMovement();
}

void AFireWallArea::RestoreEnemyMovement()
{
    for (AEnemyAIController* AI : StoppedEnemies)
    {
        if (!IsValid(AI))
        {
            continue;
        }

        AI->ResumeMovement();
    }
}