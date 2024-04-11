// Fill out your copyright notice in the Description page of Project Settings.


#include "HUD/HealthBarWidget.h"
#include "Components/ProgressBar.h"


void UHealthBarWidget::SetHealthBarPercent(float percent)
{
	if (HealthBar)
	{
		HealthBar->SetPercent(percent);
	}
}
