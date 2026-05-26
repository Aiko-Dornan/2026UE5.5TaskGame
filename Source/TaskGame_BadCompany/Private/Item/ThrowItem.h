// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Item/BaseItem.h"
#include "GameFramework/ProjectileMovementComponent.h"
#include "Components/SphereComponent.h"
#include "Perception/AISense_Hearing.h"
#include "EnemyCharacter.h"
#include "ThrowItem.generated.h"

/**
 * 
 */
UENUM(BlueprintType)
enum class EThrowItemType : uint8
{
    MINE,
    SOUND,
    SMOKE,
    FIREWALL,
};

class UNiagaraComponent;

UCLASS()
class AThrowItem : public ABaseItem
{
	GENERATED_BODY()
	
public:

    AThrowItem();

    void Throw(const FVector& Direction, float ThrowPower);

    /*UFUNCTION()
    void OnThrowItemStop(const FHitResult& ImpactResult);*/

    UFUNCTION()
    void OnMeshHit(
        UPrimitiveComponent* HitComponent,
        AActor* OtherActor,
        UPrimitiveComponent* OtherComp,
        FVector NormalImpulse,
        const FHitResult& Hit
    );

    /*UFUNCTION()
    void OnProjectileBounce(
        const FHitResult& ImpactResult,
        const FVector& ImpactVelocity
    );

    UFUNCTION()
    void OnHit(UPrimitiveComponent* HitComp, AActor* OtherActor,
        UPrimitiveComponent* OtherComp, FVector NormalImpulse,
        const FHitResult& Hit);*/

    void TouchObject();

   /* UFUNCTION()
    void OnSearchSphereOverlap(
        UPrimitiveComponent* OverlappedComponent,
        AActor* OtherActor,
        UPrimitiveComponent* OtherComp,
        int32 OtherBodyIndex,
        bool bFromSweep,
        const FHitResult& SweepResult
    );*/

    // ===== 地雷探知 =====
    UFUNCTION()
    void OnMineOverlap(
        UPrimitiveComponent* OverlappedComponent,
        AActor* OtherActor,
        UPrimitiveComponent* OtherComp,
        int32 OtherBodyIndex,
        bool bFromSweep,
        const FHitResult& SweepResult
    );

    // ===== 地雷設置 =====
    void CheckMineStopped();

    void ActivateMine();

    UFUNCTION()
    void FireMine();//地雷炸裂
   // void SearchMine();//地雷の周囲に敵がいないか探知

    void FireSound();//音で周辺の敵を呼び寄せる。

    void FireSmoke();//煙で視界妨害とそれまでの待機

    void CreateFireWall();

    void WaitFireTime();//時限式のアイテムが起動するまでの待ち時間

    //void SetThrowPower(float NewPower);



    //virtual void UseItem(APlayerCharacter* Player) override;
protected:
    virtual void BeginPlay() override;
    //virtual void Tick(float DeltaTime) override;
    UFUNCTION()
    virtual void OnThrowItemHit(
        const FHitResult& Hit);

public:
    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    EThrowItemType ThrowType;
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Item|ThrowItem")
    float ItemGravityScale = 1.0f;
    UPROPERTY(EditAnywhere, BlueprintReadWrite,Category="Item|ThrowItem")
    bool bIsBounce = true;
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Item|ThrowItem")
    float ItemBouncePower = 0.3f;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Item|ThrowItem")
    float ItemFrictionPower = 2.0f;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Item|ThrowItem")
    float ItemStopSpeed = 50.0f;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Item|ThrowItem")
    float ItemSoundLoudness = 1.0f;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Item|ThrowItem")
    float ItemSoundRange = 1200.f;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
    UProjectileMovementComponent* ProjectileMovement;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Item|Mine")
    USphereComponent* SearchSphere;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Item|Mine")
    float SearchRadius = 500.0f;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Item|Mine")
    float WaitExplosionTime = 0.5f;//起爆待機時間

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Item|Mine")
    float MineStunTime = 3.0f;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Item|ThrowItem")
    float TouchTime = 0.1f;//地面に当たると音が鳴る関数を短時間に呼びすぎないようにする待機時間

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Item|ThrowItem")
    float ReduceThrowPower = 10.0f;//投げるときのパワーを削る変数。

protected:

    

    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Item|ThrowItem")
    bool bIsThrown = false;

private:
    FTimerHandle TouchFlagTimerHandle;
    bool bIsTouchObject = false;

    FTimerHandle ExplosionMineTimerHandle;
    // ===== 地雷起動済み =====
    bool bMineActivated = false;

    // ===== 多重爆発防止 =====
    bool bExploded = false;

    float BeforeEnemySpeed = 200.0f;

    FTimerHandle MineCheckTimerHandle;

};
