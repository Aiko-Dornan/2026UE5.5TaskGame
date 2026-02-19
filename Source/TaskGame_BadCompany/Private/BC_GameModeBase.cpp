// Fill out your copyright notice in the Description page of Project Settings.


#include "BC_GameModeBase.h"
#include "PlayerCharacter.h"


ABC_GameModeBase::ABC_GameModeBase()
{
	//DefaultPawnClass = APlayerCharacter::StaticClass();

	static ConstructorHelpers::FClassFinder<APawn> PlayerPawnBPClass(TEXT("/Game/BluePrint/BP_PlayerCharacter"));
	//DefaultPawnClass = PlayerPawnBPClass.Class;

	if (PlayerPawnBPClass.Class != nullptr)
	{
		DefaultPawnClass = PlayerPawnBPClass.Class;
	}

}