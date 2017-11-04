// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "BallBoy.h"
#include "CoreMinimal.h"
#include "UObject/Class.h"
#include "GameFramework/Controller.h"
#include "GameFramework/GameMode.h"
#include "BBGameMode.generated.h"

/**
 * 
 */
UCLASS()
class BALLBRAWL_API ABBGameMode : public AGameMode
{
	GENERATED_BODY()

public:

	ABBGameMode(const FObjectInitializer& ObjectInitializer);
	virtual void PostLogin(APlayerController* NewPlayer) override;
	UClass* GetDefaultPawnClassForController_Implementation(AController* InController) override;
	virtual void BeginPlay() override;
	/*virtual void StartPlay() override;*/

private:
	void SpawnBalls();

public:

	static const int BACKGROUND_RADIUS = 242;

private:
	//UPROPERTY(EditDefaultsOnly)
	TArray<ABrawlBall*> SceneBrawlBalls;

	int PlayersSpawned;
	TArray<ABallBoy*> SceneBallBoys;
	//ABrawlBall* BrawlBall;

};
