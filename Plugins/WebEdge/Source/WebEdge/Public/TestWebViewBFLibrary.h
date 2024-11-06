// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "TestWebViewBFLibrary.generated.h"

/**
 * 
 */
UCLASS()
class WEBEDGE_API UTestWebViewBFLibrary : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()
public:
	UFUNCTION(BlueprintCallable)
	static void TestSWebView(const FString&URL);
};
