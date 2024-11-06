#pragma once
#include <strsafe.h>
#include <stdlib.h>
#include <tchar.h>
#include <wrl.h>
#include <wil/com.h>

#include "webview_interface.h"
using namespace Microsoft::WRL;
#include <locale>
#include "WebView2.h"
#include "WebView2EnvironmentOptions.h"
#include <ShellScalingApi.h>
#include <string>
#include <algorithm>
#pragma comment(lib, "ws2_32.lib")
#include<fstream>
#include <winrt/Windows.UI.Composition.Desktop.h>

/*
 * 真正的网页类
 */
class webview_item_ui : public webview_interface,public std::enable_shared_from_this<webview_item_ui>
{
    friend class message_process_host;
    friend class edge_ue_mgr;
public:
    webview_item_ui(HWND hwnd,const wchar_t* uniqueID,const wchar_t* initial_url);
    ~webview_item_ui();
    /******start webview_interface*******/

    virtual void SetIs3DUI(bool bIs3DUI)override;
    //加载网页
    virtual void LoadURL(const wchar_t*initial_url) override;
    //前进
    virtual void GoForward() const override;
    //后退
    virtual void GoBack() const override;
    //重新加载
    virtual void ReLoad() const override;
    //停止加载
    virtual void Stop() const override;
    virtual void SetBounds(RECT rect)override;
    virtual void SetBounds(POINT offset,POINT size)override;
    //获取当前网页大小
    virtual RECT GetBounds()const override;
    virtual void SetVisible(bool is_visible)override;
    virtual bool GetVisible() const override;
    virtual void DisableMouse() override;
    virtual void set_active_debug_tool(bool is_debug) override;
    //给前端发送json消息
    virtual void PostWebMessageAsJson(const wchar_t* Json) const override;
    //给前端发送string类型的消息
    virtual void PostWebMessageAsString(const wchar_t* InString) const override;

    virtual void Tick(float DeltaTime) override;
    //执行前端脚本
    virtual void execute_script(const wchar_t* script,std::function<void(const wchar_t* resultObjectAsJson)> callback_function=nullptr) override;
    //关闭网页
    virtual void Clear()override;
    virtual HWND GetMainWindowHandle() override;
    /******end webview_interface*******/
    
    void SetContainerVisual(winrt::Windows::UI::Composition::ContainerVisual webViewVisual);
    void SetActiveDebugTool();
    void SetLayerID(long layer_id){m_layer_id = layer_id;}
    long GetLayerID() const {return m_layer_id;}
protected:
    
    void CreateWebview();
    //void addEventListenerAsTransparent();
    //创建一个runtime.webview的环境设置的初始化回调函数
    HRESULT CreateEnvironmentCompletedCallback(HRESULT result, ICoreWebView2Environment* env);
    //创建一个runtime.webview的WebView2Controller控件的初始化回调函数
    HRESULT OnCreateCoreWebView2ControllerCompleted(HRESULT result, ICoreWebView2CompositionController* compositionController);
    HRESULT messageReceived(ICoreWebView2* webview, ICoreWebView2WebMessageReceivedEventArgs* args);
    //创建一个新的网页链接的回调函数
    HRESULT OnCoreWebView2NewWindowRequestedEventHandler(ICoreWebView2* sender, ICoreWebView2NewWindowRequestedEventArgs* args);
    //加载完成时的回调函数，可能成功，可能失败
    HRESULT OnNavigationCompletedEventHandler(ICoreWebView2* sender, ICoreWebView2NavigationCompletedEventArgs* args);
    //开始加载网页时的回调函数
    HRESULT OnNavigationStartingEventHandler(ICoreWebView2* sender, ICoreWebView2NavigationStartingEventArgs* args);
    //光标改变时的回调函数
    HRESULT OnCursorChangedEventHandler( ICoreWebView2CompositionController* sender,IUnknown* args);
    //捕获键盘事件回调函数
    HRESULT OnAcceleratorKeyPressedEventHandler(ICoreWebView2Controller* sender, ICoreWebView2AcceleratorKeyPressedEventArgs* args);
    //开始下载
    HRESULT OnDownloadStartingEventHandler(ICoreWebView2* sender, ICoreWebView2DownloadStartingEventArgs* args);
    //下载状态发生改变
    HRESULT OnDownLoadChangeStateEvent(ICoreWebView2DownloadOperation* sender,IUnknown* args);
    //处理下载进度
    HRESULT OnBytesReceivedChanged(ICoreWebView2DownloadOperation* sender, IUnknown* args);
    //当预计下载时间发生变化时，通知此事件
    HRESULT OnEstimatedEndTimeChanged( ICoreWebView2DownloadOperation* sender, IUnknown* args);
private:
    std::wstring m_initial_url;
    winrt::Windows::UI::Composition::ContainerVisual m_webViewVisual={nullptr};
    wil::com_ptr<ICoreWebView2Environment> m_webViewEnvironment;
    wil::com_ptr<ICoreWebView2CompositionController> m_compositionController;
    // Pointer to WebViewController
    wil::com_ptr<ICoreWebView2Controller> webviewController;
   // wil::com_ptr<ICoreWebView2Controller3>  m_controller3;
    // Pointer to WebView window
    wil::com_ptr<ICoreWebView2> webviewWindow;
    wil::com_ptr<ICoreWebView2_4> CoreWebView2_4;
    wil::com_ptr<ICoreWebView2CookieManager> m_cookieManager;
    
    EventRegistrationToken m_zoomFactorChangedToken = {};
    //EventRegistrationToken m_rasterizationScaleChangedToken = {};
    EventRegistrationToken m_messageReceiveToken{};
    EventRegistrationToken m_NewWindowRequestedToken{};
    EventRegistrationToken m_NavigationCompletedToken{};
    EventRegistrationToken m_NavigationStartingToken{};
    EventRegistrationToken m_CursorChangedEventHandlerToken{};
    EventRegistrationToken m_AcceleratorKeyPressedEventHandlerToken{};
    //下载相关的....
    EventRegistrationToken m_DownloadStartingToken{};
    EventRegistrationToken m_stateChangedToken = {};
    EventRegistrationToken m_bytesReceivedChangedToken = {};
    EventRegistrationToken m_estimatedEndTimeChanged = {};
    wil::com_ptr<ICoreWebView2DownloadOperation> download;
    INT64 totalBytesToReceive = 0;
    RECT m_webui_bound;
    bool b_is_visible;

    std::shared_ptr<message_process_host> message_process_ptr;
    HWND m_main_hwnd;
    bool bIsTop = false;
    long m_layer_id;
};


