#include "SmokeGrenadeActor.h"
#include "EnemyCharacter.h"
#include "EnemyAIController.h"

ASmokeGrenadeActor::ASmokeGrenadeActor()
{
    PrimaryActorTick.bCanEverTick = false;

    SmokeSphere =
        CreateDefaultSubobject<USphereComponent>(
            TEXT("SmokeSphere"));

    RootComponent = SmokeSphere;

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

    SmokeSphere->OnComponentBeginOverlap.AddDynamic(
        this,
        &ASmokeGrenadeActor::OnSmokeBeginOverlap
    );

    SmokeSphere->OnComponentEndOverlap.AddDynamic(
        this,
        &ASmokeGrenadeActor::OnSmokeEndOverlap
    );

    SetLifeSpan(SmokeLifeTime);
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

    AEnemyAIController* AI =
        Cast<AEnemyAIController>(Enemy->GetController());

    if (!AI)
    {
        return;
    }

    AI->bInSmoke = true;
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

    AEnemyAIController* AI =
        Cast<AEnemyAIController>(Enemy->GetController());

    if (!AI)
    {
        return;
    }

    AI->bInSmoke = false;
}