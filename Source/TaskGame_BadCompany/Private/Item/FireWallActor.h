// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Item/ThrowItem.h"
#include "FireWallActor.generated.h"

/**
 * 
 */

class UBoxComponent;

class UNavModifierComponent;

UCLASS()
class AFireWallActor : public AThrowItem
{
	GENERATED_BODY()
	
public:
    AFireWallActor();

protected:
    virtual void BeginPlay() override;

    virtual void OnThrowItemHit(
        const FHitResult& Hit) override;

    UFUNCTION()
    void ActivateFireWall();

public:
    UPROPERTY(VisibleAnywhere)
    UBoxComponent* WallCollision;

    UPROPERTY(VisibleAnywhere)
    UStaticMeshComponent* WallMesh;

    UPROPERTY(VisibleAnywhere)
    UNavModifierComponent* NavModifier;

    UPROPERTY(EditAnywhere)
    float LifeTime = 8.f;

    bool bIsActivatedFire = false;

};
