#include "EdgeMessageRecive.h"

#include "EdgeMgr.h"
#include "Kismet/KismetSystemLibrary.h"


void FEdgeMessageRecive::NoitfyReciveMessage(const wchar_t* data, size_t size)
{
	FString MessageString(data);
	GEngine->AddOnScreenDebugMessage(-1,2.f,FColor::Cyan,MessageString);
	//提供给我们的蓝图进行绑定使用
	UEdgeMgr::Instance()->WebMessageCallback.Broadcast(MessageString);
}

void FEdgeMessageRecive::QuitGame()
{
	FGenericPlatformMisc::RequestExit(true);
}
