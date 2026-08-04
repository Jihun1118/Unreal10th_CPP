// Fill out your copyright notice in the Description page of Project Settings.


#include "Enemy/DamagePopupActor.h"
#include "Components/WidgetComponent.h"
#include "Widget/DamagePopupWidget.h"

// Sets default values
ADamagePopupActor::ADamagePopupActor()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = false;
	DamagePopupWidgetComponent = CreateDefaultSubobject<UWidgetComponent>(TEXT("DamagePopupWidgetComp"));
	SetRootComponent(DamagePopupWidgetComponent);

	DamagePopupWidgetComponent->SetWidgetSpace(EWidgetSpace::Screen);
	//DamageWidgetComponent->SetCastShadow(false); // 월드스페이스일 때 그림자 발생 방지
}

void ADamagePopupActor::OnPopupStart(float InDamage)
{
	if (DamagePopupWidget)
	{
		DamagePopupWidget->SetDamage(InDamage);
		DamagePopupWidget->PlayPopupAnimation();
	}
}

// Called when the game starts or when spawned
void ADamagePopupActor::BeginPlay()
{
	Super::BeginPlay();
	DamagePopupWidget = Cast<UDamagePopupWidget>(DamagePopupWidgetComponent->GetWidget());
}

// Called every frame
void ADamagePopupActor::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

