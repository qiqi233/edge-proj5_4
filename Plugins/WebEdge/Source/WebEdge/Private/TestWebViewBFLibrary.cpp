// Fill out your copyright notice in the Description page of Project Settings.


#include "TestWebViewBFLibrary.h"

#include "SWebView.h"

void UTestWebViewBFLibrary::TestSWebView(const FString& URL)
{
	TSharedPtr<SWindow> Window =
		SNew(SWindow).AutoCenter(EAutoCenter::PrimaryWorkArea)
		.ClientSize(FVector2D(500,400));
	FSlateApplication::Get().AddWindow(Window.ToSharedRef());
	Window->SetContent(SNew(SWebView,Window.ToSharedRef())
	.bIsActiveMouse(false)
	.bIsActiveMouse(true)
	.URL(URL));

}
