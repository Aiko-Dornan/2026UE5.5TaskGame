// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Components/CapsuleComponent.h"
#include"Engine/TargetPoint.h"
#include "EnemyCharacter.generated.h"


UENUM()
enum class EEnemyMoveState : uint8
{
    Idle,
    Caution,
    Chase,
    Stop
};


UCLASS()
class TASKGAME_BADCOMPANY_API AEnemyCharacter : public ACharacter
{
	GENERATED_BODY()

public:
    AEnemyCharacter();
    virtual void Tick(float DeltaTime) override;
    void SetMoveState(EEnemyMoveState NewState);

    void SetPreviousState();

    void StunEnemy(float StunTime);

    void RecoverFromStun();

protected:
    virtual void BeginPlay() override;

public:
    //敵の移動速度
    UPROPERTY(EditAnywhere,/* BlueprintReadWrite,*/ Category = "Enemy")
    float EnemyMoveSpeed[4] = { 
        300.0f,//Idle
        10.0f,//Cation
        500.0f,//Chase
        0.0f//Stop
                              };

    //敵キャラの認識範囲
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "AI")
    float SightRadius = 1500.f;
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "AI")
    float PeripheralVisionAngle = 60.f;
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "AI")
    float LoseSightRadius = 1700.f;    // 見失う距離
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "AI")
    float SetMaxAge =3.0f;              // 記憶時間
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "AI")
    float DetectionThreshold = 300.0f;

   //Patroll
    UPROPERTY(EditAnywhere,BlueprintReadWrite, Category = "AI|Patrol")
    TArray<ATargetPoint*> EnemyPatrolPoints;//巡回ポイント用のアクタ。敵はこれを目指す。

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "AIState")
    int StateNum = 0; UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "AIState")
    EEnemyMoveState PreviousMoveState;

    bool bIsStunned;

private:
    FTimerHandle StunTimerHandle;
   
    // 現在の状態
    EEnemyMoveState CurrentMoveState;

};
