// Fill out your copyright notice in the Description page of Project Settings.

#include "BrawlBall.h"
#include "BallBoy.h"
#include "UnrealNetwork.h"

// Sets default values
ABrawlBall::ABrawlBall()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	bNetLoadOnClient = false;
	bReplicates = true;

	BoundingSphere = CreateDefaultSubobject<USphereComponent>(TEXT("SphereCollider"));
	RootComponent = BoundingSphere;
	BoundingSphere->InitSphereRadius(35.0f);

	BoundingSphere->BodyInstance.SetCollisionProfileName("Ball");
	BoundingSphere->BodyInstance.SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);

	OnActorHit.AddDynamic(this, &ABrawlBall::OnHit);
	
	SpriteComponent = CreateDefaultSubobject<UPaperSpriteComponent>(TEXT("Sprite"));
	SpriteComponent->SetupAttachment(RootComponent);

	InertialMovementComponent = CreateDefaultSubobject<UInertialMovementComponent>(TEXT("InertialMovementComponent"));
	InertialMovementComponent->SetUpdatedComponent(BoundingSphere);
	InertialMovementComponent->SetIsReplicated(true);

	/*OrbitalMovementComponent = CreateDefaultSubobject<UOrbitalMovementComponent>(TEXT("RotatingMovementComponent"));
	OrbitalMovementComponent->SetUpdatedComponent(BoundingSphere);
	OrbitalMovementComponent->SetIsReplicated(true);*/
}

void ABrawlBall::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
}

// Called when the game starts or when spawned
void ABrawlBall::BeginPlay()
{
	Super::BeginPlay();
}

// Called every frame
void ABrawlBall::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

void ABrawlBall::OnHit(AActor * SelfActor, AActor * OtherActor, FVector NormalImpulse, const FHitResult& Hit)
{
	UE_LOG(LogClass, Warning, TEXT("OnHit: %d"), (int)Role);
	if (OtherActor->IsA(ABallBoy::StaticClass()) && OtherActor != GetParentActor())
	{
		if (Role == ROLE_Authority)
		{
			Cast<ABallBoy>(OtherActor)->CatchBall(this);
			OnCatch(OtherActor->GetActorLocation());
		}
		else 
		{
		}
	}
}

void ABrawlBall::OnCatch(const FVector& CatcherLocation)
{
	if (InertialMovementComponent != nullptr)
	{
		InertialMovementComponent->UpdateVelocity(FVector::ZeroVector);
	}
	if (OrbitalMovementComponent != nullptr)
	{
		OrbitalMovementComponent->UpdatePivotTranslation(CatcherLocation - GetActorLocation());
	}
	SetActorEnableCollision(false);
}

void ABrawlBall::OnThrow(const int Color, const FVector& Direction)
{
	if (OrbitalMovementComponent != nullptr)
	{
		OrbitalMovementComponent->UpdatePivotTranslation(FVector::ZeroVector);
	}
	if (InertialMovementComponent != nullptr)
	{
		InertialMovementComponent->UpdateVelocity(Direction);
	}
	SetActorEnableCollision(true);
}

void ABrawlBall::Spin(float AngVel)
{
	if (OrbitalMovementComponent != nullptr)
	{
		OrbitalMovementComponent->SetAngularSpeed(AngVel);
	}
}

#pragma region Client Member Functions

//void ABrawlBall::SetHolder(ABallBoy& NewHolder)
//{
//	Holder = &NewHolder;
//
//	if (Role < ROLE_Authority)
//	{
//		Server_SetHolder(Holder);
//	}
//}

#pragma endregion


#pragma region Server Implementations

//void ABrawlBall::Server_SetHolder_Implementation(ABallBoy* NewHolder)
//{
//	SetHolder(*NewHolder);
//}

#pragma endregion


#pragma region Server Validations

//bool ABrawlBall::Server_SetHolder_Validate(ABallBoy* NewHolder)
//{
//	return true;
//}

#pragma endregion

