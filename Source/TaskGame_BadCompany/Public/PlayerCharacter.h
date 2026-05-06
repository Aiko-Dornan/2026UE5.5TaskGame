// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "InputActionValue.h"
#include "Components/CapsuleComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include"Item/BaseItem.h"
#include"InteractWidget.h"
#include"DetectionWidget.h"
#include "Perception/AIPerceptionStimuliSourceComponent.h"
#include "Perception/AISense_Sight.h" 
#include "PlayerCharacter.generated.h"

class UInputMappingContext;
class UInputAction;
class UCameraComponent;
class UInteractWidget;
class UDetectionWidget;
class UCharacterMovementComponent;

UCLASS()
class TASKGAME_BADCOMPANY_API APlayerCharacter : public ACharacter
{
    GENERATED_BODY()

public:
    APlayerCharacter();

    
    //非入力関数
    UFUNCTION()
    bool PerformCameraLineTrace(FHitResult& OutHit, float TraceDistance);
    void TraceForItem();
    void UpdateInteractUI(ABaseItem* NewItem);
    //----//
    //敵の感知具合を受け取る関数
    float GetMaxEnemyDetection() const;
    void UpdateDetectionUI();

protected:
    virtual void BeginPlay() override;
    virtual void SetupPlayerInputComponent(UInputComponent* PlayerInputComponent) override;

    // ===== カメラ =====
    UPROPERTY(VisibleAnywhere)
    UCameraComponent* FPSCamera;

    // ===== Input =====
    UPROPERTY(EditAnywhere,BlueprintReadWrite, Category = "Input")
    UInputMappingContext* PlayerInputContext;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Input")
    UInputAction* MoveAction;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Input")
    UInputAction* LookAction;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Input")
    UInputAction* JumpAction;

    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Input")
    UInputAction* InteractAction;

    // AIに検知されるためのコンポーネント
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "AI")
    UAIPerceptionStimuliSourceComponent* StimuliSource;

    // ===== 入力関数 =====
    void Move(const FInputActionValue& Value);
    void Look(const FInputActionValue& Value);
    void Interact();

   

public:
    ABaseItem* CurrentItem;
    UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Player")
    float LineTraceEnd = 300.0f;
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Player")
    bool bIsInRestrictedArea = false;

   

private:
    FTimerHandle TraceTimerHandle;

    UPROPERTY(EditDefaultsOnly, Category = "UI")
    TSubclassOf<UInteractWidget> InteractWidgetClass;
    UPROPERTY()
    UInteractWidget* InteractWidget;


    UPROPERTY(EditAnywhere, Category = "UI")
    TSubclassOf<UDetectionWidget> DetectionWidgetClass;
    UPROPERTY()
    UDetectionWidget* DetectionWidget;
    FTimerHandle DetectionUITimer;

};
