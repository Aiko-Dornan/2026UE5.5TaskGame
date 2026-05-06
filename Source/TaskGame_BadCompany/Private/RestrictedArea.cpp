// Fill out your copyright notice in the Description page of Project Settings.


#include "RestrictedArea.h"

// Sets default values
ARestrictedArea::ARestrictedArea()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
   Box= CreateDefaultSubobject<UBoxComponent>(TEXT("RestrictedBox"));
   RootComponent = Box;
}

// Called when the game starts or when spawned
void ARestrictedArea::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void ARestrictedArea::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void ARestrictedArea::NotifyActorBeginOverlap(AActor* OtherActor)
{
    APlayerCharacter* Player = Cast<APlayerCharacter>(OtherActor);
    if (Player)
    {
        Player->bIsInRestrictedArea = true;
        UE_LOG(LogTemp, Warning, TEXT("%s: In"), *OtherActor->GetName());
    }
}

void ARestrictedArea::NotifyActorEndOverlap(AActor* OtherActor)
{
    APlayerCharacter* Player = Cast<APlayerCharacter>(OtherActor);
    if (Player)
    {
        Player->bIsInRestrictedArea = false;
        UE_LOG(LogTemp, Warning, TEXT("%s: Out"), *OtherActor->GetName());
    }
}