// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/Widget.h"
#include "WebViewWidget.generated.h"

/* -------------------------------------------------------------------------------------------
 * Todo:友情提示，此类是给UMG使用的，只能创建在主窗口中，如果要创建在弹窗中（新的SWindow类）请使用SWebView *
 *--------------------------------------------------------------------------------------------*/

UENUM()
enum EBlueprintWebDownloadState
{
	//正在下载
	progress=0,
	//下载完成
	complete=1,
	//下载中断
	interrupted=2
};
USTRUCT(BlueprintType)
struct FWebDownloadInfo
{
	GENERATED_BODY()
	//获取下载的 URL 地址
	UPROPERTY(EditAnywhere,BlueprintReadOnly)
	FString Url;
	//下载文件的 MIME 类型
	UPROPERTY(EditAnywhere,BlueprintReadOnly)
	FString MimeType;
	//获取下载文件的内容描述
	UPROPERTY(EditAnywhere,BlueprintReadOnly)
	FString ContentDisposition;
};
class SWebView;
UCLASS()
class WEBEDGE_API UWebViewWidget : public UWidget
{
	GENERATED_UCLASS_BODY()
	DECLARE_DYNAMIC_DELEGATE_OneParam(FBPWebScriptCallbackDelegate, FString, ResultData);
	DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnBeforePopup, FString, NewURL);
	DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnLoadStarting, FString, CurURL);
	DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnLoadComplete);
	
	//监听预计下载时间
	DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FEstimatedDownloadTimeDelegate,const FString&,TimeStr);
	//监听下载进度
	DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FDownloadBytesReceivedChangedDelegate,int64,received_size,int64,total_size);
	//监听下载状态
	DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FDownloadStateChangedDelegate,EBlueprintWebDownloadState,download_state,const FString&,msg);
public:
	UPROPERTY(EditAnywhere, BlueprintReadOnly, FieldNotify, Category="WebView")
	bool bIs3DWebUI;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, FieldNotify, Category="WebView")
	bool bIsActiveDebugTool;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, FieldNotify, Category="WebView")
	bool bIsActiveMouse;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="WebView")
	FString URL;
	UPROPERTY(BlueprintAssignable, Category = "WebView|Event")
	FOnBeforePopup OnBeforePopupEvent;
	UPROPERTY(BlueprintAssignable, Category = "WebView|Event")
    FOnLoadComplete OnLoadSuccess;
	UPROPERTY(BlueprintAssignable, Category = "WebView|Event")
    FOnLoadComplete OnLoadFailed;
	UPROPERTY(BlueprintAssignable, Category = "WebView|Event")
	FOnLoadStarting OnLoadStarting;
	UPROPERTY(BlueprintAssignable, DisplayName="监听预下载时间事件",Category = "WebView|Event")
	FEstimatedDownloadTimeDelegate EstimatedDownloadTime;
	UPROPERTY(BlueprintAssignable, DisplayName="下载进度事件", Category = "WebView|Event")
	FDownloadBytesReceivedChangedDelegate DownloadBytesReceivedChanged;
	UPROPERTY(BlueprintAssignable, DisplayName="下载状态事件", Category = "WebView|Event")
	FDownloadStateChangedDelegate DownloadStateChanged;
	UFUNCTION(BlueprintCallable, Category="WebView")
	void SetIsActiveDebugTool(bool bIsDebugTool);
	UFUNCTION(BlueprintPure, Category="WebView")
	bool GetbIsActiveDebugTool()const;
	UFUNCTION(BlueprintCallable, Category="WebView")
	void SetIsActiveMouse(bool bIsActiveMouseIn);
	UFUNCTION(BlueprintPure, Category="WebView")
	bool GetIsActiveMouse()const;
	UFUNCTION(BlueprintCallable, Category="WebView")
	void LoadURL(const FString& InURL);
	UFUNCTION(BlueprintCallable, Category="WebView")
	void GoBack();
	UFUNCTION(BlueprintCallable, Category="WebView")
	void GoForward();
	//重新加载
	UFUNCTION(BlueprintCallable, Category="WebView")
	void ReLoad() const;
	//停止加载
	UFUNCTION(BlueprintCallable, Category="WebView")
	void Stop() const;
	/**
	 * 设置下载路径
	 * Todo:需要注意InPath路径只支持:.../.../.../或者...\...\...\     不支持: ...\\...\\...\\
	 */
	UFUNCTION(BlueprintCallable, Category="WebView")
	void SetDownloadPath(const FString& InPath);
	//获取跟下载相关的一些信息
	UFUNCTION(BlueprintPure,Category="WebView")
	void GetWebDownloadInfo(FWebDownloadInfo& Info)const;
	//给前端发送json类型的消息
	UFUNCTION(BlueprintCallable, Category="WebView")
	void PostWebMessageAsJson(const FString& Json) const;
	//给前端发送string类型的消息
	UFUNCTION(BlueprintCallable, Category="WebView")
	void PostWebMessageAsString(const FString& InString) const;
	//给前端插入脚本，并执行
	UFUNCTION(BlueprintCallable, Category="WebView")
	void ExecuteScript(const FString& script,FBPWebScriptCallbackDelegate Callback) const;
	virtual void ReleaseSlateResources(bool bReleaseChildren) override;
	// 重写此函数以构造你的Slate界面
	virtual TSharedRef<SWidget> RebuildWidget() override;

	virtual void SynchronizeProperties() override;
#if WITH_EDITOR
	virtual const FText GetPaletteCategory()override;
#endif
protected:
	UFUNCTION()
	void OnBeforePopupCallback(const FString& NewURL);
	UFUNCTION()
	void OnLoadCompleteCallback(bool bIsSuccess);
	UFUNCTION()
	void OnLoadStartingCallback(const FString& CurURL);
	UFUNCTION()
	void EstimatedDownloadTimeCallback(const FString& TimeStr);
	UFUNCTION()
	void DownloadBytesReceivedChangedCallback(int64 received_size,int64 total_size);
private:
	TSharedPtr<SWebView> MyWebView;
	
};
