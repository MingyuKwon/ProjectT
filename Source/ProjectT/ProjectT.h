// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"

#define RED_STENCIL 250
#define BLUE_STENCIL 251
#define WHITE_STENCIL 252

#define TRACE_LENGTH 10000
#define ECC_FloorTraceChannel ECC_GameTraceChannel1
#define ECC_BoardTraceChannel ECC_GameTraceChannel2
#define ECC_OffencerFieldTraceChannel ECC_GameTraceChannel3
#define ECC_DeffenceFieldTraceChannel ECC_GameTraceChannel4
#define ECC_BulletTraceChannel ECC_GameTraceChannel5


UENUM(BlueprintType)
enum class EObjectType : uint8
{
	EOT_NONE,
	EOT_ATTACKER,
	EOT_DEFENDER,

	EOT_MAX
};


