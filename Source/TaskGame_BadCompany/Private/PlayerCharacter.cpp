#include "PlayerCharacter.h"
#include "Camera/CameraComponent.h"
#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"
#include "InputMappingContext.h"
#include "InputAction.h"

APlayerCharacter::APlayerCharacter()
{
    PrimaryActorTick.bCanEverTick = true;

    // ===== カメラ作成 =====
    FPSCamera = CreateDefaultSubobject<UCameraComponent>(TEXT("FPSCamera"));
    check(FPSCamera != nullptr);

    FPSCamera->SetupAttachment(CastChecked<USceneComponent, UCapsuleComponent>(GetCapsuleComponent()));
    //FPSCamera->SetupAttachment(GetCapsuleComponent());
    FPSCamera->SetRelativeLocation(FVector(0.f, 0.f, 64.f));
    FPSCamera->bUsePawnControlRotation = true;

    // キャラはコントローラー回転に追従しない
    bUseControllerRotationYaw = true;
    bUseControllerRotationPitch = true;
    bUseControllerRotationRoll = false;
 
}

void APlayerCharacter::BeginPlay()
{
    Super::BeginPlay();

    if (APlayerController* PC = Cast<APlayerController>(Controller))
    {
        if (ULocalPlayer* LP = PC->GetLocalPlayer())
        {
            if (UEnhancedInputLocalPlayerSubsystem* Subsystem =
                LP->GetSubsystem<UEnhancedInputLocalPlayerSubsystem>())
            {
                Subsystem->AddMappingContext(PlayerInputContext, 0);
               
            }
        }
    }

    if (InteractWidgetClass)
    {
        InteractWidget = CreateWidget<UInteractWidget>(GetWorld(), InteractWidgetClass);

        if (InteractWidget)
        {
            InteractWidget->AddToViewport();
            InteractWidget->SetVisibility(ESlateVisibility::Hidden);
        }
    }

    //アイテム名取得
    GetWorldTimerManager().SetTimer(
        TraceTimerHandle,
        this,
        &APlayerCharacter::TraceForItem,
        0.05f,   // 0.05秒ごと
        true
    );

}

void APlayerCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
    Super::SetupPlayerInputComponent(PlayerInputComponent);

    if (UEnhancedInputComponent* EnhancedInput =
        Cast<UEnhancedInputComponent>(PlayerInputComponent))
    {
        EnhancedInput->BindAction(MoveAction, ETriggerEvent::Triggered, this, &APlayerCharacter::Move);
        EnhancedInput->BindAction(LookAction, ETriggerEvent::Triggered, this, &APlayerCharacter::Look);
        EnhancedInput->BindAction(JumpAction, ETriggerEvent::Started, this, &ACharacter::Jump);
        EnhancedInput->BindAction(JumpAction, ETriggerEvent::Completed, this, &ACharacter::StopJumping);
        EnhancedInput->BindAction(InteractAction, ETriggerEvent::Started, this, &APlayerCharacter::Interact);

    }

   
}

void APlayerCharacter::Move(const FInputActionValue& Value)
{
    FVector2D MovementVector = Value.Get<FVector2D>();
   

    AddMovementInput(GetActorForwardVector(), MovementVector.Y);
    AddMovementInput(GetActorRightVector(), MovementVector.X);
   
}

void APlayerCharacter::Look(const FInputActionValue& Value)
{
    FVector2D LookAxis = Value.Get<FVector2D>();

    AddControllerYawInput(LookAxis.X);
    AddControllerPitchInput(LookAxis.Y);
}

bool APlayerCharacter::PerformCameraLineTrace(FHitResult& OutHit, float TraceDistance)
{
    if (!FPSCamera) return false;//カメラがないなら実行しない。

    FVector Start = FPSCamera->GetComponentLocation();//開始位置はカメラ位置。
    FVector End = Start + (FPSCamera->GetForwardVector() * TraceDistance);//終了位置。

    FCollisionQueryParams Params;
    Params.AddIgnoredActor(this);//自分(この場合はプレイヤー)をヒット判定から除外

    //ライントレースを飛ばしてヒット判定を返り値にする。
    return GetWorld()->LineTraceSingleByChannel(
        OutHit,
        Start,
        End,
        ECC_Visibility,
        Params
    );
}

void APlayerCharacter::Interact()
{
   

    FHitResult Hit;
    
    bool bHit = PerformCameraLineTrace(Hit, LineTraceEnd);

    if (bHit)
    {
        ABaseItem* Item = Cast<ABaseItem>(Hit.GetActor());
        if (Item)//Hitしたのがアイテムだったら
        {
            Item->OnPickedUp(this);//取得を実行
        }
    }
}

void APlayerCharacter::TraceForItem()
{
    FHitResult Hit;

    bool bHit = PerformCameraLineTrace(Hit, LineTraceEnd);

    ABaseItem* HitItem = bHit ? Cast<ABaseItem>(Hit.GetActor()) : nullptr;

    if (HitItem != CurrentItem)
    {
        CurrentItem = HitItem;
        UpdateInteractUI(CurrentItem);
    }
}

void APlayerCharacter::UpdateInteractUI(ABaseItem* NewItem)
{
   
        if (!InteractWidget) return;

        if (NewItem)
        {
            FString Text = NewItem->ItemName.ToString() + NewItem->InteractText.ToString();

            InteractWidget->SetInteractText(Text);
            InteractWidget->SetVisibility(ESlateVisibility::Visible);
        }
        else
        {
            InteractWidget->SetVisibility(ESlateVisibility::Hidden);
        }
    
}
