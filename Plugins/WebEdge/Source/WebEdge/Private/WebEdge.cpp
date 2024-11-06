#include "WebEdge.h"

#include "EdgeMgr.h"
#include "Misc/Paths.h"
#define LOCTEXT_NAMESPACE "FWebEdgeModule"

void* LoadDLL(FString Path)
{
	Path =FPaths::ConvertRelativePathToFull(Path);
	FPlatformProcess::PushDllDirectory(*Path);
	void* Handle=FPlatformProcess::GetDllHandle(*Path);
	FPlatformProcess::PopDllDirectory(*Path);
	return Handle;
}
void FWebEdgeModule::StartupModule()
{
	Webview2Handle = nullptr;
	EdgeUeHandle = nullptr;
	FString WebView2LoaderDllPath=FPaths::Combine(FPaths::ProjectDir(),TEXT("Binaries/Win64/WebView2Loader.dll"));
	FString EdgeUEDllPath=FPaths::Combine(FPaths::ProjectDir(),TEXT("Binaries/Win64/EdgeUE.dll"));
	Webview2Handle = LoadDLL(WebView2LoaderDllPath);
	EdgeUeHandle = LoadDLL(EdgeUEDllPath);
}


void FWebEdgeModule::ShutdownModule()
{
	if(Webview2Handle)
	{
		FPlatformProcess::FreeDllHandle(Webview2Handle);
	}
	if(EdgeUeHandle)
	{
		FPlatformProcess::FreeDllHandle(EdgeUeHandle);
	}
}

#undef LOCTEXT_NAMESPACE

IMPLEMENT_MODULE(FWebEdgeModule, WebEdge)