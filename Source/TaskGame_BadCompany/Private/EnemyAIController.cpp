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
        PatrolPoints = Enemy->EnemyPatrolPoints;
        
        UE_LOG(LogTemp, Warning, TEXT("%s: PatrolPoints.Num():%d"), *GetName(),PatrolPoints.Num());
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

    //// 距離を0〜1に正規化
    //float Alpha = FMath::Clamp(
    //    (Distance - MinDetectDistance) /
    //    (MaxDetectDistance - MinDetectDistance),
    //    0.f,
    //    1.f
    //);

    //// 距離が近いほどDetectSpeedが高くなる
    //UpdateInterval = FMath::Lerp(
    //    MinDetectSpeed,
    //    MaxDetectSpeed,
    //    Alpha
    //);

    

    if (Distance < SightRadius)
        UpdateInterval = UpdateIntevalTime [0];
    else if (Distance < SightRadius*1.5)
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

    

    APlayerCharacter* Player =
        Cast<APlayerCharacter>(UGameplayStatics::GetPlayerPawn(GetWorld(), 0));

    if (!Player)
    {
        return;
    }

    if (Player)
    {
        // 視界
        SightSource.bIsActive = bCurrentlySeeingTarget;
        //SightSource.IncreaseRate = 20.f;

        // 立入禁止エリア
        RestrictedAreaSource.bIsActive =
            (Player->bIsInRestrictedArea);
        //bIsPlayerInTabooArea = Player->bIsInRestrictedArea;
        //RestrictedAreaSource.IncreaseRate = 40.f; // 強めに
    }

    /*if (RestrictedAreaSource.bIsActive)
    {
        bIsPlayerInTabooArea = true;
    }*/

    switch (CurrentState)
    {
    case EEnemyState::Idle://プレイヤーを発見していない時の挙動
      
        
        if (!bIsPatrolling && PatrolPoints.Num() >= 1)
        {
            StartPatrol();
            UE_LOG(LogTemp, Warning, TEXT("%s: Patroll Start"), *GetName());
        }

        

        if (bIsPatrolling)
        {
            ATargetPoint* CurrentPoint = PatrolPoints[CurrentPatrolIndex];
            
            if (CurrentPoint)
            {
                float Distance = FVector::Dist(
                    GetPawn()->GetActorLocation(),
                    CurrentPoint->GetActorLocation()
                );
                
                if (Distance <= PatrolAcceptanceRadius)
                {
                    UE_LOG(LogTemp, Warning, TEXT("%s: CurrentPatrolIndex:%d"), *GetName(), CurrentPatrolIndex);
                    // 次のポイントへ
                    CurrentPatrolIndex++;

                    if (CurrentPatrolIndex >= PatrolPoints.Num())
                    {
                        CurrentPatrolIndex = 0; // ループ
                    }

                    MoveToNextPatrolPoint();
                    break;
                }
            }
        }

        {
        DetectionValueDecrease(EnemyChar);
        }
        break;
    case EEnemyState::StandBy://プレイヤーを発見した時の待機状態

        
        if (RestrictedAreaSource.bIsActive)
        {
            bIsPlayerInTabooArea = true;
        }
       
        

        if (bIsPlayerInTabooArea||bIsDistrust)
        {
            

            if (bCurrentlySeeingTarget)
            {
                if (bIsSensingTerms)
                {
                    

                    //bIsPatrolling = false;
                    
                    if (!TargetActor) return;
                    FirstDetectLocation = TargetActor->GetActorLocation();
                    bHasStoredFirstDetectLocation = true;

                    bIsSensingTerms = false;
                }

                UpdateDetectionSources();
                //DetectionValueIncrease();
                // UE_LOG(LogTemp, Warning, TEXT("%s: Investigate -> Chase"), *GetName());
                 //break;
            }
            else
            {
                DetectionValueDecrease(EnemyChar);
            }

            if (bIsAlertWaiting)
            {

                AlertWaitTimer += UpdateInterval;

                if (AlertWaitTimer >= AlertWaitTime)
                {
                    bIsAlertWaiting = false;

                    //視界にいなくても必ず調査へ
                    CurrentState = EEnemyState::Investigate;
                    bHasStoredFirstDetectLocation = false;
                    bIsScanning = false;
                    ScanTimer = 0.f;
                    UE_LOG(LogTemp, Warning, TEXT("%s: StandBy -> Investigate"), *GetName());
                }
                else
                {
                    return; // 停止継続
                }

            }
        }
        else
        {
            //DetectionValueDecrease(EnemyChar);
            /*if (DetectionValue<=0.0f)
            {
                CurrentState = EEnemyState::Idle; 
                bIsPatrolling = true;
                UE_LOG(LogTemp, Warning, TEXT("%s: StandBy -> Idle"), *GetName());
            }*/

        }
        
       // UE_LOG(LogTemp, Warning, TEXT("%s: StandBy"), *GetName());

       

    break;

    case EEnemyState::Investigate://プレイヤーを発見した位置まで移動する状態

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
                UpdateDetectionSources();
                //DetectionValueIncrease();
               // UE_LOG(LogTemp, Warning, TEXT("%s: Investigate -> Chase"), *GetName());
                //break;
            }
            else
            {
                DetectionValueDecrease(EnemyChar);

               
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

        EnemyScanAround(EnemyChar);

        if (bCurrentlySeeingTarget && TargetActor)
        {
            //DetectionValueIncrease();
            UpdateDetectionSources();
           
            {
                MoveToLocation(FirstDetectLocation);
            }

        }
        else
        {
            DetectionValueDecrease(EnemyChar);
 
  
        }



        break;

    case EEnemyState::Search://ランダムな位置に移動する状態
    {
        

        SearchTimer += UpdateInterval;
        
        

       /* if (bIsScanning)
        {
            EnemyScanAround(EnemyChar);
            break;
        }*/

        // 一定時間ごとにランダム地点へ
        if (!bIsArrivedSearchLocation/*GetMoveStatus() != EPathFollowingStatus::Moving*/)
        {
            UNavigationSystemV1* NavSys =
                UNavigationSystemV1::GetCurrent(GetWorld());

            if (NavSys)
            {
                FNavLocation RandomLoc;


                if (NavSys->GetRandomReachablePointInRadius(
                    SearchCenter,
                    SearchRadius,
                    RandomLoc))
                {

                    
                    RandomMoveLocation = RandomLoc.Location;
                    /*float Dist = FVector::Dist(
                        GetPawn()->GetActorLocation(),
                        RandomLoc.Location);
                    UE_LOG(LogTemp, Warning, TEXT("DistanceToGoal: %f"), Dist);

                    if (Dist > 300.f)
                    {
                        RandomMoveLocation = RandomLoc.Location;
                        MoveToLocation(RandomMoveLocation);
                    }*/
                    StopMovement();
                    
                   
                    bIsArrivedSearchLocation = true;
                    UE_LOG(LogTemp, Warning, TEXT("%s,RML:%lf,%lf"), *GetName(), RandomMoveLocation.X, RandomMoveLocation.Y);
                    UE_LOG(LogTemp, Warning, TEXT("%s: Idoutyuu"), *GetName());
                }
            }
            

            EPathFollowingRequestResult::Type Result = MoveToLocation(RandomMoveLocation);

            UE_LOG(LogTemp, Warning, TEXT("MoveResult: %d"), (int32)Result);

            // 探索時間終了ただし感知ゲージがまだ残っているなら
            if (SearchTimer >= SearchDuration&&DetectionValue>0.0f)
            {

                CurrentState = EEnemyState::Investigate;
                //bHasStoredFirstDetectLocation = false;
                //FirstDetectLocation = RandomMoveLocation;
            }

        }
        else
        {
            MoveToLocation(RandomMoveLocation, 10.f);

            float Distance =  FVector::Dist(
                GetPawn()->GetActorLocation(),
                RandomMoveLocation
            );

            if (Distance<30.0f)
            {
               // bIsArrivedSearchLocation = false;
                FirstDetectLocation = RandomMoveLocation;
                bIsScanning = false;
               // EnemyScanAround(EnemyChar);
                CurrentState = EEnemyState::Caution;
                UE_LOG(LogTemp, Warning, TEXT("%s: IdouKanryou"), *GetName());
                break;
            }
        }

        // 再発見したらゲージが増える
        if (bCurrentlySeeingTarget)
        {
            UpdateDetectionSources();
           
        }
        else
        {
            DetectionValueDecrease(EnemyChar); 
        }
        

        break;
    }

    case EEnemyState::Chase://プレイヤーを追いかける状態
        if (!bCurrentlySeeingTarget)
        {
            UE_LOG(LogTemp, Warning, TEXT("%s:Target Lost"), *GetName());
            StopMovement();
            bIsFirstDiscoverer = false;
            bHasAlertedAllies = false;
            bIsScanning = false;
            FirstDetectLocation = LastKnownLocation;
            SearchCenter = LastKnownLocation;
            SearchTimer = 0.f;
            DetectionValueDecrease(EnemyChar);

            //EnemyScanAround(EnemyChar);

            

            //if (DetectionValue>DetectionThreshold/2)
            //{
            //    if (EnemyChar)
            //        EnemyChar->SetMoveState(EEnemyMoveState::Caution);

            //    
            //    CurrentState = EEnemyState::StandBy;
            //    UE_LOG(LogTemp, Warning, TEXT("%s:Chase->Standby"), *GetName());
            //    //MoveToLocation(LastKnownLocation);
            //}
            //else
            {
                CurrentState = EEnemyState::Caution;

                if (EnemyChar)
                    EnemyChar->SetMoveState(EEnemyMoveState::Caution);

                UE_LOG(LogTemp, Warning, TEXT("%s:Chase->Cation"), *GetName());
                break;
                //StopMovement();
               
               
            }
           
        }
        else
        {
            if (EnemyChar)
                EnemyChar->SetMoveState(EEnemyMoveState::Chase);
            MoveToActor(TargetActor, 10.f);
            
        }
        break;
    }

    

}

//void AEnemyAIController::OnMoveCompleted(
//    FAIRequestID RequestID,
//    const FPathFollowingResult& Result)
//{
//    Super::OnMoveCompleted(RequestID, Result);
//
//    if (CurrentState == EEnemyState::Search&&bIsArrivedSearchLocation)
//    {
//        bIsArrivedSearchLocation = false;
//
//        bIsScanning = true;
//        ScanTimer = 0.f;
//
//        ScanBaseYaw = GetPawn()->GetActorRotation().Yaw;
//        ScanCurrentYaw = ScanBaseYaw;
//        ScanDirection = 1;
//
//        UE_LOG(LogTemp, Warning, TEXT("%s: Search Scan Start"), *GetName());
//
//        UE_LOG(LogTemp, Warning, TEXT("%s: IdouKanryou"), *GetName());
//    }
//}

void AEnemyAIController::Tick(float DeltaTime)
{
    Super::Tick(DeltaTime);

    if (!GetPawn()) return;

    

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
                APlayerCharacter* Player = Cast<APlayerCharacter>(Actor);

                bool bPlayerInRestricted = false;

                if (Player)
                {
                    bPlayerInRestricted = Player->bIsInRestrictedArea;
                    UE_LOG(LogTemp, Warning, TEXT("%s: BisInRestricted True"), *GetName());
                }

                bool bShouldAlert = bPlayerInRestricted || bIsDistrust;

                bool bStateAllowed =
                    CurrentState != EEnemyState::Chase &&
                    CurrentState != EEnemyState::StandBy &&
                    CurrentState != EEnemyState::Investigate;

               // bIsPatrolling = false;
                bIsPlayerInTabooArea = false;
                bCurrentlySeeingTarget = true;
                bIsSensingTerms = true;
                TargetActor = Actor; 
               LastKnownLocation = Actor->GetActorLocation();
               //LockedRotation = Actor->GetActorRotation();

               if (bShouldAlert &&(CurrentState==EEnemyState::Idle||CurrentState==EEnemyState::Caution)
                   /*CurrentState != EEnemyState::Chase &&
                   CurrentState != EEnemyState::StandBy &&
                   CurrentState != EEnemyState::Investigate*/)
               {
                   UE_LOG(LogTemp, Warning, TEXT("%s: StandBy"), *GetName());
                  // if (RestrictedAreaSource.bIsActive)
                   {
                       
                   }
                   
                   /*if (StanbyOrIdle())
                   {
                      
                       StopMovement();
                   }*/

                   CurrentState = EEnemyState::StandBy;
                   bIsAlertWaiting = true;
                   AlertWaitTimer = 0.f;
                   bIsPatrolling = false;
                   StopMovement();
  
                   //SeenHeardObject = Actor;

               }

                // ★ Cautionでまだ保存していないなら保存
               /*if (CurrentState != EEnemyState::Chase && !bHasStoredFirstDetectLocation)
               {
                   FirstDetectLocation = Actor->GetActorLocation();
                   bHasStoredFirstDetectLocation = true;
               }*/

             
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
    if (/*CurrentState == EEnemyState::Chase*/bCurrentlySeeingTarget) return;

    TargetActor = PlayerActor;
    
    //CurrentState = EEnemyState::Chase;
    DetectionValue = DetectionThreshold;
    bIsFirstDiscoverer = false; // 第一発見者以外の連鎖防止
    bIsDistrust = true;
    LastKnownLocation=TargetActor->GetActorLocation();
    MoveToLocation(LastKnownLocation);

    UE_LOG(LogTemp, Warning, TEXT("%s:Jouhou Jushin."), *GetName());
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
    UE_LOG(LogTemp, Warning, TEXT("%s:kokomadeyatta"), *GetName());
    for (AActor* Actor : OverlappedActors)
    {

        AEnemyCharacter* OtherEnemy = Cast<AEnemyCharacter>(Actor);
        if (!OtherEnemy) continue;

        UE_LOG(LogTemp, Warning, TEXT("%s:Sousin,%s Jusin"), *GetName(),*OtherEnemy->GetName());

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

void AEnemyAIController::UpdateDetectionSources()//感知ゲージを増やす関数。主にプレイヤーを発見した時
{
    


    float TotalIncrease = 0.f;

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

    if (bIsDistrust)//1度敵に追いかけられているなら
    {
        if (SightSource.bIsActive)//敵に見られてたら
        {
            TotalIncrease += SightSource.IncreaseRate + DetectSpeed;
        }
    }

    if (RestrictedAreaSource.bIsActive)//立ち入り禁止区域に入った時
    {
        if (SightSource.bIsActive)//敵に見られてたら
        {
            TotalIncrease += SightSource.IncreaseRate+ DetectSpeed;
        }

        //TotalIncrease += RestrictedAreaSource.IncreaseRate;
    }

    //特定のアイテムを使用した時
    if(false)
    {
        if (SightSource.bIsActive)//敵に見られてたら
        {
            TotalIncrease += SightSource.IncreaseRate + DetectSpeed;
        }

    }

   

    // ★ 最終加算
    DetectionValue += TotalIncrease /** UpdateInterval*/;

   

    // Chase判定
    if (DetectionValue >= DetectionThreshold)
    {
       

        if (bIsPlayerInTabooArea)//立ち入り禁止区域に入った時
        {
            bIsDistrust = true;
            UE_LOG(LogTemp, Warning, TEXT("%s:Utagatteiru"), *GetName());
        }

        bIsFirstDiscoverer = true;

        if (bIsFirstDiscoverer)//第一発見者なら
        {
            UE_LOG(LogTemp, Warning, TEXT("%s:Jpuhou Sousin"), *GetName());
            AlertNearbyAllies();//周辺にいる敵に情報を共有する。
        }

        CurrentState = EEnemyState::Chase;//プレイヤーを追いかける
        bIsArrivedSearchLocation = false;
    }

}

//void AEnemyAIController::DetectionValueIncrease()
//{
//
//    {
//        float Distance = FVector::Dist(
//            GetPawn()->GetActorLocation(),
//            TargetActor->GetActorLocation()
//        );
//
//        // 距離を0〜1に正規化
//        float Alpha = FMath::Clamp(
//            (Distance - MinDetectDistance) /
//            (MaxDetectDistance - MinDetectDistance),
//            0.f,
//            1.f
//        );
//
//        // 距離が近いほどDetectSpeedが高くなる
//        float DetectSpeed = FMath::Lerp(
//            MinDetectSpeed,
//            MaxDetectSpeed,
//            Alpha
//        );
//
//        DetectionValue += DetectSpeed /** UpdateInterval*/;
//    }
//
//    if (DetectionValue >= DetectionThreshold)//発見ゲージが最大までたまったら
//    {
//        if (CurrentState != EEnemyState::Chase)
//        {
//            UE_LOG(LogTemp, Warning, TEXT("%s:ChaseTime!."), *GetName());
//            
//            bIsPatrolling = false;
//            StopMovement();
//
//            bIsFirstDiscoverer = true;
//
//            if (bIsFirstDiscoverer)
//            {
//                UE_LOG(LogTemp, Warning, TEXT("%s:Jpuhou Sousin"), *GetName());
//                AlertNearbyAllies();
//            }
//
//            CurrentState = EEnemyState::Chase;
//        }
//
//        //CurrentState = EEnemyState::Chase;
//
//    }
//
//
//}

void AEnemyAIController::DetectionValueDecrease(AEnemyCharacter* EnemyC)//感知ゲージを減らす関数。主にプレイヤーを見失った時
{
    DetectionValue = FMath::Max(
        0.f,
        DetectionValue - DetectionDecreaseAmount * UpdateInterval * 4
    );

    if (DetectionValue <= 0.0f && bIsScanEnd)
    {
        CurrentState = EEnemyState::Idle;

        if (EnemyC)
            EnemyC->SetMoveState(EEnemyMoveState::Idle);
        // リセット
        bHasStoredFirstDetectLocation = false;
        bIsArrivedSearchLocation = false;
        //  bLockLastKnownLocation = false;
    }

}

void AEnemyAIController::StartPatrol()
{
    if (PatrolPoints.Num() <= 1) return;

    bIsPatrolling = true;
    CurrentPatrolIndex = 0;

    MoveToNextPatrolPoint();
}

void AEnemyAIController::MoveToNextPatrolPoint()
{
    if (!bIsPatrolling || PatrolPoints.Num() <= 1) return;

    ATargetPoint* PatrolTarget = PatrolPoints[CurrentPatrolIndex];
    if (!PatrolTarget) return;

    MoveToLocation(PatrolTarget->GetActorLocation());

    //MoveToActor(PatrolTarget, 1.0f);
}

void AEnemyAIController::EnemyScanAround(AEnemyCharacter* EnemyC)//周囲にプレイヤーがいないか確認する関数
{
    bIsScanEnd = false;

    if (bCurrentlySeeingTarget && TargetActor)
    {
        if (bIsScanning)
        {
            // ★ スキャン中に再視認したら
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

                //AlertWaitTimer = .0f;
               
                
                CurrentState = EEnemyState::StandBy;


                //break;
            }


        }
    }
    else
    {
        if (!bIsScanning)//スキャンしてないなら
        {
           
            MoveToLocation(FirstDetectLocation);

            float Distance = FVector::Dist(
                GetPawn()->GetActorLocation(),
                FirstDetectLocation
            );
            
           

            if (Distance < 50.f || Distance == 0.0f)
            {
                StopMovement();

                bIsScanning = true;
                ScanTimer = 0.f;

                ScanBaseYaw = GetPawn()->GetActorRotation().Yaw;
                ScanCurrentYaw = ScanBaseYaw;
                ScanDirection = 1;
                UE_LOG(LogTemp, Warning, TEXT("%s:Scan Start!."), *GetName());
            }
        }
        else
        {
            //UE_LOG(LogTemp, Warning, TEXT("%s:Now Scanning!."), *GetName());
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
                if (DetectionValue<=0.0f)
                {
                    CurrentState = EEnemyState::Idle;

                    if (EnemyC)
                        EnemyC->SetMoveState(EEnemyMoveState::Idle);
                }
                else
                {
                    CurrentState = EEnemyState::Search;

                    bIsArrivedSearchLocation = false;

                    if (EnemyC)
                        EnemyC->SetMoveState(EEnemyMoveState::Caution);
                    SearchTimer = 0.0f;
                    UE_LOG(LogTemp, Warning, TEXT("%s:Search Mode!."), *GetName());
                    //SearchCenter = FirstDetectLocation;
                    //SearchDuration *= DetectionValue;
                }

                

                bHasStoredFirstDetectLocation = false;
                bIsScanning = false;
                bIsScanEnd = true;

                
            }
            else
            {
               // UE_LOG(LogTemp, Warning, TEXT("%s:Mada Owattenai!."), *GetName());



                //CurrentState = EEnemyState::Caution;
            }

        }

    }


}

bool AEnemyAIController::StanbyOrIdle()
{

    if (bIsDistrust||bIsPlayerInTabooArea)
    {

        return true;
    }
    else
    {
        return false;
    }


}

void AEnemyAIController::SourceBisActive(FDetectionSource AllSource)
{

    if (RestrictedAreaSource.bIsActive)
    {
        bIsPlayerInTabooArea = true;
    }

   
}
