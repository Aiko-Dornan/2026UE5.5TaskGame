// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AIController.h"
#include "Perception/AIPerceptionTypes.h"//AIに視覚と聴覚の概念を持たせる凄いコンポーネント
#include "Kismet/GameplayStatics.h"
#include"PlayerCharacter.h"
#include"EnemyCharacter.h"
//#include"GameFramework//Controller.h"
#include "Engine/TargetPoint.h"
#include "NavigationSystem.h"
#include "Navigation/PathFollowingComponent.h"
#include "EnemyAIController.generated.h"

class UAIPerceptionComponent;
class UAISenseConfig_Sight;
class UAISenseConfig_Hearing;



USTRUCT()
struct FDetectionSource
{
    GENERATED_BODY()

    bool bIsActive = false;//瞬間的な判断を行うフラグ。今現在条件を満たしているかどうか。
    
    float IncreaseRate = 0.f;   // 1秒あたり増加量
};

UENUM()
enum class EEnemyState : uint8
{
    Idle,
    Caution,
    Chase,
    StandBy,//プレイヤーがいた場所まで移動するまでの待機状態(疑惑)
    Investigate,
    Search,

};

/**
 * 
 */
UCLASS()
class TASKGAME_BADCOMPANY_API AEnemyAIController : public AAIController
{
	GENERATED_BODY()
	
public:
    AEnemyAIController();

    virtual void OnPossess(APawn* InPawn) override;

    UFUNCTION()
    void OnTargetPerceptionUpdated(AActor* Actor, FAIStimulus Stimulus);

    //敵の感知具合を表す関数
    float GetDetectionPercent() const;

    //敵から敵にプレイヤーの発見を伝える関数
    void AlertNearbyAllies();
    //プレイヤーを発見したことを他の敵から受け取る用の関数
    void OnAlertedByAlly(AActor* PlayerActor);



protected:
    virtual void BeginPlay() override;
    virtual void Tick(float DeltaTime) override;

    void DetectionValueIncrease();
    void DetectionValueDecrease(AEnemyCharacter* EnemyC);//感知ゲージ減少関数
    void EnemyScanAround(AEnemyCharacter* EnemyC);//周囲確認を行う関数

    void UpdateDetectionSources();//感知ゲージ増加

    void StartPatrol();         //巡回を開始する関数
    void MoveToNextPatrolPoint();//次のポイントに目標を変える関数
    bool StanbyOrIdle();
    void SourceBisActive(FDetectionSource AllSource);//bIsActiveの判定をここにまとめる。
   /* void OnMoveCompleted(
        FAIRequestID RequestID,
        const FPathFollowingResult& Result);*/


private:
    UPROPERTY()
    UAIPerceptionComponent* PerceptionComp;

    UPROPERTY()
    UAISenseConfig_Sight* SightConfig;
    UPROPERTY()
    UAISenseConfig_Hearing* HearingConfig;

    // ====== 視界設定（エディタ調整可能） ======

    UPROPERTY(EditAnywhere, Category = "AI|Sight")
    float SightRadius = 1500.f;

    UPROPERTY(EditAnywhere, Category = "AI|Sight")
    float LoseSightRadius = 1700.f;

    UPROPERTY(EditAnywhere, Category = "AI|Sight")
    float PeripheralVisionAngle = 60.f;

    UPROPERTY(EditAnywhere, Category = "AI|Hear")
    float HearRadius = 500.f;

    // ===== 発見ゲージ =====

   
    bool bCurrentlySeeingTarget = false;
    UPROPERTY(EditAnywhere, Category = "AI|Detection")
    AActor* TargetActor = nullptr;

    UPROPERTY(EditAnywhere, Category = "AI|Detection")
    float DetectionValue = 0.f;//現在の感知具合を表す
    UPROPERTY(EditAnywhere, Category = "AI|Detection")
    float DetectionThreshold = 100.f;//感知ゲージの最大値。DetectionValue=これになったらChaseにする。
    UPROPERTY(EditAnywhere, Category = "AI|Detection")
    float DetectionSpeed = 50.f;     // 1秒あたり増加量
    UPROPERTY(EditAnywhere, Category = "AI|Detection")
    float LoseSpeed = 30.f;          // 1秒あたり減少量

    UPROPERTY(EditAnywhere, Category = "AI|Detection")
    float MinDetectDistance = 100.f;   // 最速検知距離
    UPROPERTY(EditAnywhere, Category = "AI|Detection")
    float MaxDetectDistance = 2000.f;  // 最遅検知距離
    UPROPERTY(EditAnywhere, Category = "AI|Detection")
    float MinDetectSpeed = 12.f;   // 近距離時増加量
    UPROPERTY(EditAnywhere, Category = "AI|Detection")
    float MaxDetectSpeed = 0.2f;    // 遠距離時増加量
    UPROPERTY(EditAnywhere, Category = "AI|Detection")
    float DetectionDecreaseAmount=5.0f;

    UPROPERTY(EditAnywhere, Category = "AI|Detection")
    float UpdateIntevalTime[3] = { 0.05f,0.3f,1.0f };//敵とプレイヤーの距離毎に更新速度を変更する

    FVector LastKnownLocation;
   // FRotator LockedRotation;

    //// CautionからIdleに戻るまでの時間
    //UPROPERTY(EditAnywhere, Category = "AI|State")
    //float CautionDuration = 5.f;

    //float CautionTimer = 0.f;

    // ===== State =====
    UPROPERTY(VisibleAnywhere, Category = "AI|Detection")
    EEnemyState CurrentState = EEnemyState::Idle;

   

  

    // ===== Optimization =====
    FTimerHandle AITickHandle;
    float UpdateInterval = 0.1f;

    void UpdateAI();
    void UpdateLOD();

    void SetupPerception();
private:
  
    //最後にプレイヤーを発見した位置を保管してあるかどうか
    bool bLockLastKnownLocation = false;

    //最初にプレイヤーを発見した位置
    FVector FirstDetectLocation;
    //最初にプレイヤーを発見した時用のフラグ
    bool bHasStoredFirstDetectLocation = false;

    bool bIsDistrust = false;//長期的な判断用のフラグ。過去に条件を満たしていたかどうか。

    // ★ 初動停止用
    bool bIsAlertWaiting = false;
    float AlertWaitTimer = 0.f;

    UPROPERTY(EditAnywhere, Category = "AI|Alert")
    float AlertWaitTime = 1.0f; // 何秒止まるか

    //Cation時にプレイヤーがいた場所までついた時の処理するスキャン用の変数
    bool bIsScanning = false;       //スキャン可かどうか
    float ScanTimer = 0.f;
    float ScanDuration = 2.5f;     // スキャン時間
    float ScanRotateSpeed = 120.f; // 回転速度

    float ScanBaseYaw = 0.f;      // スキャン開始時の向き
    float ScanCurrentYaw = 0.f;   // 現在Yaw
    float ScanRange = 60.f;       // ±角度
    float ScanSpeed = 120.f;      // 回転速度
    int   ScanDirection = 1;      // 1 or -1

    bool bIsScanEnd = false;
    bool bIsPlayerInTabooArea = false;

    // ===== Patrol =====
    UPROPERTY()
    TArray<ATargetPoint*> PatrolPoints;//巡回ポイント用のアクタ。敵はこれを目指す。

    int32 CurrentPatrolIndex = 0;//現在何個目の巡回ポイントを目指しているか

    UPROPERTY(EditAnywhere, Category = "AI|Patrol")
    float PatrolAcceptanceRadius = 100.f;

    bool bIsPatrolling = false;//パトロール中かどうか判断用のフラグ。

    //State::Search時に使う変数
    FVector SearchCenter;
    FVector RandomMoveLocation;
    float SearchRadius = 400.f;
    float SearchDuration = 8.f;
    float SearchTimer = 0.f;
    bool bIsArrivedSearchLocation = false;

public:
   
    bool bIsFirstDiscoverer = false; // 第一発見者か
    bool bHasAlertedAllies = false;  // すでに通報したか
    bool bIsSensingTerms = false;
    AActor* SeenHeardObject=nullptr;

    bool bHeardSound = false;//音を聞いたかどうか
    FVector HeardLocation;//聞こえた場所
    bool HeardOnce = false;//1度音を聞いたかどうか、聞いたならスキャン完了まで聴覚関数には移動しない

    UPROPERTY(EditAnywhere,BlueprintReadWrite, Category = "AI|Detection")
    float AlertRadius=2000.0f;      //第一発見者から他の敵に連鎖する範囲
   
    // ===== Detection Sources =====
    FDetectionSource SightSource;
    FDetectionSource RestrictedAreaSource;

   
};
