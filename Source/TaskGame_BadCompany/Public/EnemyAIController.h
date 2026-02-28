// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AIController.h"
#include "Perception/AIPerceptionTypes.h"//AIに視覚と聴覚の概念を持たせる凄いコンポーネント
#include "Kismet/GameplayStatics.h"
#include"PlayerCharacter.h"
//#include"GameFramework//Controller.h"
#include "EnemyAIController.generated.h"

class UAIPerceptionComponent;
class UAISenseConfig_Sight;
class UAISenseConfig_Hearing;

UENUM()
enum class EEnemyState : uint8
{
    Idle,
    Caution,
    Chase
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
protected:
    virtual void BeginPlay() override;
    virtual void Tick(float DeltaTime) override;

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

    // ===== 発見ゲージ =====

   
    bool bCurrentlySeeingTarget = false;
    UPROPERTY(EditAnywhere, Category = "AI|Detection")
    AActor* TargetActor = nullptr;

    UPROPERTY(EditAnywhere, Category = "AI|Detection")
    float DetectionValue = 0.f;
    UPROPERTY(EditAnywhere, Category = "AI|Detection")
    float DetectionThreshold = 100.f;
    UPROPERTY(EditAnywhere, Category = "AI|Detection")
    float DetectionSpeed = 50.f;     // 1秒あたり増加量
    UPROPERTY(EditAnywhere, Category = "AI|Detection")
    float LoseSpeed = 30.f;          // 1秒あたり減少量

    UPROPERTY(EditAnywhere, Category = "AI|Detection")
    float MinDetectDistance = 300.f;   // 最速検知距離
    UPROPERTY(EditAnywhere, Category = "AI|Detection")
    float MaxDetectDistance = 2000.f;  // 最遅検知距離
    UPROPERTY(EditAnywhere, Category = "AI|Detection")
    float MinDetectSpeed = 40.f;   // 近距離時増加量
    UPROPERTY(EditAnywhere, Category = "AI|Detection")
    float MaxDetectSpeed = 5.f;    // 遠距離時増加量
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
    EEnemyState CurrentState = EEnemyState::Idle;

   

  

    // ===== Optimization =====
    FTimerHandle AITickHandle;
    float UpdateInterval = 0.1f;

    void UpdateAI();
    void UpdateLOD();

    void SetupPerception();
private:
   // APawn* Player;
    bool bLockLastKnownLocation = false;

    FVector FirstDetectLocation;
    bool bHasStoredFirstDetectLocation = false;

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

public:
   

   
};
