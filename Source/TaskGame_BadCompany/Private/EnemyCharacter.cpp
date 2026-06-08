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
    // 現在状態を保存
    CurrentMoveState = NewState;

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
    case EEnemyMoveState::Stop:

        GetCharacterMovement()->MaxWalkSpeed = EnemyMoveSpeed[3];

        bUseControllerRotationYaw = false;

        GetCharacterMovement()->bOrientRotationToMovement = false;

        break;
    }
}

void AEnemyCharacter::SetPreviousState()
{
    // 停止前の状態を保存
    PreviousMoveState = CurrentMoveState;

}

void AEnemyCharacter::StunEnemy(float StunTime)
{

    // 既に停止中なら更新だけ
    if (CurrentMoveState == EEnemyMoveState::Stop)
    {
        GetWorldTimerManager().ClearTimer(StunTimerHandle);

        GetWorldTimerManager().SetTimer(
            StunTimerHandle,
            this,
            &AEnemyCharacter::RecoverFromStun,
            StunTime,
            false
        );

        return;
    }

    bIsStunned = true;
    
    SetPreviousState();
    // 移動停止
    SetMoveState(EEnemyMoveState::Stop);

    // AI移動も止める
    AAIController* AICon = Cast<AAIController>(GetController());

    if (AICon)
    {
        AICon->StopMovement();
    }

    // 一定時間後に解除
    GetWorldTimerManager().ClearTimer(StunTimerHandle);

    GetWorldTimerManager().SetTimer(
        StunTimerHandle,
        this,
        &AEnemyCharacter::RecoverFromStun,
        StunTime,
        false
    );
}

void AEnemyCharacter::RecoverFromStun()
{
    // 元の状態へ戻す
    SetMoveState(PreviousMoveState);
    bIsStunned = false;

    AEnemyAIController* EnemyAI = Cast<AEnemyAIController>(GetController());

    if (!EnemyAI)
    {
        return;
    }

    // IdleならPatrol再開
    if (PreviousMoveState == EEnemyMoveState::Idle)
    {
        EnemyAI->MoveToNextPatrolPoint();
    }

}



