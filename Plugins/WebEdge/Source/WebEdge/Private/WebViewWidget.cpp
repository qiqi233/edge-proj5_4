// Fill out your copyright notice in the Description page of Project Settings.


#include "WebViewWidget.h"
#include "SWebView.h"


UWebViewWidget::UWebViewWidget(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
	, bIs3DWebUI(false)
	, bIsActiveDebugTool(false)
	, bIsActiveMouse(true)
	, URL(TEXT("https://space.bilibili.com/43501343"))
{
	
}

void UWebViewWidget::SetIsActiveDebugTool(bool bIsDebugTool)
{
	bIsActiveDebugTool = bIsDebugTool;
	if(MyWebView)
	{
		MyWebView->SetIsActiveDebugTool(bIsActiveDebugTool);
	}
}

bool UWebViewWidget::GetbIsActiveDebugTool() const
{
	return bIsActiveDebugTool;
}

void UWebViewWidget::SetIsActiveMouse(bool bIsActiveMouseIn)
{
	bIsActiveMouse = bIsActiveMouseIn;
	if(MyWebView)
	{
		MyWebView->SetIsActiveMouse(bIsActiveMouse);
	}
}

bool UWebViewWidget::GetIsActiveMouse() const
{
	return bIsActiveMouse;
}

void UWebViewWidget::LoadURL(const FString& InURL)
{
	URL = InURL;
	if(MyWebView)
	{
		MyWebView->LoadURL(InURL);
	}
}

void UWebViewWidget::GoBack()
{
	if(MyWebView)
	{
		MyWebView->GoBack();
	}
}

void UWebViewWidget::GoForward()
{
	if(MyWebView)
	{
		MyWebView->GoForward();
	}
}

void UWebViewWidget::ReLoad() const
{
	if(MyWebView)
	{
		MyWebView->ReLoad();
	}
}

void UWebViewWidget::Stop() const
{
	if(MyWebView)
	{
		MyWebView->Stop();
	}
}

void UWebViewWidget::SetDownloadPath(const FString& InPath)
{
	if(MyWebView)
	{
		MyWebView->SetDownloadPath(InPath);
	}
}

void UWebViewWidget::GetWebDownloadInfo(FWebDownloadInfo& Info) const
{
	if(MyWebView)
	{
		MyWebView->GetWebDownloadInfo(Info.Url,Info.MimeType,Info.ContentDisposition);
	}
}

void UWebViewWidget::PostWebMessageAsJson(const FString& Json) const
{
	if(MyWebView)
	{
		MyWebView->PostWebMessageAsJson(Json);
	}
}

void UWebViewWidget::PostWebMessageAsString(const FString& InString) const
{
	if(MyWebView)
	{
		MyWebView->PostWebMessageAsJson(InString);
	}
}

void UWebViewWidget::ExecuteScript(const FString& script,FBPWebScriptCallbackDelegate Callback) const
{
	if(MyWebView)
	{
		MyWebView->ExecuteScript(script,FWebScriptCallbackDelegate::CreateLambda([Callback](const FString& Data)
		{
			if(Callback.IsBound())
			{
				Callback.Execute(Data);
			}
		}));
	}
}

void UWebViewWidget::ReleaseSlateResources(bool bReleaseChildren)
{
	Super::ReleaseSlateResources(bReleaseChildren);
	MyWebView.Reset();
}

TSharedRef<SWidget> UWebViewWidget::RebuildWidget()
{
	if(GEngine&&GEngine->GameViewport)
	{
		return
			SAssignNew(MyWebView,SWebView,GEngine->GameViewport->GetWindow().ToSharedRef())
			.bIs3DWebUI(bIs3DWebUI)
			.bIsActiveMouse(bIsActiveMouse)
			.bIsActiveDebugTool(bIsActiveDebugTool)
			.URL(URL)
			.NewURLDelegate_UObject(this,&UWebViewWidget::OnBeforePopupCallback)
			.LoadCompleteDelegate_UObject(this,&UWebViewWidget::OnLoadCompleteCallback)
			.LoadStartingDelegate_UObject(this,&UWebViewWidget::OnLoadStartingCallback)
			.EstimatedDownloadTimeDelegate_UObject(this,&UWebViewWidget::EstimatedDownloadTimeCallback)
			.DownloadBytesReceivedChangedDelegate_UObject(this,&UWebViewWidget::DownloadBytesReceivedChangedCallback)
			.DownloadStateChangedDelegate_Lambda([this](EWebDownloadState download_state, const FString& msg)
			{
				DownloadStateChanged.Broadcast((EBlueprintWebDownloadState)download_state,msg);
			})
			.Visibility(EVisibility::SelfHitTestInvisible);
	}
	return SNew(SBox)
		.HAlign(HAlign_Center)
		.VAlign(VAlign_Center)
		[
			SNew(STextBlock).Text(FText::FromString(TEXT("this is webview ui")))
		];
}

void UWebViewWidget::SynchronizeProperties()
{
	Super::SynchronizeProperties();
	if(MyWebView)
	{
		MyWebView->SetIsActiveMouse(bIsActiveMouse);
		MyWebView->SetIsActiveDebugTool(bIsActiveDebugTool);
		
	}
}

#if WITH_EDITOR
const FText UWebViewWidget::GetPaletteCategory()
{
	return FText::FromString(TEXT("Edge"));
}
#endif

void UWebViewWidget::OnBeforePopupCallback(const FString& NewURL)
{
	OnBeforePopupEvent.Broadcast(NewURL);
}

void UWebViewWidget::OnLoadCompleteCallback(bool bIsSuccess)
{
	if(bIsSuccess)
	{
		OnLoadSuccess.Broadcast();
	}
	else
	{
		OnLoadFailed.Broadcast();
	}
}

void UWebViewWidget::OnLoadStartingCallback(const FString& CurURL)
{
	OnLoadStarting.Broadcast(CurURL);
}

void UWebViewWidget::EstimatedDownloadTimeCallback(const FString& TimeStr)
{
	EstimatedDownloadTime.Broadcast(TimeStr);
}

void UWebViewWidget::DownloadBytesReceivedChangedCallback(int64 received_size, int64 total_size)
{
	DownloadBytesReceivedChanged.Broadcast(received_size,total_size);
}

