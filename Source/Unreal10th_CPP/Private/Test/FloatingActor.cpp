// Fill out your copyright notice in the Description page of Project Settings.


#include "Test/FloatingActor.h"

// Sets default values
AFloatingActor::AFloatingActor()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	USceneComponent* Root = CreateDefaultSubobject<USceneComponent>(TEXT("Root"));
	SetRootComponent(Root);

	Mesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("MainMesh"));
	Mesh->SetupAttachment(RootComponent);
}

// Called when the game starts or when spawned
void AFloatingActor::BeginPlay()
{
	Super::BeginPlay();

	if (Mesh)
	{
		HeightOffset = Mesh->GetRelativeLocation().Z;
	}
}

// Called every frame
void AFloatingActor::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	ElapsedTime += DeltaTime;

	UpdateFloatingMesh();
}

void AFloatingActor::UpdateFloatingMesh()
{		
	float Div = FMath::Max(Duration, 0.001f);
	float InCos = ElapsedTime * UE_PI * 2 / Div;
	float CosValue = FMath::Cos(InCos);			// 1 -> 0 -> -1 -> 0 -> 1 -> ...
	CosValue += 1;								// 2 -> 1 -> 0 -> 1 -> 2 -> ...
	CosValue *= 0.5f;							// 1 -> 0.5 -> 0 -> 0.5 -> 1 -> ...
	CosValue = 1 - CosValue;					// 0 -> 0.5 -> 1 -> 0.5 -> 0 -> ...
	Mesh->SetRelativeLocation(FVector(0, 0, HeightOffset + CosValue * MoveHeight));

	//UE_LOG(LogTemp, Log, TEXT("ElapsedTime : %.2f"), ElapsedTime);
}

