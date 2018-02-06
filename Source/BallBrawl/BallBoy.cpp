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
	//DOREPLIFETIME(ABallBoy, SpringArmComponent);
	//DOREPLIFETIME(ABallBoy, TargetBallDirection);
	DOREPLIFETIME(ABallBoy, BallRotationSpeed);
	DOREPLIFETIME(ABallBoy, TargetAngularDistance);
	DOREPLIFETIME(ABallBoy, bRotateBall);
	DOREPLIFETIME(ABallBoy, HeldBall);
}
// Called when the game starts or when spawned
void ABallBoy::BeginPlay()
{
	Super::BeginPlay();
	bRotateBall = false;
	TargetAngularDistance = 0.0f;
}

void ABallBoy::RotateBall(const float DeltaAngle)
{
	//UE_LOG(LogClass, Warning, TEXT("Rotate Ball 1: %d"), (int)Role);
	if (HeldBall != nullptr && bRotateBall)
	{
		UE_LOG(LogClass, Warning, TEXT("Rotate Ball: %d"), (int)Role);
		const FQuat DeltaQuat = FQuat(FVector::RightVector, FMath::DegreesToRadians(DeltaAngle));
		SpringArmComponent->AddWorldRotation(DeltaQuat);
		TargetAngularDistance -= DeltaAngle;

		if (TargetAngularDistance * DeltaAngle < 0.0f)
		{
			bRotateBall = false;
			TargetAngularDistance = 0;
		}
	}
}

// Called every frame
void ABallBoy::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);	
	
	//UE_LOG(LogClass, Warning, TEXT("HeldBall: 0x%p, %d"), (void*)HeldBall, (int)Role);
	if (IsHoldingBall())
	{
		if (Role = ROLE_AutonomousProxy)
		{
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
		}

		//if (TargetAngularDistance != 0)
		//{
			float DeltaAngle = DeltaTime * BallRotationSpeed * FMath::Sign(TargetAngularDistance);
			//UE_LOG(LogClass, Warning, TEXT("Rotate Ball %d"), (int)Role);
			RotateBall(DeltaAngle);
		//}
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
	//const TArray<USceneComponent*> Children = SpringArmComponent->GetAttachChildren();
	//return Children.Num() != 0;// != nullptr;
	return HeldBall != nullptr;
}

bool ABallBoy::CanCatchBall(ABrawlBall* Ball) const
{
	const FVector MyVelocity = InertialMovementComponent->Velocity;
	const FVector BallVelocity = Ball->GetVelocity();
	const FVector BoyToBall = Ball->GetActorLocation() - GetActorLocation();

	return !(IsHoldingBall() || Ball == nullptr) ||
		(FVector::Coincident(MyVelocity, BallVelocity, .01f) && 
			FVector::Coincident(BoyToBall, MyVelocity, .01f)); //Is Chasing ball from behind		
}

void ABallBoy::MoveX_Implementation(float Magnitude)
{
	if (Magnitude != InertialMovementComponent->Velocity.X)
	{
		InertialMovementComponent->UpdateVelocity(FVector(Magnitude, 0, InertialMovementComponent->Velocity.Z));
	}
}

void ABallBoy::MoveY_Implementation(float Magnitude)
{
	if (Magnitude != InertialMovementComponent->Velocity.Z)
	{
		InertialMovementComponent->UpdateVelocity(FVector(InertialMovementComponent->Velocity.X, 0, Magnitude));
	}
}

void ABallBoy::SetBall_X_Implementation(float Magnitude)
{
	bTickMoveBall = true;
}

void ABallBoy::SetBall_Y_Implementation(float Magnitude)
{
	bTickMoveBall = true;
}

void ABallBoy::CatchBall(ABrawlBall* Ball)
{
	UE_LOG(LogClass, Warning, TEXT("Catch Ball %d"), (int)Role);
	
	
	if (!CanCatchBall(Ball)) { return; }
//	if (Role == ROLE_AutonomousProxy)
	//{
		

	Ball->OnCatch(GetRootComponent());
			HeldBallDistance = BoundingSphere->GetScaledSphereRadius() + Ball->BoundingSphere->GetScaledSphereRadius();
			FVector Diff = Ball->GetActorLocation() - GetActorLocation();
			const FRotator Rotation = Diff.ToOrientationRotator();
			SpringArmComponent->SetWorldRotation(Rotation);

			Ball->GetRootComponent()->AttachToComponent(SpringArmComponent, FAttachmentTransformRules(EAttachmentRule::KeepWorld, false), NAME_None);

			HeldBall = Ball;

			//Server_CatchBall(Ball, Rotation);
		
//	}

	//Ball->SetReplicates(false);
	//HeldBall = Ball;
	//UE_LOG(LogClass, Warning, TEXT("HeldBall: 0x%p, %d"), (void*)HeldBall, (int)Role);
	//if (Role == ROLE_Authority)
	//{
	//	Client_SetHeldBall(Ball);
	//}
}

void ABallBoy::Server_CatchBall_Implementation(ABrawlBall* Ball, const FRotator& Rotation)
{
	UE_LOG(LogClass, Warning, TEXT("Server Catch Ball %d"), (int)Role);
	if (Ball != nullptr && !IsHoldingBall())
	{
		//CatchBall(Ball);
		SpringArmComponent->SetWorldRotation(Rotation); 
		Ball->OnCatch(GetRootComponent());
		Ball->GetRootComponent()->AttachToComponent(SpringArmComponent, FAttachmentTransformRules(EAttachmentRule::KeepWorld, false), NAME_None);
		HeldBall = Ball;
	}
}
void ABallBoy::Client_SetHeldBall_Implementation(ABrawlBall * Ball)
{
	UE_LOG(LogClass, Warning, TEXT("Client Catch Ball %d"), (int)Role);
	if (Ball == nullptr || IsHoldingBall()) { return; }

	HeldBall = Ball;
	HeldBallDistance = BoundingSphere->GetScaledSphereRadius() + Ball->BoundingSphere->GetScaledSphereRadius(); 
	Ball->OnCatch(GetRootComponent());
}

//void ABallBoy::NetMulticast_CatchBall_Implementation(ABrawlBall* Ball, const FRotator& Rotation)
//{
//	UE_LOG(LogClass, Warning, TEXT("NM Catch Ball %d"), (int)Role);
//	if (Ball == nullptr || IsHoldingBall()) { return; }
//
//	CatchBall(Ball);
//}

void ABallBoy::ThrowBall()
{
	UE_LOG(LogClass, Warning, TEXT("Throw Ball %d"), (int)Role);
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
	ThrowBall();
	//NetMulticast_ThrowBall(HeldBall);
}
void ABallBoy::NetMulticast_ThrowBall_Implementation(ABrawlBall* Ball)
{
	ThrowBall();
}

void ABallBoy::SetHeldBallDirection(float xOffset, float yOffset)
{
	UE_LOG(LogClass, Warning, TEXT("Set Held Ball direction %d"), (int)Role);
	if (!IsHoldingBall()) { return; }

	if (Role == ROLE_AutonomousProxy)
	{
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

		Server_SetHeldBallDirection(TargetBallDirection, TargetAngularDistance);
	}
}
void ABallBoy::Server_SetHeldBallDirection_Implementation(const FVector Direction, const float AngularDistance)
{
	UE_LOG(LogClass, Warning, TEXT("Server Held Ball direction %d"), (int)Role);
	if (!IsHoldingBall()) { return; }

	TargetBallDirection = Direction;
	TargetAngularDistance = AngularDistance;
	bRotateBall = true;
	//HeldBall->SpinTo(Loc);
	//HeldBall->SetActorLocation(Loc);
}

#pragma endregion

bool ABallBoy::Server_CatchBall_Validate(ABrawlBall* Ball, const FRotator& Rotation)
{
	return true;
}

bool ABallBoy::Server_ThrowBall_Validate()
{
	return true;
}

bool ABallBoy::Server_SetHeldBallDirection_Validate(const FVector Direction, const float AngularDistance)
{
	return true;
}