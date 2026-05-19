// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Item/BaseItem.h"
#include "ThrowItem.generated.h"

/**
 * 
 */

enum class IDThrowItemType : uint8
{
    MINE,
    SOUND,
    SMOKE,
    FIREWALL,
};

UCLASS()
class AThrowItem : public ABaseItem
{
	GENERATED_BODY()
	
    UFUNCTION()
    void FireMine();//地雷炸裂
    void SearchMine();//地雷の周囲に敵がいないか探知

    void FireSound();//音で周辺の敵を呼び寄せる。

    void FireSmoke();//煙で視界妨害とそれまでの待機

    void CreateFireWall();

    void WaitFireTime();//時限式のアイテムが起動するまでの待ち時間

};
