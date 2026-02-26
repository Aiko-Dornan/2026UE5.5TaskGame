// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AIController.h"
#include "Perception/AIPerceptionTypes.h"//AIに視覚と聴覚の概念を持たせる凄いコンポーネント
#include "Kismet/GameplayStatics.h"
#include "EnemyAIController.generated.h"

class UAIPerceptionComponent;
class UAISenseConfig_Sight;

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

protected:
    virtual void BeginPlay() override;
    virtual void Tick(float DeltaTime) override;

private:
    UPROPERTY()
    UAIPerceptionComponent* PerceptionComp;

    UPROPERTY()
    UAISenseConfig_Sight* SightConfig;

    // ====== 視界設定（エディタ調整可能） ======

    UPROPERTY(EditAnywhere, Category = "AI|Sight")
    float SightRadius = 1500.f;

    UPROPERTY(EditAnywhere, Category = "AI|Sight")
    float LoseSightRadius = 1700.f;

    UPROPERTY(EditAnywhere, Category = "AI|Sight")
    float PeripheralVisionAngle = 60.f;

    // ===== 発見ゲージ =====

    UPROPERTY(EditAnywhere, Category = "AI|Detection")
    float DetectionValue = 0.f;
    UPROPERTY(EditAnywhere, Category = "AI|Detection")
    float DetectionThreshold = 100.f;
    UPROPERTY(EditAnywhere, Category = "AI|Detection")
    float DetectionSpeed = 50.f;     // 1秒あたり増加量
    UPROPERTY(EditAnywhere, Category = "AI|Detection")
    float LoseSpeed = 30.f;          // 1秒あたり減少量
    bool bCurrentlySeeingTarget = false;
    UPROPERTY()
    AActor* TargetActor = nullptr;

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
    // ===== State =====
    EEnemyState CurrentState = EEnemyState::Idle;

   

  

    // ===== Optimization =====
    FTimerHandle AITickHandle;
    float UpdateInterval = 0.1f;

    void UpdateAI();
    void UpdateLOD();

    void SetupPerception();
public:
    virtual void OnPossess(APawn* InPawn) override;
    UFUNCTION()
    void OnTargetPerceptionUpdated(AActor* Actor, FAIStimulus Stimulus);
};
