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
	BoundingSphere->BodyInstance.SetCollisionEnabled(ECollisionEnabled::QueryOnly);
	BoundingSphere->SetIsReplicated(true);
	OnActorHit.AddDynamic(this, &ABrawlBall::OnHit);
	
	SpriteComponent = CreateDefaultSubobject<UPaperSpriteComponent>(TEXT("Sprite"));
	SpriteComponent->SetupAttachment(RootComponent);

	InertialMovementComponent = CreateDefaultSubobject<UInertialMovementComponent>(TEXT("InertialMovementComponent"));
	InertialMovementComponent->SetUpdatedComponent(BoundingSphere);
	InertialMovementComponent->SetIsReplicated(true);

	/*OrbitalMovementComponent = CreateDefaultSubobject<UOrbitalMovementComponent>(TEXT("RotatingMovementComponent"));
	OrbitalMovementComponent->SetUpdatedComponent(BoundingSphere);
	OrbitalMovementComponent->SetIsReplicated(true);*/

	/*PlanarRotatingMovementComponent = CreateDefaultSubobject<UPlanarRotatingMovementComponent>(TEXT("PlanarRotatingMovementComponent"));
	PlanarRotatingMovementComponent->SetUpdatedComponent(BoundingSphere);
	PlanarRotatingMovementComponent->SetIsReplicated(true);*/
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
		UE_LOG(LogClass, Warning, TEXT("Hit %d"), (int)Role);
		Cast<ABallBoy>(OtherActor)->CatchBall(this);
	}
	else
	{
		if (Role == ROLE_Authority)
		{
			InertialMovementComponent->BounceSurface(Hit.ImpactNormal);
		}
	}
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
	//if (OrbitalMovementComponent != nullptr)
	//{
	//	//OrbitalMovementComponent->UpdatePivotTranslation(CatcherLocation);
	//}

	//UE_LOG(LogClass, Warning, TEXT("%p"), PlanarRotatingMovementComponent);
	//if (PlanarRotatingMovementComponent != nullptr)
	//{
	//	//UE_LOG(LogClass, Warning, TEXT("%d Catcher: %f %f, Ball: %f %f"), (int)Role, CatcherLocation.X, CatcherLocation.Z, GetActorLocation().X, GetActorLocation().Z);
	//	PlanarRotatingMovementComponent->SetPivot(CatcherLocation);
	//}
	UE_LOG(LogClass, Warning, TEXT("Disable Collision: %d"), (int)Role);
	SetCollisionResponseToChannel(ECollisionChannel::ECC_GameTraceChannel1, ECollisionResponse::ECR_Ignore);
	//bReplicateMovement = false;
	//bReplicates = false;
}

void ABrawlBall::OnThrow(const int Color, const FVector& Direction)
{
	if (InertialMovementComponent != nullptr)
	{
		InertialMovementComponent->UpdateVelocity(Direction);
	}
	/*if (OrbitalMovementComponent != nullptr)
	{
		OrbitalMovementComponent->UpdatePivotTranslation(FVector::ZeroVector);
	}*/
	SetCollisionResponseToChannel(ECollisionChannel::ECC_GameTraceChannel1, ECollisionResponse::ECR_Block);
}

void ABrawlBall::Spin(float AngVel)
{
	/*if (OrbitalMovementComponent != nullptr)
	{
		OrbitalMovementComponent->SetAngularSpeed(AngVel);
	}*/
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

