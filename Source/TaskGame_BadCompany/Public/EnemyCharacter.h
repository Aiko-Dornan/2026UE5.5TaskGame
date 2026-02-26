// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "EnemyCharacter.generated.h"

UCLASS()
class TASKGAME_BADCOMPANY_API AEnemyCharacter : public ACharacter
{
	GENERATED_BODY()

public:
    AEnemyCharacter();
    virtual void Tick(float DeltaTime) override;

protected:
    virtual void BeginPlay() override;

public:
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Enemy")
    float EnemyMoveSpeed = 400.0f;//敵の移動速度

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
};
