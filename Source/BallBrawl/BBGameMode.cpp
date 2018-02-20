// Fill out your copyright notice in the Description page of Project Settings.

#include "BBGameMode.h"
#include "BBPlayerState.h"
#include "Kismet/GameplayStatics.h"
#include "BallBoyController.h"

ABBGameMode::ABBGameMode(const FObjectInitializer& ObjectInitializer)
	:Super(ObjectInitializer)
{
	DefaultPawnClass = ABallBoy::StaticClass();
	PlayerStateClass = ABBPlayerState::StaticClass();
	PlayerControllerClass = ABallBoyController::StaticClass();
	//GameStateClass = nullptr;
	//SpectatorClass = nullptr; 
	//HUDClass = nullptr;

	PlayersSpawned = 0;
	TArray<AActor*> BallBoyActors;
	UGameplayStatics::GetAllActorsOfClass(this, ABallBoy::StaticClass(), BallBoyActors);
	for(int i = 0; i < BallBoyActors.Num(); i++)
	{
		SceneBallBoys.Add(Cast<ABallBoy>(BallBoyActors[i]));
	}
	
	
}

void ABBGameMode::PostLogin(APlayerController* NewPlayer)
{
	Super::PostLogin(NewPlayer);
	NewPlayer->Possess(SceneBallBoys[PlayersSpawned]);
	PlayersSpawned++;	
}

UClass* ABBGameMode::GetDefaultPawnClassForController_Implementation(AController* InController)
{
	return nullptr;
}

void ABBGameMode::BeginPlay()
{
	Super::BeginPlay();

	TArray<AActor*> BrawlBallActors;
	UGameplayStatics::GetAllActorsOfClass(this, ABrawlBall::StaticClass(), BrawlBallActors);
	for (int i = 0; i < BrawlBallActors.Num(); i++)
	{
		SceneBrawlBalls.Add(Cast<ABrawlBall>(BrawlBallActors[i]));
	}
	
	TArray<AActor*> PlasmaBarrelActors;
	UGameplayStatics::GetAllActorsOfClass(this, ABrawlBall::StaticClass(), PlasmaBarrelActors);
	for (int i = 0; i < PlasmaBarrelActors.Num(); i++)
	{
		ScenePlasmaBarrels.Add(Cast<APlasmaBarrel>(PlasmaBarrelActors[i]));
	}
}

int ABBGameMode::GetHealthPerTeam() const
{
	UE_LOG(LogClass, Warning, TEXT("GameMode %d"), HealthPerTeam);
	return HealthPerTeam;
}

void ABBGameMode::SpawnBalls()
{
	
}