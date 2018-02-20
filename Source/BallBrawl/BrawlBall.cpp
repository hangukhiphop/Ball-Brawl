// Fill out your copyright notice in the Description page of Project Settings.

#include "BrawlBall.h"
#include "BallBoy.h"
#include "PlasmaBarrel.h"
#include "UnrealNetwork.h"

// Sets default values
ABrawlBall::ABrawlBall()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	bNetLoadOnClient = false;
	bReplicates = true;
	bReplicateMovement = true;

	BoundingSphere = CreateDefaultSubobject<USphereComponent>(TEXT("SphereCollider"));
	RootComponent = BoundingSphere;
	BoundingSphere->InitSphereRadius(35.0f);

	BoundingSphere->BodyInstance.SetCollisionProfileName("Ball");
	BoundingSphere->BodyInstance.SetCollisionEnabled(ECollisionEnabled::QueryOnly);
	OnActorHit.AddDynamic(this, &ABrawlBall::OnHit);
	
	SpriteComponent = CreateDefaultSubobject<UPaperSpriteComponent>(TEXT("Sprite"));
	SpriteComponent->SetupAttachment(RootComponent);

	InertialMovementComponent = CreateDefaultSubobject<UInertialMovementComponent>(TEXT("InertialMovementComponent"));
	InertialMovementComponent->SetUpdatedComponent(BoundingSphere);
	InertialMovementComponent->SetIsReplicated(true);
}

void ABrawlBall::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
}

FVector ABrawlBall::GetVelocity() const
{
	return InertialMovementComponent->Velocity;
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
	if (OtherActor->IsA(ABallBoy::StaticClass()) && OtherActor != GetParentActor())
	{
		Cast<ABallBoy>(OtherActor)->CatchBall(this);
	}
	else if (OtherActor->IsA(APlasmaBarrel::StaticClass()))
	{
		if (Role == ROLE_Authority)
		{
			Cast<APlasmaBarrel>(OtherActor)->OnHit(this, 1);
			OnBounce(Hit);
		}
	}
	else
	{
		if (Role == ROLE_Authority)
		{
			OnBounce(Hit);
		}
	}
}

void ABrawlBall::OnBounce(const FHitResult & Hit)
{
	InertialMovementComponent->BounceSurface(Hit.ImpactNormal);
}

void ABrawlBall::SetCollisionResponseToChannel(ECollisionChannel Channel, ECollisionResponse NewResponse)
{
	BoundingSphere->SetCollisionResponseToChannel(Channel, NewResponse);
}

void ABrawlBall::OnCatch(USceneComponent* const CatcherLocation)
{
	SetCollisionResponseToChannel(ECollisionChannel::ECC_GameTraceChannel1, ECollisionResponse::ECR_Ignore);
	if (InertialMovementComponent != nullptr)
	{
		InertialMovementComponent->UpdateVelocity(FVector::ZeroVector);
	}
}

void ABrawlBall::OnThrow(const int Color, const FVector& Direction)
{
	if (InertialMovementComponent != nullptr)
	{
		InertialMovementComponent->UpdateVelocity(Direction);
	}
}

