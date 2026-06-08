// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Item/BaseItem.h"
#include "InstallationItem.generated.h"

/**
 * 
 */
UCLASS()
class AInstallationItem : public ABaseItem
{
	GENERATED_BODY()
	
public:

    AInstallationItem();

    UPROPERTY(EditAnywhere, Category = "Installation")
    float InstallationDistance = 500.f;

    //bool InstallItem(class APlayerCharacter* Player);

protected:

    /*virtual void SpawnInstalledActor(
        const FHitResult& Hit);*/

};
