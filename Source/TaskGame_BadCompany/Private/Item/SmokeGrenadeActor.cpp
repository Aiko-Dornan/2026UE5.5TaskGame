#include "SmokeGrenadeActor.h"
#include "EnemyCharacter.h"
#include "EnemyAIController.h"

ASmokeGrenadeActor::ASmokeGrenadeActor()
{
    PrimaryActorTick.bCanEverTick = false;

    SmokeSphere =
        CreateDefaultSubobject<USphereComponent>(
            TEXT("SmokeSphere"));

    //RootComponent = SmokeSphere;

    SmokeSphere->SetupAttachment(RootComponent);

    SmokeSphere->SetSphereRadius(SmokeRadius);

    SmokeSphere->SetCollisionEnabled(
        ECollisionEnabled::QueryOnly);

    SmokeSphere->SetCollisionResponseToAllChannels(
        ECR_Ignore);

    SmokeSphere->SetCollisionResponseToChannel(
        ECC_Pawn,
        ECR_Overlap);
}

void ASmokeGrenadeActor::BeginPlay()
{
    Super::BeginPlay();

}

void ASmokeGrenadeActor::OnThrowItemHit(
    const FHitResult& Hit)
{
    Super::OnThrowItemHit(Hit);

    ActivateSmoke();
}

void ASmokeGrenadeActor::OnSmokeBeginOverlap(
    UPrimitiveComponent* OverlappedComp,
    AActor* OtherActor,
    UPrimitiveComponent* OtherComp,
    int32 OtherBodyIndex,
    bool bFromSweep,
    const FHitResult& SweepResult
)
{
    AEnemyCharacter* Enemy =
        Cast<AEnemyCharacter>(OtherActor);

    if (!Enemy)
    {
        return;
    }

    //TArray<AActor*> OverlapActors;

    //UKismetSystemLibrary::SphereOverlapActors(
    //    GetWorld(),
    //    GetActorLocation(),
    //    SmokeRadius,
    //    TArray<TEnumAsByte<EObjectTypeQuery>>(),
    //    AEnemyCharacter::StaticClass(),
    //    TArray<AActor*>(),
    //    OverlapActors
    //);

    //for (AActor* Actor : OverlapActors)
    //{
    //   /* AEnemyCharacter* Enemy2 =
    //        Cast<AEnemyCharacter>(Actor);*/

    //    if (Enemy)
    //    {
    //        Enemy->SetPreviousState();

    //        AEnemyAIController* AI =
    //            Cast<AEnemyAIController>(Enemy->GetController());

    //        if (!AI)
    //        {
    //            return;
    //        }
    //        AI->FirstDetectLocation = GetActorLocation();
    //        AI->bInSmoke = true;
    //        
    //        UE_LOG(LogTemp, Warning, TEXT("%s: Smoke"), *Enemy->GetName());
    //    }
    //}

    Enemy->SetPreviousState();

    AEnemyAIController* AI =
        Cast<AEnemyAIController>(Enemy->GetController());

    if (!AI)
    {
        return;
    }

    SetSearchPoint(Enemy);

    /*AI->FirstDetectLocation = Enemy->GetActorLocation();
    AI->FirstDetectLocation.Z = Enemy->GetActorLocation().Z;
    AI->bInSmoke = true;*/
    UE_LOG(LogTemp, Warning, TEXT("%s: Smoke"), *Enemy->GetName());
    
}

void ASmokeGrenadeActor::OnSmokeEndOverlap(
    UPrimitiveComponent* OverlappedComp,
    AActor* OtherActor,
    UPrimitiveComponent* OtherComp,
    int32 OtherBodyIndex
)
{
    AEnemyCharacter* Enemy =
        Cast<AEnemyCharacter>(OtherActor);

    if (!Enemy)
    {
        return;
    }

    Enemy->SetMoveState(Enemy->PreviousMoveState);

    AEnemyAIController* AI =
        Cast<AEnemyAIController>(Enemy->GetController());

    if (!AI)
    {
        return;
    }

    AI->bInSmoke = false;

   
    
        if (Enemy->PreviousMoveState == EEnemyMoveState::Idle)
        {
            AI->MoveToNextPatrolPoint();
        }
    

}

void ASmokeGrenadeActor::ActivateSmoke()
{
    if (bSmokeActivated)
    {
        return;
    }

    bSmokeActivated = true;

    SmokeSphere->SetCollisionEnabled(
        ECollisionEnabled::QueryOnly);

    SmokeSphere->OnComponentBeginOverlap.AddDynamic(
        this,
        &ASmokeGrenadeActor::OnSmokeBeginOverlap
    );

    SmokeSphere->OnComponentEndOverlap.AddDynamic(
        this,
        &ASmokeGrenadeActor::OnSmokeEndOverlap
    );

    // Šù‚É’†‚É‚¢‚é“GŽæ“¾
    TArray<AActor*> OverlapActors;

    UKismetSystemLibrary::SphereOverlapActors(
        GetWorld(),
        GetActorLocation(),
        SmokeRadius,
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

        OnSmokeBeginOverlap(
            SmokeSphere,
            Enemy,
            nullptr,
            0,
            false,
            FHitResult()
        );
    }

    SetLifeSpan(SmokeLifeTime);

    UE_LOG(LogTemp, Warning,
        TEXT("Smoke Activated"));
}

void ASmokeGrenadeActor::SetSearchPoint(AActor* Actor)
{

    AEnemyCharacter* Enemy =
        Cast<AEnemyCharacter>(Actor);

    if (!Enemy)
    {
        return;
    }

    AEnemyAIController* AI =
        Cast<AEnemyAIController>(
            Enemy->GetController());

    if (!AI)
    {
        return;
    }
    AI->bIsPatrolling = false;
    // AI->bIsAlertWaiting = true;
    // AI->AlertWaitTimer = 0.f;

     // ’²¸’n“_Ý’è
    AI->SearchCenter = Enemy->GetActorLocation();
    AI->SearchCenter.Z = Enemy->GetActorLocation().Z;

    AI->FirstDetectLocation = AI->SearchCenter;



    AI->CurrentState = EEnemyState::Search;

    AI->bIsArrivedSearchLocation = false;
    AI->bHasStoredFirstDetectLocation = false;
    AI->bIsScanning = false;

    AI->SearchTimer = 0.f;
    AI->bInSmoke = true;

}