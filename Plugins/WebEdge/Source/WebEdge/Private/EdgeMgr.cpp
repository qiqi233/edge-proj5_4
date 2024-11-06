#include "EdgeMgr.h"

#include "EdgeMessageRecive.h"
#include "edge_ue_mgr.h"
#include "SWebView.h"
#include "webview_interface.h"
#include "Engine/Engine.h"
#include "Framework/Application/SlateApplication.h"
#include "Widgets/Input/SEditableText.h"
#include "Widgets/Layout/SConstraintCanvas.h"


class FWebviewWindowsMessageHandler :public IWindowsMessageHandler
{
public:
	virtual bool ProcessMessage(HWND hwnd, uint32 msg, WPARAM wParam, LPARAM lParam, int32& OutResult) override;
};

static FWebviewWindowsMessageHandler MessageHandler;

UEdgeMgr* UEdgeMgr::g_Instance = nullptr;

UEdgeMgr* UEdgeMgr::Instance(UObject* WorldContext)
{
	if(!g_Instance)
	{
		if(!WorldContext)
		{
			WorldContext = GWorld;
		}
		if (UWorld* World = GEngine->GetWorldFromContextObject(WorldContext, EGetWorldErrorMode::LogAndReturnNull))
		{
			g_Instance = World->GetSubsystem<UEdgeMgr>();
		}
	}
	return g_Instance;
}

void UEdgeMgr::OnWorldBeginPlay(UWorld& InWorld)
{
	UWorldSubsystem::OnWorldBeginPlay(InWorld);
	g_Instance = this;
	if(EdgeMessageRecive)
		return;
	if(FSlateApplication::IsInitialized())
	{
		EdgeMessageRecive = new FEdgeMessageRecive();
		//void * m_hwnd = GEngine->GameViewport->GetWindow()->GetNativeWindow()->GetOSWindowHandle();
		edge_ue_mgr::instance()->init(EdgeMessageRecive);
		FString rootPath = FPaths::Combine(FPaths::ProjectDir(),TEXT("Binaries/Win64/"));
		rootPath = FPaths::ConvertRelativePathToFull(rootPath);
		edge_ue_mgr::instance()->set_root_path(rootPath.GetCharArray().GetData());
		//监听窗口消息（鼠标、键盘）
		TSharedPtr<FWindowsApplication> WindowsApplication = StaticCastSharedPtr<FWindowsApplication>(FSlateApplication::Get().GetPlatformApplication());
		if(WindowsApplication.IsValid())
		{
			WindowsApplication->AddMessageHandler(MessageHandler);
		}
	}

	
	TSharedRef<SConstraintCanvas> FullScreenCanvas = SNew(SConstraintCanvas)
		+ SConstraintCanvas::Slot()
		.Offset(FVector2D::ZeroVector)
		.AutoSize(false)
		.Anchors(FAnchors(0,0))
		.Alignment(FVector2D::ZeroVector)
		+SConstraintCanvas::Slot()
		[
			SNew(SBox).WidthOverride(1).HeightOverride(1)
			[
				SAssignNew(Z_TextPtr,SEditableText)
				.ColorAndOpacity(FLinearColor(0,0,0,0))
			]
		];
if (InWorld.GetFirstPlayerController()&&InWorld.GetGameViewport())
{
	InWorld.GetGameViewport()->AddViewportWidgetForPlayer(InWorld.GetFirstPlayerController()->GetLocalPlayer(), FullScreenCanvas,-100);
}
else
{
	InWorld.GetGameViewport()->AddViewportWidgetContent(FullScreenCanvas,  -10);
}
}

void UEdgeMgr::Deinitialize()
{
	if(!g_Instance)
		return;
	if(FSlateApplication::IsInitialized())
	{
		TSharedPtr<FWindowsApplication> WindowsApplication = StaticCastSharedPtr<FWindowsApplication>(FSlateApplication::Get().GetPlatformApplication());
		if(WindowsApplication.IsValid())
		{
			WindowsApplication->RemoveMessageHandler(MessageHandler);
		}
	}
	g_Instance = nullptr;
	if(EdgeMessageRecive)
	{
		delete EdgeMessageRecive;
		EdgeMessageRecive=nullptr;
	}
	Z_TextPtr=nullptr;
	edge_ue_mgr::instance()->shutdown();

}

void UEdgeMgr::FocusUE()
{
	if(Z_TextPtr.IsValid())
	{
		FSlateApplication::Get().SetKeyboardFocus(Z_TextPtr);
		Z_TextPtr->SetText(FText::FromString(TEXT("")));
		Z_TextPtr->SetClearKeyboardFocusOnCommit(true);
	}
}

void UEdgeMgr::AddGC(std::shared_ptr<webview_interface> webui)
{
	if(webui)
	{
		//页面已经创建成功，直接释放就可以
		if(webui->is_create_success())
		{
			webui->Clear();
			webui = nullptr;
		}
		else
		{
			//等待网页创建完再释放
			webui->SetVisible(false);
			webviews_GC.push_back(webui);
		}
	
	}
}

void UEdgeMgr::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	for(std::vector<std::shared_ptr<webview_interface>>::iterator it = webviews_GC.begin();it!=webviews_GC.end();)
	{
		if((*it)&&(*it)->is_create_success())
		{
			(*it)->Clear();
			webviews_GC.erase(it);
		}
		else
		{
			++it;
		}
	}
}


bool FWebviewWindowsMessageHandler::ProcessMessage(HWND hwnd, uint32 msg, WPARAM wParam, LPARAM lParam, int32& OutResult)
{
	if(edge_ue_mgr::is_initialized())
	{
		//WebView自己会捕获键盘，这里不需要给他同步键盘事件
		if (msg==WM_KEYDOWN||msg==WM_KEYUP||msg==WM_SYSKEYDOWN||msg==WM_SYSKEYUP)
		{
			return false;
		}
		return edge_ue_mgr::instance()->handle_window_message(hwnd,msg,wParam,lParam);
	}
	return false;
}
