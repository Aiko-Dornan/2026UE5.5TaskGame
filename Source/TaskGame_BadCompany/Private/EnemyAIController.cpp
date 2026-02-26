// Fill out your copyright notice in the Description page of Project Settings.


#include "EnemyAIController.h"

#include "Perception/AIPerceptionComponent.h"
#include "Perception/AISenseConfig_Sight.h"
#include "DrawDebugHelpers.h"
#include"EnemyCharacter.h"

AEnemyAIController::AEnemyAIController()
{
    PrimaryActorTick.bCanEverTick = true;

    PerceptionComp = CreateDefaultSubobject<UAIPerceptionComponent>(TEXT("PerceptionComp"));
    SetPerceptionComponent(*PerceptionComp);

    SightConfig = CreateDefaultSubobject<UAISenseConfig_Sight>(TEXT("SightConfig"));


    PerceptionComp->OnTargetPerceptionUpdated.AddDynamic(
        this,
        &AEnemyAIController::OnTargetPerceptionUpdated
    );
   
}

void AEnemyAIController::BeginPlay()
{
    Super::BeginPlay();

    SetupPerception();

    AEnemyCharacter* Enemy = Cast<AEnemyCharacter>(GetPawn());
    if (Enemy)
    {
        SightRadius = Enemy->SightRadius;
        PeripheralVisionAngle = Enemy->PeripheralVisionAngle;
        LoseSightRadius = Enemy->LoseSightRadius;
        SightConfig->SetMaxAge(Enemy->SetMaxAge);
        DetectionThreshold = Enemy->DetectionThreshold;
    }

    // Tickを使わずタイマーで更新
    GetWorld()->GetTimerManager().SetTimer(
        AITickHandle,
        this,
        &AEnemyAIController::UpdateAI,
        UpdateInterval,
        true
    );

}

void AEnemyAIController::SetupPerception()
{
    SightConfig->SightRadius = SightRadius;
    SightConfig->LoseSightRadius = LoseSightRadius;
    SightConfig->PeripheralVisionAngleDegrees = PeripheralVisionAngle;

    SightConfig->SetMaxAge(3.f);

    SightConfig->DetectionByAffiliation.bDetectEnemies = true;
    SightConfig->DetectionByAffiliation.bDetectNeutrals = true;
    SightConfig->DetectionByAffiliation.bDetectFriendlies = true;

    PerceptionComp->ConfigureSense(*SightConfig);
    PerceptionComp->SetDominantSense(SightConfig->GetSenseImplementation());
    
    PerceptionComp->RequestStimuliListenerUpdate();//OnTargetPerceptionUpdatedの更新...らしい。
}

void AEnemyAIController::UpdateLOD()
{
    APawn* Player = UGameplayStatics::GetPlayerPawn(GetWorld(), 0);
    if (!Player || !GetPawn()) return;

    float Distance = FVector::Dist(Player->GetActorLocation(), GetPawn()->GetActorLocation());

    if (Distance < SightRadius)
        UpdateInterval = 0.05f;
    else if (Distance < SightRadius*2)
        UpdateInterval = 0.3f;
    else
        UpdateInterval = 1.0f;

    // タイマー更新
    GetWorld()->GetTimerManager().ClearTimer(AITickHandle);
    GetWorld()->GetTimerManager().SetTimer(
        AITickHandle,
        this,
        &AEnemyAIController::UpdateAI,
        UpdateInterval,
        true
    );
}

void AEnemyAIController::UpdateAI()
{
    UpdateLOD();

    switch (CurrentState)
    {
    case EEnemyState::Idle:
       /* if (bCurrentlySeeingTarget)
        {
            DetectionValue += 20.f;

            if (DetectionValue >= DetectionThreshold)
            {
                CurrentState = EEnemyState::Chase;
                MoveToActor(TargetActor);
            }
        }
        else
        {
            DetectionValue = FMath::Max(0.f, DetectionValue - 10.f);
        }*/

        if (bCurrentlySeeingTarget && TargetActor)
        {
            float Distance = FVector::Dist(
                GetPawn()->GetActorLocation(),
                TargetActor->GetActorLocation()
            );

            // 距離を0〜1に正規化
            float Alpha = FMath::Clamp(
                (Distance - MinDetectDistance) /
                (MaxDetectDistance - MinDetectDistance),
                0.f,
                1.f
            );

            // 距離が近いほどDetectSpeedが高くなる
            float DetectSpeed = FMath::Lerp(
                MinDetectSpeed,
                MaxDetectSpeed,
                Alpha
            );

            DetectionValue += DetectSpeed * UpdateInterval;

            if (DetectionValue >= DetectionThreshold)
            {
                CurrentState = EEnemyState::Chase;
                MoveToActor(TargetActor, 100.f);
            }
        }
        else
        {
            DetectionValue = FMath::Max(
                0.f,
                DetectionValue - DetectionDecreaseAmount * UpdateInterval
            );
        }

        break;

    case EEnemyState::Chase:
        if (!bCurrentlySeeingTarget)
        {
            CurrentState = EEnemyState::Idle;
            StopMovement();
        }
        break;
    }
}

void AEnemyAIController::Tick(float DeltaTime)
{
    Super::Tick(DeltaTime);

    if (!GetPawn()) return;

    //if (bCurrentlySeeingTarget && TargetActor)
    //{
    //    DetectionValue += DetectionSpeed * DeltaTime;
    //}
    //else
    //{
    //    DetectionValue -= LoseSpeed * DeltaTime;
    //}

    //DetectionValue = FMath::Clamp(DetectionValue, 0.f, DetectionThreshold);

    //// 一定値で追跡開始
    //if (DetectionValue >= DetectionThreshold && TargetActor)
    //{
    //    MoveToActor(TargetActor, 100.f);
    //}

    FVector Location = GetPawn()->GetActorLocation();
    FVector Forward = GetPawn()->GetActorForwardVector();

    DrawDebugCone(
        GetWorld(),
        Location,
        Forward,
        SightRadius,
        FMath::DegreesToRadians(PeripheralVisionAngle),
        FMath::DegreesToRadians(PeripheralVisionAngle),
        12,
        FColor::Green,
        false,
        -1.f,
        0,
        1.f
    );
}

void AEnemyAIController::OnPossess(APawn* InPawn)
{
    Super::OnPossess(InPawn);

    if (!InPawn) return;
}

void AEnemyAIController::OnTargetPerceptionUpdated(AActor* Actor, FAIStimulus Stimulus)
{
    if (!Actor) return;
    if (Stimulus.Type == UAISense::GetSenseID<UAISense_Sight>())
    {
        if (Stimulus.WasSuccessfullySensed())
        {
            // プレイヤー発見 → 追跡
            bCurrentlySeeingTarget = true;
            TargetActor = Actor;
            UE_LOG(LogTemp, Warning, TEXT("%s:AI initialized successfully."), *Actor->GetName());
        }
        else
        {
            // 見失った
            bCurrentlySeeingTarget = false;
            UE_LOG(LogTemp, Warning, TEXT("AI initialized faied."));
        }
    }
}
