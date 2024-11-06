#include "SWebView.h"
#include <edge_ue_mgr.h>
#include <webview_interface.h>
#include "Windows/AllowWindowsPlatformTypes.h"
#include <windows.h>
#include "Windows/HideWindowsPlatformTypes.h"
#include "EdgeMgr.h"
#include "Engine/Engine.h"
#include "Misc/MessageDialog.h"
#include "Widgets/SWindow.h"

SWebView::SWebView()
	: bIs3DWebUI(true)
	, bIsActiveDebugTool(false)
	, bIsActiveMouse(true)
	, UniqueId(FGuid::NewGuid().ToString())
{
	bCanHaveChildren = false;
}

SWebView::~SWebView()
{
	webview->SetNewWindowRequestedEvent(nullptr);
	webview->SetNavigationCompletedEvent(nullptr);
	webview->SetNavigationStartingEvent(nullptr);
	webview->SetEstimatedDownloadTimeEvent(nullptr);
	webview->SetDownloadStateChangedEvent(nullptr);
	webview->SetDownloadProgressEvent(nullptr);
	webview->SetCursorChangedEvent(nullptr);
	
	NewURLRequestDelegate.Unbind();
	LoadCompleteDelegate.Unbind();
	LoadStartingDelegate.Unbind();
	UEdgeMgr::Instance()->AddGC(webview);
	
}

void SWebView::Construct(const FArguments& InArgs, TSharedRef<SWindow> InParentWindowPtr)
{
	ParentWindowPtr = InParentWindowPtr;
	LoadURL(InArgs._URL);
	NewURLRequestDelegate = InArgs._NewURLDelegate;
	LoadCompleteDelegate = InArgs._LoadCompleteDelegate;
	LoadStartingDelegate = InArgs._LoadStartingDelegate;
	EstimatedDownloadTimeDelegate = InArgs._EstimatedDownloadTimeDelegate;
	DownloadStateChangedDelegate = InArgs._DownloadStateChangedDelegate;
	DownloadBytesReceivedChangedDelegate = InArgs._DownloadBytesReceivedChangedDelegate;
	if (ParentWindowPtr.Pin()->GetNativeWindow())
	{
		void* hwnd = ParentWindowPtr.Pin()->GetNativeWindow()->GetOSWindowHandle();
		webview =edge_ue_mgr::instance()->create_webview(
			(HWND)hwnd,
			UniqueId.GetCharArray().GetData(),
			InitializeURL.GetCharArray().GetData());
		webview->SetNewWindowRequestedEvent(std::bind(&SWebView::NewWindowRequestedEvent, this, std::placeholders::_1));
		webview->SetNavigationCompletedEvent(std::bind(&SWebView::LoadCompleteEvent, this, std::placeholders::_1));
		webview->SetNavigationStartingEvent(std::bind(&SWebView::LoadStartingdEvent, this, std::placeholders::_1));
		webview->SetEstimatedDownloadTimeEvent(std::bind(&SWebView::EstimatedDownloadTimeEvent, this, std::placeholders::_1));
		webview->SetDownloadStateChangedEvent([this](e_web_download_state download_state,const wchar_t* msg)
		{
			if(DownloadStateChangedDelegate.IsBound())
			{
				EWebDownloadState State = (EWebDownloadState)download_state;
				DownloadStateChangedDelegate.Execute(State,msg);
			}
		});
		webview->SetDownloadProgressEvent(std::bind(&SWebView::DownloadBytesReceivedChangedEvent, this, std::placeholders::_1,std::placeholders::_2));
		webview->SetCursorChangedEvent([this](HCURSOR webviewCursor)
		{
			HCURSOR HANDCursor = LoadCursor(NULL, IDC_HAND);
			if (webviewCursor == HANDCursor)
			{
				MouseCursor = EMouseCursor::Hand;
				return;
			}
			HCURSOR hIBeamCursor = LoadCursor(NULL, IDC_IBEAM);
			if (webviewCursor == hIBeamCursor)
			{
				MouseCursor = EMouseCursor::TextEditBeam;
				return;
			}
			MouseCursor = EMouseCursor::Default;
			// Do something with webviewCursor and/or this
		});
		SetIs3DUI(InArgs._bIs3DWebUI);
		SetIsActiveDebugTool(InArgs._bIsActiveDebugTool);
		SetIsActiveMouse(InArgs._bIsActiveMouse);
	}
	else
	{
		FMessageDialog::Open(EAppMsgType::Ok, FText::FromString(TEXT("请先调用AddWindow之类的方法，SWebView需要一个HWND句柄")));
		webview = nullptr;
	}
}

void SWebView::LoadURL(const FString& URL)
{
	InitializeURL = URL;
	if(!(InitializeURL.StartsWith(TEXT("https://"))||
		InitializeURL.StartsWith(TEXT("http://"))||
		InitializeURL.StartsWith(TEXT("file:///"))))
	{
		if(InitializeURL.StartsWith(TEXT("www")))
		{
			InitializeURL = TEXT("http://") + InitializeURL;
		}
		else
		{
			InitializeURL = TEXT("file:///") + InitializeURL;
		}
	}
	if (webview)
	{
		
		webview->LoadURL(InitializeURL.GetCharArray().GetData());
	}
}

void SWebView::GoForward() const
{
	if (webview)
	{
		webview->GoForward();
	}
}

void SWebView::GoBack() const
{
	if (webview)
	{
		webview->GoBack();
	}
}

void SWebView::ReLoad() const
{
	if (webview)
	{
		webview->ReLoad();
	}
}

void SWebView::Stop() const
{
	if (webview)
	{
		webview->Stop();
	}
}

void SWebView::SetDownloadPath(const FString& InPath)
{
	if (webview&&!InPath.IsEmpty())
	{
		webview->set_download_path(InPath.GetCharArray().GetData());
	}
}

void SWebView::GetWebDownloadInfo(FString& OutUrl, FString& OutMimeType, FString& OutDisposition)const
{
	if (webview)
	{
		OutUrl = webview->get_download_info().url.c_str();
		OutMimeType = webview->get_download_info().mimeType.c_str();
		OutDisposition = webview->get_download_info().contentDisposition.c_str();
	}
}

void SWebView::SetIs3DUI(bool IsHandle)
{
	bIs3DWebUI = IsHandle;
	if (webview)
	{
		webview->SetIs3DUI(bIs3DWebUI);
	}
}

void SWebView::SetIsActiveDebugTool(bool isdebug)
{
	bIsActiveDebugTool = isdebug;
	if (webview)
	{
		webview->set_active_debug_tool(isdebug);
	}
}

void SWebView::SetIsActiveMouse(bool bAcitve)
{
	bIsActiveMouse = bAcitve;
	if (webview)
	{
		if (bIsActiveMouse)
		{
			webview->EnableMouse();
		}
		else
		{
			webview->DisableMouse();
		}
	}
}

FString SWebView::GetUniqueID() const
{
	return UniqueId;
}

void SWebView::PostWebMessageAsJson(const FString& Json) const
{
	if (webview)
	{
		webview->PostWebMessageAsJson(Json.GetCharArray().GetData());
	}
}

void SWebView::PostWebMessageAsString(const FString& InString) const
{
	if (webview)
	{
		webview->PostWebMessageAsJson(InString.GetCharArray().GetData());
	}
}

void SWebView::ExecuteScript(const FString& script,FWebScriptCallbackDelegate ScriptCallback) const
{
	if (webview)
	{
		webview->execute_script(script.GetCharArray().GetData(),[ScriptCallback](const wchar_t* resultObjectAsJson)
		{
			if(ScriptCallback.IsBound())
			{
				ScriptCallback.Execute(resultObjectAsJson);
			}
		}
		);
	}
}


void SWebView::SetVisibility(TAttribute<EVisibility> InVisibility)
{
	SWidget::SetVisibility(InVisibility);
	if (webview)
	{
		if(GetVisibility()==EVisibility::Visible)
		{
			SetVisibility(EVisibility::SelfHitTestInvisible);
		}
		if (GetVisibility() == EVisibility::Collapsed || GetVisibility() == EVisibility::Hidden)
		{
			webview->SetVisible(false);
		}
		else
		{
			webview->SetVisible(true);
		}
	}
}

FChildren* SWebView::GetChildren()
{
	return &FNoChildren::NoChildrenInstance;
}

void SWebView::OnArrangeChildren(const FGeometry& AllottedGeometry, FArrangedChildren& ArrangedChildren) const
{
	// Nothing to arrange; Leaf Widgets do not have children.
}

FReply SWebView::OnMouseButtonDown(const FGeometry& MyGeometry, const FPointerEvent& MouseEvent)
{
	if(MouseEvent.IsMouseButtonDown(EKeys::RightMouseButton))
	{
		UEdgeMgr::Instance()->FocusUE();
		return FReply::Handled();
	}
	return FReply::Handled();
}


void SWebView::OnMouseEnter(const FGeometry& MyGeometry, const FPointerEvent& MouseEvent)
{
	bIsMouseEnter = true;
	if (bIsActiveDebugTool)
	{
		GEngine->AddOnScreenDebugMessage(101, 2.f, FColor::Cyan,
		                                 FString::Printf(TEXT("OnMouseEnter[%s]！"), *InitializeURL));
	}
}

void SWebView::OnMouseLeave(const FPointerEvent& MouseEvent)
{
	bIsTop = false;
	bIsMouseEnter = false;
	if (bIsActiveDebugTool)
	{
		GEngine->AddOnScreenDebugMessage(101, 2.f, FColor::Cyan,
		                                 FString::Printf(TEXT("OnMouseLeave[%s]！"), *InitializeURL));
	}
	UEdgeMgr::Instance()->FocusUE();
}

bool SWebView::HasKeyboardFocus() const
{
	return bIsMouseEnter;
}

void SWebView::Tick(const FGeometry& AllottedGeometry, const double InCurrentTime, const float InDeltaTime)
{
	if(webview)
	{
		webview->Tick(InDeltaTime);
	}
}

int32 SWebView::OnPaint(const FPaintArgs& Args, const FGeometry& AllottedGeometry, const FSlateRect& MyCullingRect,
                        FSlateWindowElementList& OutDrawElements, int32 LayerId, const FWidgetStyle& InWidgetStyle,
                        bool bParentEnabled) const
{
	if (webview)
	{
		FVector2D offset = AllottedGeometry.LocalToAbsolute(FVector2D::ZeroVector);
		FVector2D size = AllottedGeometry.GetDrawSize();
		if (bIsActiveDebugTool)
		{
			static FSlateBrush Brush;
			FSlateDrawElement::MakeBox(
				OutDrawElements,
				LayerId,
				AllottedGeometry.ToPaintGeometry(),
				&Brush,
				ESlateDrawEffect::None,
				FLinearColor(1, 0, 0, 0.2f));
			offset.X += 10;
			offset.Y += 10;
			size.X -= 20;
			size.Y -= 20;
		}
		POINT _of;
		_of.x = offset.X;
		_of.y = offset.Y;
		POINT _si;
		_si.x = size.X;
		_si.y = size.Y;
		webview->SetBounds(_of, _si);
	}
	return LayerId + 1;
}

FVector2D SWebView::ComputeDesiredSize(float) const
{
	if (GetVisibility() == EVisibility::Collapsed)
	{
		return FVector2D::ZeroVector;
	}
	if (webview)
	{
		RECT bound = webview->GetBounds();
		return FVector2D(
			(static_cast<float>(bound.right - bound.left)),
			(static_cast<float>(bound.bottom - bound.top)));
	}
	return FVector2D::ZeroVector;
}

FCursorReply SWebView::OnCursorQuery(const FGeometry& MyGeometry, const FPointerEvent& CursorEvent) const
{
	return FCursorReply::Cursor(MouseCursor);
}

void SWebView::NewWindowRequestedEvent(const wchar_t* NewURL)
{
	if (NewURLRequestDelegate.IsBound())
	{
		NewURLRequestDelegate.Execute(NewURL);
	}
}

void SWebView::LoadCompleteEvent(bool bIsSuccess)
{
	if (LoadCompleteDelegate.IsBound())
	{
		LoadCompleteDelegate.Execute(bIsSuccess);
	}
}

void SWebView::LoadStartingdEvent(const wchar_t* CurURL)
{
	if (LoadStartingDelegate.IsBound())
	{
		LoadStartingDelegate.Execute(CurURL);
	}
}

void SWebView::EstimatedDownloadTimeEvent(const wchar_t* time)
{
	if(EstimatedDownloadTimeDelegate.IsBound())
	{
		EstimatedDownloadTimeDelegate.Execute(time);
	}

}

void SWebView::DownloadBytesReceivedChangedEvent(INT64 received_size, INT64 total_size)
{
	if(DownloadBytesReceivedChangedDelegate.IsBound())
	{
		DownloadBytesReceivedChangedDelegate.Execute(received_size,total_size);
	}
}
