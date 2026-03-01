// Fill out your copyright notice in the Description page of Project Settings.


#include "EnemyAIController.h"

#include "Perception/AIPerceptionComponent.h"
#include "Perception/AISenseConfig_Sight.h"
#include"Perception/AISenseConfig_Hearing.h"
#include "DrawDebugHelpers.h"
#include "Kismet/KismetSystemLibrary.h"
#include "EnemyCharacter.h"

AEnemyAIController::AEnemyAIController()
{
    PrimaryActorTick.bCanEverTick = true;

    PerceptionComp = CreateDefaultSubobject<UAIPerceptionComponent>(TEXT("PerceptionComp"));
    SetPerceptionComponent(*PerceptionComp);

    SightConfig = CreateDefaultSubobject<UAISenseConfig_Sight>(TEXT("SightConfig"));
    HearingConfig = CreateDefaultSubobject<UAISenseConfig_Hearing>(TEXT("HearingConfig"));

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
    //視界
    SightConfig->SightRadius = SightRadius;
    SightConfig->LoseSightRadius = LoseSightRadius;
    SightConfig->PeripheralVisionAngleDegrees = PeripheralVisionAngle;

    SightConfig->SetMaxAge(3.f);

    SightConfig->DetectionByAffiliation.bDetectEnemies = true;
    SightConfig->DetectionByAffiliation.bDetectNeutrals = true;
    SightConfig->DetectionByAffiliation.bDetectFriendlies = true;
   

    //聴覚
    HearingConfig->HearingRange = 2000.f;
    HearingConfig->SetMaxAge(2.f);

    HearingConfig->DetectionByAffiliation.bDetectEnemies = true;
    HearingConfig->DetectionByAffiliation.bDetectNeutrals = true;
    HearingConfig->DetectionByAffiliation.bDetectFriendlies = true;

    PerceptionComp->ConfigureSense(*HearingConfig);

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
        UpdateInterval = UpdateIntevalTime [0];
    else if (Distance < SightRadius*2)
        UpdateInterval = UpdateIntevalTime[1];
    else
        UpdateInterval = UpdateIntevalTime[2];

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

    AEnemyCharacter* EnemyChar = Cast<AEnemyCharacter>(GetPawn());

    switch (CurrentState)
    {
    case EEnemyState::Idle:
      

        //if (bCurrentlySeeingTarget && TargetActor)
        //{
        //    float Distance = FVector::Dist(
        //        GetPawn()->GetActorLocation(),
        //        TargetActor->GetActorLocation()
        //    );

        //    // 距離を0〜1に正規化
        //    float Alpha = FMath::Clamp(
        //        (Distance - MinDetectDistance) /
        //        (MaxDetectDistance - MinDetectDistance),
        //        0.f,
        //        1.f
        //    );

        //    // 距離が近いほどDetectSpeedが高くなる
        //    float DetectSpeed = FMath::Lerp(
        //        MinDetectSpeed,
        //        MaxDetectSpeed,
        //        Alpha
        //    );

        //    DetectionValue += DetectSpeed * UpdateInterval;

        //    if (DetectionValue >= DetectionThreshold)
        //    {
        //        CurrentState = EEnemyState::Chase;
        //        MoveToActor(TargetActor, 100.f);
        //    }
        //}
        //else

        /*if (bCurrentlySeeingTarget && TargetActor)
        {
            CurrentState = EEnemyState::Caution;

            if (EnemyChar)
                EnemyChar->SetMoveState(EEnemyMoveState::Caution);
        }*/

        {
            DetectionValue = FMath::Max(
                0.f,
                DetectionValue - DetectionDecreaseAmount * UpdateInterval*3
            );
        }

      /*  float DistSq = FVector::DistSquared(
            GetPawn()->GetActorLocation(),
            TargetActor->GetActorLocation()
        );

        const float ChaseRangeSq = 1000.f * 1000.f;

        if (DistSq < ChaseRangeSq)
        {
            DetectionValue += 30.f * UpdateInterval;
        }

        if (DetectionValue >= DetectionThreshold)
        {
            CurrentState = EEnemyState::Chase;
            MoveToActor(TargetActor, 100.f);
        }*/

        break;
    case EEnemyState::StandBy:

        UE_LOG(LogTemp, Warning, TEXT("%s: StandBy"), *GetName());

        if (bIsAlertWaiting)
        {
            AlertWaitTimer += UpdateInterval;

            if (AlertWaitTimer >= AlertWaitTime)
            {
                bIsAlertWaiting = false;

                // ★ 視界にいなくても必ず調査へ
                CurrentState = EEnemyState::Investigate;
                bHasStoredFirstDetectLocation = false;
                UE_LOG(LogTemp, Warning, TEXT("%s: StandBy -> Investigate"), *GetName());
            }
            else
            {
                return; // 停止継続
            }
        }

    break;

    case EEnemyState::Investigate:

      // UE_LOG(LogTemp, Warning, TEXT("%s: Investigating"), *GetName());

        if (/*CurrentState != EEnemyState::Chase && */!bHasStoredFirstDetectLocation)
        {
            // ★ 移動開始（1回だけ呼ぶように制御してもOK）
            MoveToLocation(FirstDetectLocation);
            bHasStoredFirstDetectLocation = true;
        }
       
       

        // 距離チェック
        {
            float Distance = FVector::Dist(
                GetPawn()->GetActorLocation(),
                FirstDetectLocation
            );

            // 再視認したらゲージ増加
            if (bCurrentlySeeingTarget)
            {
                DetectionValueAdd();
               // UE_LOG(LogTemp, Warning, TEXT("%s: Investigate -> Chase"), *GetName());
                //break;
            }

            // ★ 到達したらCautionへ
            if (Distance < 100.f)
            {
                CurrentState = EEnemyState::Caution;
                UE_LOG(LogTemp, Warning, TEXT("%s: Investigate -> Caution"), *GetName());
            }
        }

        break;

    case EEnemyState::Caution:

       

        if (bCurrentlySeeingTarget && TargetActor)
        {
            DetectionValueAdd();

            if (DetectionValue >= DetectionThreshold)//発見ゲージが最大までたまったら
            {
                if (CurrentState != EEnemyState::Chase)
                {
                    CurrentState = EEnemyState::Chase;

                    bIsFirstDiscoverer = true;

                    if (bIsFirstDiscoverer)
                    {
                        AlertNearbyAllies();
                    }

                }

                //CurrentState = EEnemyState::Chase;
               
            }
            else
            {
              

                MoveToLocation(FirstDetectLocation);
            }


            if (bIsScanning)
            {
                // ★ スキャン中に再視認したらChase
                if (bCurrentlySeeingTarget && TargetActor)
                {
                    /* CurrentState = EEnemyState::Chase;

                     if (EnemyChar)
                         EnemyChar->SetMoveState(EEnemyMoveState::Chase);

                     MoveToActor(TargetActor, 100.f);*/

                    UE_LOG(LogTemp, Warning, TEXT("%s:dokoniirunda..?"), *GetName());

                    bHasStoredFirstDetectLocation = false;

                  

                    if (/*CurrentState != EEnemyState::Chase && */!bHasStoredFirstDetectLocation)
                    {
                        FirstDetectLocation = TargetActor->GetActorLocation();
                       
                        UE_LOG(LogTemp, Warning, TEXT("%s:sokoniitaka!."), *GetName());
                    }

                   // MoveToLocation(FirstDetectLocation);

                    bIsScanning = false;

                    CurrentState = EEnemyState::StandBy;

                    break;
                }

            }

        }
        else
        {
            DetectionValue = FMath::Max(
                0.f,
                DetectionValue - DetectionDecreaseAmount * UpdateInterval * 2
            );

           
           

            if (!bIsScanning)//スキャンしてないなら
            {
                MoveToLocation(FirstDetectLocation);

                float Distance = FVector::Dist(
                    GetPawn()->GetActorLocation(),
                    FirstDetectLocation
                );

                if (Distance < 50.f)
                {
                    StopMovement();

                    bIsScanning = true;
                    ScanTimer = 0.f;

                    ScanBaseYaw = GetPawn()->GetActorRotation().Yaw;
                    ScanCurrentYaw = ScanBaseYaw;
                    ScanDirection = 1;

                }
            }
            else
            {
                // スキャン処理
                ScanTimer += UpdateInterval;

                ScanCurrentYaw += ScanDirection * ScanSpeed * UpdateInterval;

                // 上限チェック
                if (ScanCurrentYaw >= ScanBaseYaw + ScanRange)
                {
                    ScanCurrentYaw = ScanBaseYaw + ScanRange;
                    ScanDirection = -1;
                }
                else if (ScanCurrentYaw <= ScanBaseYaw - ScanRange)
                {
                    ScanCurrentYaw = ScanBaseYaw - ScanRange;
                    ScanDirection = 1;
                }

                APawn* MyPawn = GetPawn();
                if (!MyPawn) return;

                FRotator NewRot = GetPawn()->GetActorRotation();
                NewRot.Yaw = ScanCurrentYaw;
                GetPawn()->SetActorRotation(NewRot);
                MyPawn->GetController()->SetControlRotation(NewRot);

               

              

                // スキャン終了
                if (ScanTimer >= ScanDuration)
                {
                    CurrentState = EEnemyState::Idle;

                    if (EnemyChar)
                        EnemyChar->SetMoveState(EEnemyMoveState::Idle);

                    bHasStoredFirstDetectLocation = false;
                    bIsScanning = false;
                }
            }


            if (DetectionValue<=0.0f)
            {
                CurrentState = EEnemyState::Idle;

                if (EnemyChar)
                    EnemyChar->SetMoveState(EEnemyMoveState::Idle);
                // リセット
                bHasStoredFirstDetectLocation = false;
              //  bLockLastKnownLocation = false;
            }

        }



        break;

    case EEnemyState::Chase:
        if (!bCurrentlySeeingTarget)
        {
            CurrentState = EEnemyState::Caution;

            if (EnemyChar)
                EnemyChar->SetMoveState(EEnemyMoveState::Caution);


            //StopMovement();
            MoveToLocation(LastKnownLocation);
            bIsScanning = false;
        }
        else
        {
            if (EnemyChar)
                EnemyChar->SetMoveState(EEnemyMoveState::Chase);
            MoveToActor(TargetActor, 100.f);
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
    //APlayerCharacter* Player = Cast<APlayerCharacter>(Actor);
    //if (!Player) return;  // プレイヤー以外は無視
    if (Stimulus.Type == UAISense::GetSenseID<UAISense_Sight>()/*&&Actor==Player*/)
    {
        if (Stimulus.WasSuccessfullySensed())
        {
            if (Actor->ActorHasTag("Player"))
            {
                bCurrentlySeeingTarget = true;
                TargetActor = Actor; 
               LastKnownLocation = Actor->GetActorLocation();
               //LockedRotation = Actor->GetActorRotation();

               if (CurrentState == EEnemyState::Idle|| CurrentState == EEnemyState::Caution)
               {
                   

                   CurrentState = EEnemyState::StandBy;

                   bIsAlertWaiting = true;
                   AlertWaitTimer = 0.f;

                   FirstDetectLocation = Actor->GetActorLocation();
                   bHasStoredFirstDetectLocation = true;

                   StopMovement();

                  
               }

                // ★ Cautionでまだ保存していないなら保存
               if (CurrentState != EEnemyState::Chase && !bHasStoredFirstDetectLocation)
               {
                   FirstDetectLocation = Actor->GetActorLocation();
                   bHasStoredFirstDetectLocation = true;
               }

               /* if (!bLockLastKnownLocation)
                {
                    LastKnownLocation = Actor->GetActorLocation();
                }*/

              // CurrentState = EEnemyState::Caution;
            }

            // プレイヤー発見 → 追跡
           // bCurrentlySeeingTarget = true;
            //TargetActor = Actor;
            //TargetActor = Player;
            UE_LOG(LogTemp, Warning, TEXT("%s:AI initialized successfully."), *Actor->GetName());
        }
        else
        {
            // 見失った
            if (Actor == TargetActor)
            {
                bCurrentlySeeingTarget = false;
                // ★ ここでロック開始
              //  bLockLastKnownLocation = true;
                LastKnownLocation = Stimulus.StimulusLocation;
                TargetActor = nullptr;
            }
          //  bCurrentlySeeingTarget = false;
            UE_LOG(LogTemp, Warning, TEXT("AI initialized faied."));
        }
    }
    // ===== 聴覚 =====
    else if (Stimulus.Type == UAISense::GetSenseID<UAISense_Hearing>())
    {
        if (Stimulus.WasSuccessfullySensed())
        {
            FVector SoundLocation = Stimulus.StimulusLocation;

            // 音源へ移動
            MoveToLocation(SoundLocation);

            CurrentState = EEnemyState::Caution;
        }
    }

}

void AEnemyAIController::OnAlertedByAlly(AActor* PlayerActor)
{
    if (CurrentState == EEnemyState::Chase) return;

    TargetActor = PlayerActor;
    CurrentState = EEnemyState::Chase;
    DetectionValue = DetectionThreshold;
    bIsFirstDiscoverer = false; // 第一発見者以外の連鎖防止
}

void AEnemyAIController::AlertNearbyAllies()
{
    if (!bIsFirstDiscoverer) return;
    if (bHasAlertedAllies) return;

    bHasAlertedAllies = true;

    TArray<AActor*> OverlappedActors;

    TArray<TEnumAsByte<EObjectTypeQuery>> ObjectTypes;
    ObjectTypes.Add(UEngineTypes::ConvertToObjectType(ECC_Pawn));

    TArray<AActor*> IgnoreActors;
    IgnoreActors.Add(GetPawn());

    UKismetSystemLibrary::SphereOverlapActors(
        GetWorld(),
        GetPawn()->GetActorLocation(),
        AlertRadius,
        ObjectTypes,
        AEnemyCharacter::StaticClass(),
        IgnoreActors,
        OverlappedActors
    );

    for (AActor* Actor : OverlappedActors)
    {
        AEnemyCharacter* OtherEnemy = Cast<AEnemyCharacter>(Actor);
        if (!OtherEnemy) continue;

        AEnemyAIController* OtherAI =
            Cast<AEnemyAIController>(OtherEnemy->GetController());

        if (!OtherAI) continue;

        OtherAI->OnAlertedByAlly(TargetActor);
    }
}

float AEnemyAIController::GetDetectionPercent() const
{
    if (DetectionThreshold <= 0.f) return 0.f;
    return DetectionValue / DetectionThreshold;
}

void AEnemyAIController::DetectionValueAdd()
{

    {//絶対関数化してやる
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
    }

}