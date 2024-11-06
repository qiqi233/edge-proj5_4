#pragma once
#include <memory>
#include "Widgets/SLeafWidget.h"

class webview_interface;

enum class EWebDownloadState : uint8
{
	//正在下载
	progress=0,
	//下载完成
	complete=1,
	//下载中断
	interrupted=2
};

DECLARE_DELEGATE_OneParam(FWebScriptCallbackDelegate,const FString& /*Data*/)
DECLARE_DELEGATE_OneParam(FWebViewNewURLDelegate,const FString& /*NewURL*/)
DECLARE_DELEGATE_OneParam(FWebViewLoadCompleteDelegate,bool)
DECLARE_DELEGATE_OneParam(FWebViewLoadStartingDelegate,const FString& /*CurURL*/)
//监听预计下载时间
DECLARE_DELEGATE_OneParam(FEstimatedDownloadTimeDelegate,const FString&  /*time*/)
//监听下载进度
DECLARE_DELEGATE_TwoParams(FDownloadBytesReceivedChangedDelegate,int64 /*received_size*/,int64 /*total_size*/)
//监听下载状态
DECLARE_DELEGATE_TwoParams(FDownloadStateChangedDelegate,EWebDownloadState /*download_state*/,const FString& /*msg*/)


class WEBEDGE_API SWebView :public SWidget
{
public:
	SLATE_BEGIN_ARGS(SWebView)
		: _bIs3DWebUI(false)
		, _bIsActiveDebugTool(false)
		, _bIsActiveMouse(true)
		, _URL(TEXT("https://space.bilibili.com/43501343"))
	{
		
	}
	SLATE_ARGUMENT(bool,bIs3DWebUI)
	SLATE_ARGUMENT(bool,bIsActiveDebugTool)
	SLATE_ARGUMENT(bool,bIsActiveMouse)
	SLATE_ARGUMENT(FString,URL)
	SLATE_EVENT(FWebViewNewURLDelegate,NewURLDelegate)
	SLATE_EVENT(FWebViewLoadCompleteDelegate,LoadCompleteDelegate)
	SLATE_EVENT(FWebViewLoadStartingDelegate,LoadStartingDelegate)
	SLATE_EVENT(FEstimatedDownloadTimeDelegate,EstimatedDownloadTimeDelegate)
	SLATE_EVENT(FDownloadStateChangedDelegate,DownloadStateChangedDelegate )
	SLATE_EVENT(FDownloadBytesReceivedChangedDelegate,DownloadBytesReceivedChangedDelegate)
	SLATE_END_ARGS()
	SWebView();
	~SWebView();
	void Construct( const FArguments& InArgs,TSharedRef<SWindow> InParentWindowPtr);
	void LoadURL(const FString& URL);
	//前进
	void GoForward() const;
	//后退
	void GoBack() const;
	//重新加载
	void ReLoad() const;
	//停止加载
	void Stop() const;
	void SetDownloadPath(const FString& InPath);
	void GetWebDownloadInfo(FString& OutUrl,FString& OutMimeType,FString& OutDisposition)const;
	void SetIs3DUI(bool Is3DWebUI);
	void SetIsActiveDebugTool(bool isdebug);
	void SetIsActiveMouse(bool bAcitve);
	FString GetUniqueID()const;
	//给前端发送json类型的消息
	void PostWebMessageAsJson(const FString& Json) const;
	//给前端发送string类型的消息
	void PostWebMessageAsString(const FString& InString) const;
	//给前端插入脚本，并执行
	void ExecuteScript(const FString& script,FWebScriptCallbackDelegate ScriptCallback=FWebScriptCallbackDelegate()) const;
	
	virtual void SetVisibility( TAttribute<EVisibility> InVisibility ) override final;
	virtual FChildren* GetChildren() override;

	virtual void OnArrangeChildren( const FGeometry& AllottedGeometry, FArrangedChildren& ArrangedChildren ) const override;
	virtual FReply OnMouseButtonDown(const FGeometry& MyGeometry, const FPointerEvent& MouseEvent) override;
	virtual void OnMouseEnter(const FGeometry& MyGeometry, const FPointerEvent& MouseEvent) override;
	virtual void OnMouseLeave(const FPointerEvent& MouseEvent) override;
	virtual bool HasKeyboardFocus() const override;
	virtual void Tick(const FGeometry& AllottedGeometry, const double InCurrentTime, const float InDeltaTime) override;
	virtual int32 OnPaint( const FPaintArgs& Args, const FGeometry& AllottedGeometry, const FSlateRect& MyCullingRect, FSlateWindowElementList& OutDrawElements, int32 LayerId, const FWidgetStyle& InWidgetStyle, bool bParentEnabled ) const override;
	virtual FVector2D ComputeDesiredSize(float) const override;
	virtual FCursorReply OnCursorQuery(const FGeometry& MyGeometry, const FPointerEvent& CursorEvent) const override;
	void NewWindowRequestedEvent(const wchar_t* NewURL);
	void LoadCompleteEvent(bool bIsSuccess);
	void LoadStartingdEvent(const wchar_t* CurURL);
	void EstimatedDownloadTimeEvent(const wchar_t* time);
	void DownloadBytesReceivedChangedEvent(INT64 received_size,INT64 total_size);


private:
	bool bIs3DWebUI;
	bool bIsActiveDebugTool;
	bool bIsActiveMouse;
	FWebScriptCallbackDelegate WebScriptCallbackDelegateQueue;
	FWebViewNewURLDelegate NewURLRequestDelegate;
	FWebViewLoadCompleteDelegate LoadCompleteDelegate;
	FWebViewLoadStartingDelegate LoadStartingDelegate;
	FEstimatedDownloadTimeDelegate EstimatedDownloadTimeDelegate;
	FDownloadStateChangedDelegate DownloadStateChangedDelegate ;
	FDownloadBytesReceivedChangedDelegate DownloadBytesReceivedChangedDelegate;
	FString InitializeURL;
	std::shared_ptr<webview_interface> webview=nullptr;
	FString UniqueId;
	TWeakPtr<SWindow> ParentWindowPtr;
	bool bIsMouseEnter = false;
	bool bIsTop = false;
	EMouseCursor::Type MouseCursor = EMouseCursor::Default;
};
