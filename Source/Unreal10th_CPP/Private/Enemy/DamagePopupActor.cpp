// Fill out your copyright notice in the Description page of Project Settings.


#include "Enemy/DamagePopupActor.h"
#include "Components/WidgetComponent.h"
#include "Widget/DamagePopupWidget.h"
#include "Framework/SubSystem/ObjectPoolSubsystem.h"

// Sets default values
ADamagePopupActor::ADamagePopupActor()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = false;
	DamagePopupWidgetComponent = CreateDefaultSubobject<UWidgetComponent>(TEXT("DamagePopupWidgetComp"));
	SetRootComponent(DamagePopupWidgetComponent);

	DamagePopupWidgetComponent->SetWidgetSpace(EWidgetSpace::Screen);
	//DamageWidgetComponent->SetCastShadow(false); // 월드스페이스일 때 그림자 발생 방지
	DamagePopupWidgetComponent->SetCollisionEnabled(ECollisionEnabled::NoCollision);
}

void ADamagePopupActor::OnPopupStart(float InDamage)
{
	if (DamagePopupWidget)
	{
		DamagePopupWidget->SetDamage(InDamage);
		DamagePopupWidget->PlayPopupAnimation();
	}
}

void ADamagePopupActor::UseFinish()
{
	UE_LOG(LogTemp, Log, TEXT("UseFinish"));
	if (UGameInstance* GameInstance = GetGameInstance())
	{
		UObjectPoolSubsystem* SubSystem = GameInstance->GetSubsystem<UObjectPoolSubsystem>();
		SubSystem->ReturnPool(this);
	}
}

void ADamagePopupActor::OnSpawn_Implementation()
{
	SetActorHiddenInGame(false);
	//SetActorTickEnabled()
	//SetActorEnableCollision()	

	//FTimerHandle TimerHandle;
	//GetWorldTimerManager().SetTimer(
	//	TimerHandle,
	//	this,
	//	&ADamagePopupActor::UseFinish,
	//	LifeTime);
}

void ADamagePopupActor::OnReturn_Implementation()
{
	SetActorLocation(FVector(0.0f, 0.0f, -10000.0f));
	SetActorHiddenInGame(true);
}

// Called when the game starts or when spawned
void ADamagePopupActor::BeginPlay()
{
	Super::BeginPlay();

	DamagePopupWidget = Cast<UDamagePopupWidget>(DamagePopupWidgetComponent->GetWidget());
	DamagePopupWidget->OnPopUpAnimationFinished.BindUObject(this, &ADamagePopupActor::UseFinish);
}

// Called every frame
void ADamagePopupActor::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

