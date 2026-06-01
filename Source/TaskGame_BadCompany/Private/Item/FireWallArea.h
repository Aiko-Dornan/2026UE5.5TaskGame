// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include"EnemyAIController.h"
#include "FireWallArea.generated.h"

class UBoxComponent;
class UNavModifierComponent;

UCLASS()
class AFireWallArea : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AFireWallArea();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	UFUNCTION()
	void EndPlay(
		const EEndPlayReason::Type EndPlayReason);

	

	void RestoreEnemyMovement();

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	UPROPERTY(VisibleAnywhere)
	UBoxComponent* NavBlockBox;

	UPROPERTY(VisibleAnywhere)
	UNavModifierComponent* NavModifier;

	UPROPERTY(EditAnywhere)
	float LifeTime = 10.f;

	UPROPERTY()
	TArray<AEnemyAIController*> StoppedEnemies;

};
