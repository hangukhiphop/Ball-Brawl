// Fill out your copyright notice in the Description page of Project Settings.

#include "BallBoy.h"
#include "UnrealNetwork.h"
#include "Core.h"

#pragma region Constructor(s)

// Sets default values
ABallBoy::ABallBoy()
{
	AutoPossessPlayer = EAutoReceiveInput::Disabled;

	PrimaryActorTick.bCanEverTick = true;
	bReplicates = true;

	BoundingSphere = CreateDefaultSubobject<USphereComponent>(TEXT("SphereCollider"));
	RootComponent = BoundingSphere;
	BoundingSphere->InitSphereRadius(15.0f);
	BoundingSphere->BodyInstance.SetCollisionProfileName("BallBoy");
	BoundingSphere->BodyInstance.SetCollisionEnabled(ECollisionEnabled::QueryOnly);
	
	SpriteComponent = CreateDefaultSubobject<UPaperSpriteComponent>(TEXT("Sprite"));	
	SpriteComponent->SetCollisionProfileName("NoCollision");

	InertialMovementComponent = CreateDefaultSubobject<UInertialMovementComponent>(TEXT("InertialMovementComponent"));
	InertialMovementComponent->SetUpdatedComponent(BoundingSphere);
	InertialMovementComponent->SetIsReplicated(true);

	SpringArmComponent = CreateDefaultSubobject<USpringArmComponent>(TEXT("SpringArmComponent"));
	SpringArmComponent->SetupAttachment(RootComponent);
	SpringArmComponent->TargetArmLength = 0.0f;
	SpringArmComponent->bUsePawnControlRotation = false;	
}

#pragma endregion


#pragma region Inherited from AActor

void ABallBoy::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	DOREPLIFETIME(ABallBoy, HeldBall);
}

// Called when the game starts or when spawned
void ABallBoy::BeginPlay()
{
	Super::BeginPlay();
	bRotateBall = false;
	TargetAngularDistance = 0.0f;
}

// Called every frame
void ABallBoy::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);	

	if (!IsHoldingBall()) { return; }

	if (bTickMoveBall)
	{
		float x = GetInputAxisValue("HoldBall_X");
		float y = GetInputAxisValue("HoldBall_Y");
		if (!(FMath::Abs(x) < .99f && FMath::Abs(y) < .99f))
		{
			SetHeldBallDirection(x, y);
		}

		bTickMoveBall = false;
	}

	if (bRotateBall)
	{
		float DeltaAngle = DeltaTime * BallRotationSpeed * FMath::Sign(TargetAngularDistance);
		const FQuat DeltaQuat = FQuat(FVector::RightVector, FMath::DegreesToRadians(DeltaAngle));
		SpringArmComponent->AddWorldRotation(DeltaQuat);
		TargetAngularDistance -= DeltaAngle;

		if (TargetAngularDistance * DeltaAngle < 0.0f)
		{
			bRotateBall = false;
		}
	}
}

// Called to bind functionality to input
void ABallBoy::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);
	PlayerInputComponent->BindAxis("Move_Horizontal", this, &ABallBoy::MoveX);
	PlayerInputComponent->BindAxis("Move_Vertical", this, &ABallBoy::MoveY);
	PlayerInputComponent->BindAxis("HoldBall_X", this, &ABallBoy::SetBall_X);
	PlayerInputComponent->BindAxis("HoldBall_Y", this, &ABallBoy::SetBall_Y);
	PlayerInputComponent->BindAxis("Rotate", this, &ABallBoy::RotateBall);
	PlayerInputComponent->BindAction("Throw_Ball", IE_Pressed, this, &ABallBoy::ThrowBall);
}

#pragma endregion


#pragma region Member Functions

bool ABallBoy::IsHoldingBall() const
{
	return HeldBall != nullptr;
}

void ABallBoy::MoveX_Implementation(float Magnitude)
{
	if (Magnitude != InertialMovementComponent->Velocity.X)
	{
		InertialMovementComponent->UpdateVelocity(FVector(Magnitude, 0, InertialMovementComponent->Velocity.Z));
	}
}
void ABallBoy::SetBall_X_Implementation(float Magnitude)
{
	bTickMoveBall = true;
}

void ABallBoy::MoveY_Implementation(float Magnitude)
{
	if (Magnitude != InertialMovementComponent->Velocity.Z)
	{
		InertialMovementComponent->UpdateVelocity(FVector(InertialMovementComponent->Velocity.X, 0, Magnitude));
	}
}
void ABallBoy::SetBall_Y_Implementation(float Magnitude)
{
	bTickMoveBall = true;
}

void ABallBoy::CatchBall(ABrawlBall* Ball)
{
	const FVector MyVelocity = InertialMovementComponent->Velocity;
	const FVector BallVelocity = Ball->GetVelocity();
	const FVector BoyToBall = Ball->GetActorLocation() - GetActorLocation();

	if (IsHoldingBall() || FVector::Coincident(MyVelocity, BallVelocity, .01f) && FVector::Coincident(BoyToBall, MyVelocity, .01f) ||  Ball == nullptr)
	{
		return;
	}

	Ball->OnCatch(GetRootComponent());
	HeldBallDistance = BoundingSphere->GetScaledSphereRadius() + Ball->BoundingSphere->GetScaledSphereRadius();
	FVector Diff = Ball->GetActorLocation() - GetActorLocation();
	SpringArmComponent->SetWorldRotation(Diff.ToOrientationRotator());
	Ball->GetRootComponent()->AttachToComponent(SpringArmComponent, FAttachmentTransformRules(EAttachmentRule::KeepWorld, false), NAME_None);

if (Role == ROLE_Authority)
{
	HeldBall = Ball;
	Client_SetHeldBall(HeldBall);
}
}
void ABallBoy::Server_CatchBall_Implementation(ABrawlBall* Ball)
{
	if (Ball != nullptr)
	{
		CatchBall(Ball);
	}
}
void ABallBoy::Client_SetHeldBall_Implementation(ABrawlBall * Ball)
{
	HeldBall = Ball;
	HeldBallDistance = BoundingSphere->GetScaledSphereRadius() + Ball->BoundingSphere->GetScaledSphereRadius();
}

void ABallBoy::ThrowBall()
{
	if (!IsHoldingBall()) { return; }

	if (Role == ROLE_Authority)
	{
		FVector Dir = (HeldBall->GetActorLocation() - GetActorLocation()).GetSafeNormal();
		HeldBall->OnThrow(0, Dir);
	}
	else if (Role == ROLE_AutonomousProxy)
	{
		Server_ThrowBall();
	}

	HeldBall->DetachFromActor(FDetachmentTransformRules::KeepWorldTransform);
	HeldBall->SetCollisionResponseToChannel(ECollisionChannel::ECC_GameTraceChannel1, ECollisionResponse::ECR_Block);
	HeldBall = nullptr;
	HeldBallDistance = 0;
}
void ABallBoy::Server_ThrowBall_Implementation()
{
	NetMulticast_ThrowBall(HeldBall);
}
void ABallBoy::NetMulticast_ThrowBall_Implementation(ABrawlBall* Ball)
{
	ThrowBall();
}

void ABallBoy::SetHeldBallDirection(float xOffset, float yOffset)
{
	if (!IsHoldingBall()) { return; }

	FVector NewDirection = xOffset * FVector::ForwardVector + yOffset * FVector::UpVector;
	NewDirection.Normalize();

	const FVector OldDirection = SpringArmComponent->GetForwardVector();

	if (NewDirection.Equals(OldDirection, .1f) || NewDirection.Equals(TargetBallDirection, .1f))
	{
		return;
	}
	
	const int Direction = FMath::Sign(FMath::Asin(FVector::CrossProduct(OldDirection, NewDirection).Y));
	const float Angle = FMath::RadiansToDegrees(FMath::Acos(FVector::DotProduct(OldDirection, NewDirection)));
	TargetAngularDistance = Direction * Angle;

	bRotateBall = true;
	TargetBallDirection = NewDirection;

	if(Role == ROLE_AutonomousProxy)
	{		
		Server_SetHeldBallDirection(NewDirection);
	}
}
void ABallBoy::Server_SetHeldBallDirection_Implementation(FVector Loc)
{
	if (!IsHoldingBall()) { return; }
	//HeldBall->SpinTo(Loc);
	//HeldBall->SetActorLocation(Loc);
}

void ABallBoy::RotateBall_Implementation(float Magnitude)
{
	if (Magnitude == 0.0f || !IsHoldingBall()) { return; }

	Server_SetSpinDirection(Magnitude);
}
void ABallBoy::Server_SetSpinDirection_Implementation(float Magnitude)
{
	HeldBall->Spin(Magnitude);
}

#pragma endregion

bool ABallBoy::Server_CatchBall_Validate(ABrawlBall* Ball)
{
	return true;
}

bool ABallBoy::Server_ThrowBall_Validate()
{
	return true;
}

bool ABallBoy::Server_SetSpinDirection_Validate(float Magnitude)
{
	return true;
}

bool ABallBoy::Server_SetHeldBallDirection_Validate(FVector Loc)
{
	return true;
}