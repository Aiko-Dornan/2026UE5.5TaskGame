// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "InputActionValue.h"
#include "Components/CapsuleComponent.h"
#include"Item/BaseItem.h"
#include"InteractWidget.h"
#include "PlayerCharacter.generated.h"

class UInputMappingContext;
class UInputAction;
class UCameraComponent;
class UInteractWidget;

UCLASS()
class TASKGAME_BADCOMPANY_API APlayerCharacter : public ACharacter
{
    GENERATED_BODY()

public:
    APlayerCharacter();

    
    //îÒì¸óÕä÷êî
    UFUNCTION()
    bool PerformCameraLineTrace(FHitResult& OutHit, float TraceDistance);
    void TraceForItem();
    void UpdateInteractUI(ABaseItem* NewItem);
protected:
    virtual void BeginPlay() override;
    virtual void SetupPlayerInputComponent(UInputComponent* PlayerInputComponent) override;

    // ===== ÉJÉÅÉâ =====
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

   

    // ===== ì¸óÕä÷êî =====
    void Move(const FInputActionValue& Value);
    void Look(const FInputActionValue& Value);
    void Interact();

   

public:
    ABaseItem* CurrentItem;
    UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Player")
    float LineTraceEnd = 300.0f;
   

private:
    FTimerHandle TraceTimerHandle;

    UPROPERTY(EditDefaultsOnly, Category = "UI")
    TSubclassOf<UInteractWidget> InteractWidgetClass;

    UPROPERTY()
    UInteractWidget* InteractWidget;
};
