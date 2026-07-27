// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "FloatingActor.generated.h"

UCLASS()
class UNREAL10TH_CPP_API AFloatingActor : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AFloatingActor();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// 메시의 움직임을 결정할 함수
	virtual void UpdateFloatingMesh();

	// 누적시간 확인용 함수
	inline float GetElapsedTime() const { return ElapsedTime; }
	// 높이 오프셋 확인용 함수
	inline float GetHeightOffset() const { return HeightOffset; }
protected:
	// 위아래 왕복에 걸리는 시간
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float Duration = 5.0f;
	
	// 위아래 움직이는 높이
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float MoveHeight = 300.0f;

private:
	// 누적 시간
	float ElapsedTime = 0.0f;

	// 메시의 높이 오프셋
	float HeightOffset = 0.0f;

protected:
	UPROPERTY(VisibleAnywhere)
	TObjectPtr<UStaticMeshComponent> Mesh = nullptr;
};
