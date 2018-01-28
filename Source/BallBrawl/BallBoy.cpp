// Fill out your copyright notice in the Description page of Project Settings.

#include "BallBoy.h"
#include "UnrealNetwork.h"
//#include "PaperSprite.h"
#include "Core.h"

#pragma region Constructor(s)

// Sets default values
ABallBoy::ABallBoy()
{
	AutoPossessPlayer = EAutoReceiveInput::Disabled;

	PrimaryActorTick.bCanEverTick = true;
	bReplicates = true;
	//bReplicateMovement = true;

	BoundingSphere = CreateDefaultSubobject<USphereComponent>(TEXT("SphereCollider"));
	RootComponent = BoundingSphere;
	BoundingSphere->InitSphereRadius(15.0f);

	BoundingSphere->BodyInstance.SetCollisionProfileName("BallBoy");
	BoundingSphere->BodyInstance.SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
	
	SpriteComponent = CreateDefaultSubobject<UPaperSpriteComponent>(TEXT("Sprite"));	

	InertialMovementComponent = CreateDefaultSubobject<UInertialMovementComponent>(TEXT("InertialMovementComponent"));
	InertialMovementComponent->SetUpdatedComponent(BoundingSphere);
	InertialMovementComponent->SetIsReplicated(true);
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
}

// Called every frame
void ABallBoy::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);	

	if (HeldBall == nullptr)
	{
		return;
	}
	float x = GetInputAxisValue("HoldBall_X");
	float y = GetInputAxisValue("HoldBall_Y");
	if (!(x == 0.0f && y == 0.0f))
	{
		SetHeldBallDirection(x, y);
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

void ABallBoy::MoveX(float Magnitude)
{
	InertialMovementComponent->UpdateVelocity(FVector(Magnitude, 0, InertialMovementComponent->Velocity.Z));
}

void ABallBoy::MoveY(float Magnitude)
{
	InertialMovementComponent->UpdateVelocity(FVector(InertialMovementComponent->Velocity.X, 0, Magnitude));
}

void ABallBoy::SetBall_X(float Magnitude)
{

}

void ABallBoy::SetBall_Y(float Magnitude)
{

}

void ABallBoy::RotateBall(float Magnitude)
{
	if (HeldBall == nullptr)
	{
		return;
	}

	//if(Magnitude == HeldBall->GetRootComponent()->)
	Server_SetSpinDirection(Magnitude);
}

void ABallBoy::CatchBall(ABrawlBall* Ball)
{
	UE_LOG(LogClass, Warning, TEXT("Catchball: %d"), (int)Role);
	if (HeldBall != nullptr)
	{
		return;
	}
	//if (Role == ROLE_AutonomousProxy)
	//{

		FAttachmentTransformRules FATRules(EAttachmentRule::KeepWorld, EAttachmentRule::KeepWorld, EAttachmentRule::KeepWorld, false);
		Ball->AttachToActor(this, FATRules);
		HeldBall = Ball;
		HeldBallDistance = BoundingSphere->GetScaledSphereRadius() + HeldBall->BoundingSphere->GetScaledSphereRadius();

		UE_LOG(LogClass, Warning, TEXT("%d: %d"), (int)Role, HeldBall);

	//}
	/*else if (Role == ROLE_Authority)
	{
		Ball->OnCatch(GetActorLocation());
	}*/
}

void ABallBoy::Server_CatchBall_Implementation(ABrawlBall* Ball)
{
	if (Ball != nullptr)
	{
		UE_LOG(LogClass, Warning, TEXT("Balls"));
		Ball->OnCatch(GetActorLocation());
	}
	//CatchBall(HeldBall);
}

void ABallBoy::ThrowBall()
{
	if (HeldBall == nullptr)
	{
		return;
	}		

	FDetachmentTransformRules FDTRules(EDetachmentRule::KeepWorld, EDetachmentRule::KeepWorld, EDetachmentRule::KeepWorld, false);
	FVector Dir = (HeldBall->GetActorLocation() - GetActorLocation()).GetSafeNormal();
	HeldBall->OnThrow(0, Dir);

	HeldBall->DetachFromActor(FDTRules);
	HeldBall = nullptr;
	HeldBallDistance = 0;

	/*if (Role == ROLE_AutonomousProxy)
	{
		Server_ThrowBall();
	}*/	
}

void ABallBoy::SetHeldBallDirection(float xOffset, float yOffset)
{
	if (HeldBall == nullptr)
	{
		return;
	}

	FVector Radius = xOffset * FVector::ForwardVector + yOffset * FVector::UpVector;
	Radius.Normalize();
	Radius *= HeldBallDistance;
	FVector Loc = GetActorLocation() + Radius;
	HeldBall->SetActorLocation(Loc);


	if (Role == ROLE_AutonomousProxy)
	{
		Server_SetHeldBallDirection(Loc);
	}
}

#pragma endregion



void ABallBoy::Server_ThrowBall_Implementation()
{	
	ThrowBall();
}

void ABallBoy::Server_SetSpinDirection_Implementation(float Magnitude)
{
	HeldBall->Spin(Magnitude);
}

void ABallBoy::Server_SetHeldBallDirection_Implementation(FVector Loc)
{
	if (HeldBall == nullptr)
		return;
	HeldBall->SetActorLocation(Loc);
}

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