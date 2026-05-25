#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Components/SphereComponent.h"
#include "SmokeGrenadeActor.generated.h"

UCLASS()
class TASKGAME_BADCOMPANY_API ASmokeGrenadeActor : public AActor
{
    GENERATED_BODY()

public:

    ASmokeGrenadeActor();

protected:

    virtual void BeginPlay() override;

    UFUNCTION()
    void OnSmokeBeginOverlap(
        UPrimitiveComponent* OverlappedComp,
        AActor* OtherActor,
        UPrimitiveComponent* OtherComp,
        int32 OtherBodyIndex,
        bool bFromSweep,
        const FHitResult& SweepResult
    );

    UFUNCTION()
    void OnSmokeEndOverlap(
        UPrimitiveComponent* OverlappedComp,
        AActor* OtherActor,
        UPrimitiveComponent* OtherComp,
        int32 OtherBodyIndex
    );

public:

    UPROPERTY(VisibleAnywhere)
    USphereComponent* SmokeSphere;

    UPROPERTY(EditAnywhere)
    float SmokeRadius = 700.f;

    UPROPERTY(EditAnywhere)
    float SmokeLifeTime = 10.f;
};