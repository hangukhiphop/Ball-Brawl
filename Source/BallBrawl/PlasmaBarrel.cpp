// Fill out your copyright notice in the Description page of Project Settings.

#include "PlasmaBarrel.h"
#include "BBGameMode.h"
#include <Runtime/Engine/Classes/Engine/Engine.h>

// Sets default values
APlasmaBarrel::APlasmaBarrel()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;	
	bReplicates = true;
	bNetLoadOnClient = true;

	CapsuleComponent = CreateDefaultSubobject<UCapsuleComponent>(TEXT("Collider"));
	RootComponent = CapsuleComponent;
	SpriteComponent = CreateDefaultSubobject<UPaperSpriteComponent>(TEXT("Sprite"));
	SpriteComponent->SetupAttachment(RootComponent);
}

void APlasmaBarrel::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
}

// Called when the game starts or when spawned
void APlasmaBarrel::BeginPlay()
{
	Super::BeginPlay();

	UE_LOG(LogClass, Warning, TEXT("PlasmaBarrel BeginPlay: %d"), (int)Role);
	if (Role == ROLE_Authority)
	{
		MaxHealth = ((ABBGameMode*)GetWorld()->GetAuthGameMode())->GetHealthPerTeam();
		CurrentHealth = MaxHealth;
	}

	CapsuleComponent->SetCollisionResponseToChannel(ECollisionChannel::ECC_GameTraceChannel1, ECollisionResponse::ECR_Block);
}

// Called every frame
void APlasmaBarrel::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void APlasmaBarrel::OnHit(ABrawlBall* Ball, int Damage)
{
	GEngine->AddOnScreenDebugMessage(-1, 15.0f, FColor::Yellow, GetName() + TEXT(" Hit ") + FString::FromInt((int)Role) + TEXT(" HP: ") + FString::FromInt(CurrentHealth));
	CurrentHealth -= Damage;
	if (CurrentHealth <= 0)
	{
		OnZeroHealth();
	}
}

void APlasmaBarrel::OnZeroHealth()
{
	CapsuleComponent->SetCollisionResponseToChannel(ECollisionChannel::ECC_GameTraceChannel1, ECollisionResponse::ECR_Ignore);
	GEngine->AddOnScreenDebugMessage(-1, 15.0f, FColor::Yellow, GetName() + TEXT(" Destroyed ") + FString::FromInt((int)Role));
}

