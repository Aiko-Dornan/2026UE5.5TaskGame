#include "EnemyCharacter.h"
#include "EnemyAIController.h"

AEnemyCharacter::AEnemyCharacter()
{
    PrimaryActorTick.bCanEverTick = true;

    // AIコントローラー設定
    AIControllerClass = AEnemyAIController::StaticClass();
    AutoPossessAI = EAutoPossessAI::PlacedInWorldOrSpawned;

    // 向きを移動方向に合わせる
    GetCharacterMovement()->bOrientRotationToMovement = true;
    bUseControllerRotationYaw = false;

    // 移動速度
    GetCharacterMovement()->MaxWalkSpeed = EnemyMoveSpeed[0];
    GetCharacterMovement()->bEnablePhysicsInteraction = false;
    GetCapsuleComponent()->SetCollisionResponseToChannel(ECC_GameTraceChannel1, ECR_Ignore);
}

void AEnemyCharacter::BeginPlay()
{
    Super::BeginPlay();

    if (!GetController())
    {
        SpawnDefaultController();
    }
}

void AEnemyCharacter::Tick(float DeltaTime)
{
    Super::Tick(DeltaTime);
}

void AEnemyCharacter::SetMoveState(EEnemyMoveState NewState)
{
    switch (NewState)
    {
    case EEnemyMoveState::Idle:
        GetCharacterMovement()->MaxWalkSpeed = EnemyMoveSpeed[0];
        bUseControllerRotationYaw = false;
        GetCharacterMovement()->bOrientRotationToMovement = true;
        break;

    case EEnemyMoveState::Caution:
        GetCharacterMovement()->MaxWalkSpeed = EnemyMoveSpeed[1];
        // ★ 回転無効化
        bUseControllerRotationYaw = false;
        GetCharacterMovement()->bOrientRotationToMovement = true;
        break;

    case EEnemyMoveState::Chase:
        GetCharacterMovement()->MaxWalkSpeed = EnemyMoveSpeed[2];
        // ★ 回転無効化
        bUseControllerRotationYaw = true;
        GetCharacterMovement()->bOrientRotationToMovement = true;
        break;
    }
}