#include "PlayerCharacter.h"
#include "Camera/CameraComponent.h"
#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"
#include "InputMappingContext.h"
#include"EnemyAIController.h"
#include "InputAction.h"
#include "InventoryComponent.h"
#include"InventoryWidget.h"

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
 
    // StimuliSource 作成
    StimuliSource = CreateDefaultSubobject<UAIPerceptionStimuliSourceComponent>(TEXT("StimuliSource"));
    // Sight登録
    StimuliSource->RegisterForSense(UAISense_Sight::StaticClass());
    // システムへ登録
    StimuliSource->RegisterWithPerceptionSystem();

    GetCharacterMovement()->bEnablePhysicsInteraction = false;
    GetCapsuleComponent()->SetCollisionResponseToChannel(ECC_GameTraceChannel1, ECR_Ignore);

    Tags.Add("Player");

    Inventory = CreateDefaultSubobject<UInventoryComponent>(TEXT("Inventory"));

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

    if (InventoryWidgetClass)
    {
        InventoryWidget = CreateWidget<UInventoryWidget>(
            GetWorld(),
            InventoryWidgetClass
        );

        if (InventoryWidget)
        {
            InventoryWidget->AddToViewport();

            InventoryWidget->InitializeInventory(Inventory);
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

    if (DetectionWidgetClass)
    {
        DetectionWidget = CreateWidget<UDetectionWidget>(
            GetWorld(),
            DetectionWidgetClass
        );

        if (DetectionWidget)
        {
            DetectionWidget->AddToViewport();
        }
    }

    GetWorldTimerManager().SetTimer(
        DetectionUITimer,
        this,
        &APlayerCharacter::UpdateDetectionUI,
        0.1f,
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
        EnhancedInput->BindAction(SneakAction, ETriggerEvent::Started, this, &APlayerCharacter::SneakStart);
        EnhancedInput->BindAction(SneakAction, ETriggerEvent::Completed, this, &APlayerCharacter::SneakEnd);
        EnhancedInput->BindAction(IA_InventoryScroll,ETriggerEvent::Triggered,this,&APlayerCharacter::OnInventoryScroll);
        EnhancedInput->BindAction(IA_UseItem,ETriggerEvent::Started,this,&APlayerCharacter::StartUseItem);
        EnhancedInput->BindAction(IA_UseItem, ETriggerEvent::Completed, this, &APlayerCharacter::ReleaseUseItem);
    }

   
}

void APlayerCharacter::Tick(float DeltaTime)
{
    Super::Tick(DeltaTime);

    if (bChargingThrow)
    {
        ThrowChargeTime += DeltaTime;

        float Rate =
            ThrowChargeTime / 2.f;

        CurrentThrowPower =
            FMath::Lerp(
                800.f,
                1500.f,
                Rate
            );

        
    }
}

void APlayerCharacter::Move(const FInputActionValue& Value)
{
    FVector2D MovementVector = Value.Get<FVector2D>();
   
    if (bIsCrouchingStealth)
    {
        MovementVector = Value.Get<FVector2D>()/3;
    }
    else
    {
        MovementVector = Value.Get<FVector2D>();
    }


    AddMovementInput(GetActorForwardVector(), MovementVector.Y);
    AddMovementInput(GetActorRightVector(), MovementVector.X);
   
    MakeFootstepNoise();

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
        if (Item&&!Item->ImpossiblePick)//Hitしたのがアイテムだったら
        {
            Item->OnPickedUp(this);//取得を実行
        }
    }
}

void APlayerCharacter::SneakStart()
{
    bIsCrouchingStealth = true;
    UE_LOG(LogTemp, Warning, TEXT("Sneak ON!!."));
}

void APlayerCharacter::SneakEnd()
{
    bIsCrouchingStealth = false;
    UE_LOG(LogTemp, Warning, TEXT("Sneak END."));
}

void APlayerCharacter::OnInventoryScroll(const FInputActionValue& Value)
{
    float Scroll = Value.Get<float>();

    if (!InventoryWidget) return;

    InventoryWidget->MoveSelection((int32)Scroll);
}

void APlayerCharacter::OnUseItem()
{
    if (!bCanUseItem) return;

    bCanUseItem = false;

    if (InventoryWidget)
    {
        InventoryWidget->UseSelectedItem();
        UE_LOG(LogTemp, Warning, TEXT("Use Item Player"));
    }

    // すぐ戻す
    GetWorldTimerManager().SetTimer(
        ItemUseTimerHandle,
        this,
        &APlayerCharacter::ResetUse,
        0.1f,
        false
    );
}

void APlayerCharacter::ResetUse()
{
    bCanUseItem = true;
}

void APlayerCharacter::TraceForItem()
{
    FHitResult Hit;
    bool bHit = PerformCameraLineTrace(Hit, LineTraceEnd);

    ABaseItem* HitItem = bHit ? Cast<ABaseItem>(Hit.GetActor()) : nullptr;

    if (HitItem != CurrentItem)
    {
        //// 前のアイテムのハイライト解除
        //if (CurrentItem)
        //{
        //    CurrentItem->SetHighlight(false);
        //}

        CurrentItem = HitItem;

        /* //新しいアイテムをハイライト
        if (CurrentItem)
        {
            CurrentItem->SetHighlight(true);
        }*/
        
            UpdateInteractUI(CurrentItem);
        
        
    }
}

void APlayerCharacter::UpdateInteractUI(ABaseItem* NewItem)
{
   
        if (!InteractWidget) return;

        if (NewItem&&!NewItem->ImpossiblePick)
        {
            FString Text = NewItem->ItemData.ItemName.ToString() + NewItem->InteractText.ToString();

            InteractWidget->SetInteractText(Text);
            InteractWidget->SetVisibility(ESlateVisibility::Visible);
        }
        else
        {
            InteractWidget->SetVisibility(ESlateVisibility::Hidden);
        }
    
}



float APlayerCharacter::GetMaxEnemyDetection() const
{
    float MaxValue = 0.f;

    TArray<AActor*> FoundControllers;
    UGameplayStatics::GetAllActorsOfClass(
        GetWorld(),
        AEnemyAIController::StaticClass(),
        FoundControllers
    );

    for (AActor* Actor : FoundControllers)
    {
        AEnemyAIController* AI = Cast<AEnemyAIController>(Actor);
        if (!AI) continue;

        float Value = AI->GetDetectionPercent();
        MaxValue = FMath::Max(MaxValue, Value);
    }

    return MaxValue;
}

void APlayerCharacter::UpdateDetectionUI()
{
    if (!DetectionWidget) return;

    float MaxValue = GetMaxEnemyDetection();
    DetectionWidget->UpdateDetection(MaxValue);
}

void APlayerCharacter::MakeFootstepNoise()
{
    float Loudness = FootstepLoudness;
    
    // しゃがみ時は小さく
    if (bIsCrouchingStealth)
    {
        Loudness *= 0.0f;
    }

    // 移動速度で音量変化
    float Speed = GetVelocity().Size();

    Loudness *= FMath::Clamp(Speed / 600.f, 0.2f, 1.5f);

    UAISense_Hearing::ReportNoiseEvent(
        GetWorld(),
        GetActorLocation(),
        Loudness,
        this,
        FootstepRange,
        FName("FootStep")
    );
}

//投擲関連

void APlayerCharacter::StartUseItem()
{
    UE_LOG(LogTemp, Warning, TEXT("Use Item Pr"));
    if (!InventoryWidget) return;

    EItemType Type =
        InventoryWidget->GetSelectedItemType();

    if (Type == EItemType::TRAPTHROW)
    {
        bChargingThrow = true;
        ThrowChargeTime = 0.f;
        UE_LOG(LogTemp, Warning, TEXT("Use Item T"));
    }
    else
    {
        UE_LOG(LogTemp, Warning, TEXT("Use Item D"));
        OnUseItem();
    }
}

void APlayerCharacter::ReleaseUseItem()
{
    if (bChargingThrow)
    {
        bChargingThrow = false;

        OnUseItem();
    }
}

//void APlayerCharacter::StartThrowCharge()
//{
//    bChargingThrow = true;
//
//    ThrowChargeTime = 0.f;
//}
//
//void APlayerCharacter::ReleaseThrow()
//{
//    bChargingThrow = false;
//
//    float ChargeRate =
//        ThrowChargeTime / MaxChargeTime;
//
//    float ThrowPower =
//        FMath::Lerp(
//            MinThrowPower,
//            MaxThrowPower,
//            ChargeRate
//        );
//
//    
//}

//void APlayerCharacter::ThrowCurrentItem(
//    float ThrowPower
//)
//{
//    FVector SpawnLocation =
//        GetActorLocation()
//        + GetActorForwardVector() * 100.f;
//
//    FRotator SpawnRotation =
//        GetControlRotation();
//
//    AThrowItem* SpawnedItem =
//        GetWorld()->SpawnActor<AThrowItem>(
//            ThrowItemClass,
//            SpawnLocation,
//            SpawnRotation
//        );
//
//    if (SpawnedItem)
//    {
//        FVector ThrowDirection =
//            GetControlRotation().Vector();
//
//        ThrowDirection.Z += 0.25f;
//
//        ThrowDirection.Normalize();
//
//        SpawnedItem->Throw(
//            ThrowDirection,
//            ThrowPower
//        );
//    }
//}