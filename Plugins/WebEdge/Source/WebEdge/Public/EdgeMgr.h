#pragma once
#include "CoreMinimal.h"
#include "Subsystems/WorldSubsystem.h"
#include "Windows/WindowsApplication.h"
#include <memory>
#include <vector>
#include "EdgeMgr.generated.h"

class SWebView;
class FEdgeMessageRecive;
class webview_interface;
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FWebMessageReviveCallback,FString,Data);
UCLASS(Blueprintable,BlueprintType)
class WEBEDGE_API UEdgeMgr:public UTickableWorldSubsystem
{
	GENERATED_BODY()
public:
	static UEdgeMgr* Instance(UObject* WorldContext=nullptr);
	virtual void OnWorldBeginPlay(UWorld& InWorld) override;
	virtual void Deinitialize() override;
	void FocusUE();
	void AddGC(std::shared_ptr<webview_interface> webui);
	virtual void Tick(float DeltaTime) override;
	virtual TStatId GetStatId() const override{ return TStatId();}
	UPROPERTY(BlueprintAssignable)
	FWebMessageReviveCallback WebMessageCallback;
private:
	static UEdgeMgr* g_Instance;
	FEdgeMessageRecive* EdgeMessageRecive=nullptr;

	TSharedPtr<class SEditableText> Z_TextPtr;
	std::vector<std::shared_ptr<webview_interface>> webviews_GC;
};


