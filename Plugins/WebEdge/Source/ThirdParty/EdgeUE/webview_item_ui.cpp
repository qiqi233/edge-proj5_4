#include "webview_item_ui.h"

#include <filesystem>

#include "WebView2EnvironmentOptions.h"
#include <windows.h>
#include <strsafe.h>
#include <stdlib.h>
#include <tchar.h>
#include <wrl.h>
#include "WebView2EnvironmentOptions.h"
#include <ShellScalingApi.h>
#include "edge_ue_mgr.h"
#include "message_process_host.h"
#include "message_receive_host.h"
#include "ue_message_interface.h"

webview_item_ui::webview_item_ui(HWND hwnd, const wchar_t* uniqueID, const wchar_t* initial_url)
    : webview_interface(uniqueID)
      , m_initial_url(initial_url == NULL ? L"" : initial_url)
      , b_is_visible(true)
      , m_main_hwnd(hwnd)
{
    CreateWebview();
}

webview_item_ui::~webview_item_ui()
{
    b_create_success = false;
    OutputDebugString(L"webview_item_ui delete~");
}

void webview_item_ui::SetIs3DUI(bool bIs3DUI)
{
    if (m_bIs3DUI == bIs3DUI)
        return;
    m_bIs3DUI = bIs3DUI;

    if (message_process_ptr)
    {
        message_process_ptr->RefreshWebViewVisual(this->shared_from_this());
    }
}

void webview_item_ui::LoadURL(const wchar_t* initial_url)
{
    m_initial_url = initial_url == NULL ? L"" : initial_url;
    if (!webviewWindow)
    {
        return;
    }
    webviewWindow->Navigate(initial_url);
}

void webview_item_ui::GoForward() const
{
    if (webviewWindow)
    {
        BOOL bIsSuccess = false;
        webviewWindow->get_CanGoForward(&bIsSuccess);
        if (bIsSuccess)
        {
            webviewWindow->GoForward();
        }
    }
}

void webview_item_ui::GoBack() const
{
    if (webviewWindow)
    {
        BOOL bIsSuccess = false;
        webviewWindow->get_CanGoBack(&bIsSuccess);
        if (bIsSuccess)
        {
            webviewWindow->GoBack();
        }
    }
}

void webview_item_ui::ReLoad() const
{
    if (webviewWindow)
    {
        webviewWindow->Reload();
    }
}

void webview_item_ui::Stop() const
{
    if (webviewWindow)
    {
        webviewWindow->Stop();
    }
}

void webview_item_ui::Clear()
{
    if (download)
    {
        download->remove_StateChanged(m_stateChangedToken);
        download->remove_BytesReceivedChanged(m_bytesReceivedChangedToken);
        download->remove_EstimatedEndTimeChanged(m_estimatedEndTimeChanged);
        download = nullptr;
    }
    if (message_process_ptr)
    {
        message_process_ptr->DestroyVisualAsWebview(this->shared_from_this());
    }
    if (webviewController)
    {
        webviewController->remove_ZoomFactorChanged(m_zoomFactorChangedToken);
        webviewController->remove_AcceleratorKeyPressed(m_AcceleratorKeyPressedEventHandlerToken);
        webviewController->Close();
        webviewController = nullptr;


        m_cookieManager = nullptr;
        m_webViewEnvironment = nullptr;
    }
    if (webviewWindow)
    {
        CoreWebView2_4->remove_DownloadStarting(m_DownloadStartingToken);
        webviewWindow->remove_WebMessageReceived(m_messageReceiveToken);
        webviewWindow->remove_NavigationCompleted(m_NavigationCompletedToken);
        webviewWindow->remove_NewWindowRequested(m_NewWindowRequestedToken);
        webviewWindow->remove_NavigationStarting(m_NavigationStartingToken);
        CoreWebView2_4 = nullptr;
        webviewWindow = nullptr;
    }
    if (m_compositionController)
    {
        m_compositionController->remove_CursorChanged(m_CursorChangedEventHandlerToken);
    }
    m_compositionController = nullptr;
}

HWND webview_item_ui::GetMainWindowHandle()
{
    return m_main_hwnd;
}

void webview_item_ui::PostWebMessageAsJson(const wchar_t* Json) const
{
    if (webviewWindow)
    {
        webviewWindow->PostWebMessageAsString(Json);
    }
}

void webview_item_ui::PostWebMessageAsString(const wchar_t* InString) const
{
    if (webviewWindow)
    {
        webviewWindow->PostWebMessageAsString(InString);
    }
}

void webview_item_ui::Tick(float DeltaTime)
{
}

RECT webview_item_ui::GetBounds() const
{
    return m_webui_bound;
    if (webviewController)
    {
        RECT bounds;
        webviewController->get_Bounds(&bounds);
        return bounds;
    }
    return m_webui_bound;
}

void webview_item_ui::SetBounds(RECT rect)
{
    m_webui_bound = rect;
    if (webviewController)
    {
        webviewController->put_Bounds(rect);
    }
    if (m_compositionController)
    {
        winrt::com_ptr<IUnknown> rootVisualTarget;
        m_compositionController->get_RootVisualTarget(rootVisualTarget.put());
        if (rootVisualTarget)
        {
            winrt::Windows::UI::Composition::ContainerVisual webviewVisual = rootVisualTarget.as<
                winrt::Windows::UI::Composition::ContainerVisual>();
            if (webviewVisual)
            {
                webviewVisual.Offset({
                    static_cast<float>(m_webui_bound.left), static_cast<float>(m_webui_bound.top), 0
                });
                webviewVisual.Size({
                    (static_cast<float>(m_webui_bound.right - m_webui_bound.left)),
                    (static_cast<float>(m_webui_bound.bottom - m_webui_bound.top))
                });
            }
        }
    }
}

void webview_item_ui::SetBounds(POINT offset, POINT size)
{
    m_webui_bound.top = LONG(offset.y);
    m_webui_bound.left = LONG(offset.x);
    m_webui_bound.bottom = LONG(size.y + m_webui_bound.top);
    m_webui_bound.right = LONG(size.x + m_webui_bound.left);
    SetBounds(m_webui_bound);
}

void webview_item_ui::SetVisible(bool is_visible)
{
    if (b_is_visible == is_visible)
        return;
    b_is_visible = is_visible;
    if (webviewController)
    {
        webviewController->put_IsVisible(b_is_visible);
    }
}

bool webview_item_ui::GetVisible() const
{
    return b_is_visible;
}


void webview_item_ui::DisableMouse()
{
    webview_interface::DisableMouse();
}

void webview_item_ui::set_active_debug_tool(bool is_debug)
{
    webview_interface::set_active_debug_tool(is_debug);
    SetActiveDebugTool();
}

void webview_item_ui::execute_script(const wchar_t* script,
                                     std::function<void(const wchar_t* resultObjectAsJson)> callback_function)
{
    if (!webviewWindow)
        return;
    webviewWindow->ExecuteScript(script,
        Callback<ICoreWebView2ExecuteScriptCompletedHandler>(
               [callback_function](HRESULT errorCode, LPCWSTR resultObjectAsJson) -> HRESULT
               {
                   if (!errorCode && resultObjectAsJson)
                   {
                       if (callback_function)
                       {
                           auto len = wcslen(resultObjectAsJson);
                           std::wstring result(resultObjectAsJson, resultObjectAsJson + len);
                           callback_function(result.c_str());
                       }
                   }

                   return S_OK;
               }).Get());
}

void webview_item_ui::SetContainerVisual(winrt::Windows::UI::Composition::ContainerVisual webViewVisual)
{
    m_webViewVisual = webViewVisual;
}

void webview_item_ui::SetActiveDebugTool()
{
    if (webviewWindow)
    {
        ICoreWebView2Settings* Settings;
        webviewWindow->get_Settings(&Settings);
        //调试工具
        Settings->put_AreDevToolsEnabled(is_active_debug_tool());
        //右击菜单
        Settings->put_AreDefaultContextMenusEnabled(is_active_debug_tool());
        //脚本对话   
        Settings->put_AreDefaultScriptDialogsEnabled(true);
        Settings->put_IsScriptEnabled(true);
        Settings->put_IsWebMessageEnabled(true);
        //禁止网页缩放
        Settings->put_IsZoomControlEnabled(FALSE);
        //清理本地缓存
        //WebviewControll->ClearBrowsingData(COREWEBVIEW2_BROWSING_DATA_KINDS_DISK_CACHE);
        //WebviewControll->ClearBrowsingData(COREWEBVIEW2_BROWSING_DATA_KINDS_COOKIES);
    }
}

void webview_item_ui::CreateWebview()
{
    auto options = Microsoft::WRL::Make<CoreWebView2EnvironmentOptions>();
    //options->put_AdditionalBrowserArguments(L"--disable-features=RendererCodeIntegrity");
    auto enviCreateHandle = Microsoft::WRL::Callback<ICoreWebView2CreateCoreWebView2EnvironmentCompletedHandler>(
        this, &webview_item_ui::CreateEnvironmentCompletedCallback);
    HRESULT res = CreateCoreWebView2EnvironmentWithOptions(
        nullptr,
        L"C:/temp/",
        options.Get(),
        enviCreateHandle.Get());
}

// void webview_item_ui::addEventListenerAsTransparent()
// {
//     if(!webviewWindow)
//         return;
//     std::wstring jsscript = LR"(
//         window.addEventListener('load', function(){
//         document.body.addEventListener('mousemove', function(e) {
//         var x = e.clientX, y = e.clientY,
//         pixel = document.elementFromPoint(x, y);
//         var computedStyle = window.getComputedStyle(pixel),
//         backgroundColor = computedStyle.backgroundColor;
//         //window.chrome.webview.hostObjects.sync.ue.NotifyTransparent(backgroundColor.a);
//         console.log(backgroundColor);
//      });
//     });
//     )";
//  // std::wstring jsscript = LR"(
//  //    document.addEventListener('mousemove', function(e) {
//  //        var x = e.clientX, y = e.clientY,
//  //        element = document.elementFromPoint(x, y);
//  //        if (element) {
//  //            console.log('Mouse is over an element:', element);
//  //        } else {
//  //            console.log('Mouse is not over any elements.');
//  //        }
//  //    });
//  //    )";
//     webviewWindow->ExecuteScript(jsscript.c_str(), Callback<ICoreWebView2ExecuteScriptCompletedHandler>(
//         [](HRESULT errorCode, LPCWSTR resultObjectAsJson) -> HRESULT {
//             if (!errorCode && resultObjectAsJson) {
//                 // do something with the result
//             }
//             return S_OK;
//         }).Get());
// }

HRESULT webview_item_ui::CreateEnvironmentCompletedCallback(HRESULT result, ICoreWebView2Environment* env)
{
    m_webViewEnvironment = env;
    wil::com_ptr<ICoreWebView2Environment3> webViewEnvironment3 = m_webViewEnvironment.try_query<
        ICoreWebView2Environment3>();

    if (webViewEnvironment3)
    {
        auto CompositionCotroller = Microsoft::WRL::Callback<
            ICoreWebView2CreateCoreWebView2CompositionControllerCompletedHandler>(
            this, &webview_item_ui::OnCreateCoreWebView2ControllerCompleted);
        webViewEnvironment3->CreateCoreWebView2CompositionController(m_main_hwnd,
                                                                     CompositionCotroller.Get());
    }
    return S_OK;
}

HRESULT webview_item_ui::OnCreateCoreWebView2ControllerCompleted(HRESULT result,
                                                                 ICoreWebView2CompositionController*
                                                                 compositionController)
{
    if (compositionController != nullptr)
    {
        m_compositionController = compositionController;
        message_process_ptr = edge_ue_mgr::instance()->get_message_process(m_main_hwnd);
        message_process_ptr->CreateWebViewVisual(this->shared_from_this());
        m_compositionController->QueryInterface(IID_PPV_ARGS(&webviewController));
        webviewController->get_CoreWebView2(&webviewWindow);
    }
    SetActiveDebugTool();
    //RECT bounds;
    //GetClientRect(edge_ue_mgr::instance()->get_main_window(), &bounds);
    //设置网页背景透明
    COREWEBVIEW2_COLOR transparentColor = {0, 255, 255, 255};
    wil::com_ptr<ICoreWebView2Controller2> TmpContrll = webviewController.query<ICoreWebView2Controller2>();
    if (TmpContrll)
    {
        TmpContrll->get_ParentWindow(nullptr);
        TmpContrll->put_DefaultBackgroundColor(transparentColor);

        TmpContrll->add_ZoomFactorChanged(
            Microsoft::WRL::Callback<ICoreWebView2ZoomFactorChangedEventHandler>(
                [this](ICoreWebView2Controller* sender,
                       IUnknown* args) -> HRESULT
                {
                    //double zoomFactor;
                    //sender->get_ZoomFactor(&zoomFactor);
                    sender->put_ZoomFactor(1.f);
                    return S_OK;
                })
            .Get(), &m_zoomFactorChangedToken);
    }

    //监听打开新的链接时的事件
    // 假设你已经有了一个 `ICoreWebView2Controller` 实例，命名为 `webView`
    auto NewWindowRequestedHandle = Callback<ICoreWebView2NewWindowRequestedEventHandler>(
        this, &webview_item_ui::OnCoreWebView2NewWindowRequestedEventHandler);
    webviewWindow->add_NewWindowRequested(NewWindowRequestedHandle.Get(), &m_NewWindowRequestedToken);

    //绑定网页开始加载时的回调
    auto NavigationStartingEventHandler = Callback<ICoreWebView2NavigationStartingEventHandler>(
        this, &webview_item_ui::OnNavigationStartingEventHandler);
    webviewWindow->add_NavigationStarting(NavigationStartingEventHandler.Get(), &m_NavigationStartingToken);

    //绑定网页加载完成时的回调
    auto NavigationCompletedEventHandler = Callback<ICoreWebView2NavigationCompletedEventHandler>(
        this, &webview_item_ui::OnNavigationCompletedEventHandler);
    webviewWindow->add_NavigationCompleted(NavigationCompletedEventHandler.Get(), &m_NavigationCompletedToken);


    //绑定开始下载事件
    CoreWebView2_4 = webviewWindow.query<ICoreWebView2_4>();
    auto DownloadStartingHandler = Callback<ICoreWebView2DownloadStartingEventHandler>(
        this, &webview_item_ui::OnDownloadStartingEventHandler);
    CoreWebView2_4->add_DownloadStarting(DownloadStartingHandler.Get(), &m_DownloadStartingToken);
    //绑定光标发生变化时的回调
    auto CursorChangedEventHandler = Callback<ICoreWebView2CursorChangedEventHandler>(
        this, &webview_item_ui::OnCursorChangedEventHandler);
    m_compositionController->add_CursorChanged(CursorChangedEventHandler.Get(), &m_CursorChangedEventHandlerToken);


    //捕获webView键盘事件
    auto AcceleratorKeyPressedEventHandler = Callback<ICoreWebView2AcceleratorKeyPressedEventHandler>(
        this, &webview_item_ui::OnAcceleratorKeyPressedEventHandler);
    webviewController->add_AcceleratorKeyPressed(AcceleratorKeyPressedEventHandler.Get(),
                                                 &m_AcceleratorKeyPressedEventHandlerToken);

    //判断鼠标键盘事件是否被WebView处理了

    /*1-消息通讯：
    *string或者json类型的消息，类似与web与ue的那种通讯方式
    *调用规则：
    *   window.chrome.webview.postMessage(window.document.URL)
    *@监听规则：
     *  window.chrome.webview.addEventListener("message", (e) => {
     *      console.log(e);
     *  })
     */
    auto messageReceiveCB = Microsoft::WRL::Callback<
        ICoreWebView2WebMessageReceivedEventHandler>(this, &webview_item_ui::messageReceived);
    webviewWindow->add_WebMessageReceived(messageReceiveCB.Get(), &m_messageReceiveToken);


    //2-另一种消息通讯，把对象直接暴露给我们的js进行调用 例如：chrome.webview.hostObjects.sync.ue.CloseWindow();
    wil::com_ptr<message_receive_host> hostObj = Microsoft::WRL::Make<message_receive_host>(
        edge_ue_mgr::instance()->get_root_path().c_str(), m_uniqueID.c_str());
    VARIANT remoteObjectAsVariant = {};
    hostObj.query_to<IDispatch>(&remoteObjectAsVariant.pdispVal);
    remoteObjectAsVariant.vt = VT_DISPATCH;
    assert(webviewWindow->AddHostObjectToScript(L"ue", &remoteObjectAsVariant)==S_OK);
    remoteObjectAsVariant.pdispVal->Release();

    webviewController->put_IsVisible(b_is_visible);
    webviewController->put_Bounds(m_webui_bound);
    webviewWindow->Navigate(m_initial_url.c_str());
    b_create_success = true;
    return S_OK;
}

HRESULT webview_item_ui::OnNavigationCompletedEventHandler(ICoreWebView2* sender,
                                                           ICoreWebView2NavigationCompletedEventArgs* args)
{
    BOOL isSuccess;
    args->get_IsSuccess(&isSuccess);
    if (NavigationCompletedEvent)
    {
        NavigationCompletedEvent(isSuccess);
    }
    return S_OK;
}

HRESULT webview_item_ui::OnNavigationStartingEventHandler(ICoreWebView2* sender,
                                                          ICoreWebView2NavigationStartingEventArgs* args)
{
    wil::unique_cotaskmem_string uri;
    args->get_Uri(&uri);
    if (NavigationStartingEvent)
    {
        NavigationStartingEvent(uri.get());
    }
    return S_OK;
}

HRESULT webview_item_ui::OnCursorChangedEventHandler(ICoreWebView2CompositionController* sender, IUnknown* args)
{
    // Retrieve ICoreWebView2Cursor
    HCURSOR webviewCursor;
    HRESULT result = sender->get_Cursor(&webviewCursor);

    if (FAILED(result))
    {
        return result;
    }
    if (CursorChangedEvent)
    {
        CursorChangedEvent(webviewCursor);
    }
    return S_OK;
}

HRESULT webview_item_ui::OnAcceleratorKeyPressedEventHandler(ICoreWebView2Controller* sender,
                                                             ICoreWebView2AcceleratorKeyPressedEventArgs* args)
{
    COREWEBVIEW2_KEY_EVENT_KIND keyEventKind;
    args->get_KeyEventKind(&keyEventKind);
    UINT key;
    args->get_VirtualKey(&key);

    if (keyEventKind == COREWEBVIEW2_KEY_EVENT_KIND_KEY_DOWN)
        PostMessage(m_main_hwnd, WM_KEYDOWN, key, 0);
    else if (keyEventKind == COREWEBVIEW2_KEY_EVENT_KIND_KEY_UP)
        PostMessage(m_main_hwnd, WM_KEYUP, key, 0);

    return S_OK;
}

HRESULT webview_item_ui::OnDownloadStartingEventHandler(ICoreWebView2* sender,
                                                        ICoreWebView2DownloadStartingEventArgs* args)
{
    // 获取下载操作的实例
    args->get_DownloadOperation(&download);
    // // 获取下载是否被取消的状态
    // BOOL cancel = FALSE;
    // args->get_Cancel(&cancel);
    // 获取下载的 URL 地址
    wil::unique_cotaskmem_string uri;
    download->get_Uri(&uri);
    m_download_info.url = uri.get();
    // 获取下载文件的 MIME 类型
    wil::unique_cotaskmem_string mimeType;
    download->get_MimeType(&mimeType);
    m_download_info.mimeType = mimeType.get();

    // 获取下载文件的内容描述
    wil::unique_cotaskmem_string contentDisposition;
    download->get_ContentDisposition(&contentDisposition);
    m_download_info.contentDisposition = contentDisposition.get();
    // // 获取下载状态
    // COREWEBVIEW2_DOWNLOAD_STATE state;
    // download->get_State(&state);
    // 获取总共需要下载的字节大小
    download->get_TotalBytesToReceive(&totalBytesToReceive);

    // 获取是否处理下载的状态
    // BOOL handled = FALSE;
    // args->get_Handled(&handled);
    //隐藏下载的UI界面
    args->put_Handled(true);

    auto ActiveDownloadFunc = [this]()
    {
        // 当下载状态发生改变时，定义处理函数
        download->add_StateChanged(Callback<ICoreWebView2StateChangedEventHandler>(
                                       this, &webview_item_ui::OnDownLoadChangeStateEvent).Get(), &m_stateChangedToken);

        // 当下载接收字节改变时，定义处理函数
        download->add_BytesReceivedChanged(
            Callback<ICoreWebView2BytesReceivedChangedEventHandler>(
                this, &webview_item_ui::OnBytesReceivedChanged).Get(), &m_bytesReceivedChangedToken);

        // 当预估下载结束时间改变时，定义处理函数
        download->add_EstimatedEndTimeChanged(
            Callback<ICoreWebView2EstimatedEndTimeChangedEventHandler>(
                this, &webview_item_ui::OnEstimatedEndTimeChanged).Get(), &m_estimatedEndTimeChanged);
    };


    //自定义了下载路径，则延迟下载（等路径设置完成后，在激活下载）
    if (!m_download_path.empty() && std::filesystem::is_directory(m_download_path))
    {
        std::replace(m_download_path.begin(), m_download_path.end(), '/', '\\');
        // 获取预设的下载路径
        wil::unique_cotaskmem_string resultFilePath;
        args->get_ResultFilePath(&resultFilePath);
        std::filesystem::path filePath(resultFilePath.get());
        std::wstring fileName = filePath.filename().wstring();
        //自定义下载路径
        std::wstring customDownloadPath = m_download_path + fileName;
        args->put_ResultFilePath(customDownloadPath.c_str());
        wil::com_ptr<ICoreWebView2Deferral> deferral;
        args->GetDeferral(&deferral);
        message_process_ptr->RunAsync([deferral, ActiveDownloadFunc]()
        {
            ActiveDownloadFunc();
            deferral->Complete();
        });
    }
    else
    {
        //没有自定义路径，直接激活下载
        ActiveDownloadFunc();
    }


    return S_OK;
}


std::wstring InterruptReasonToString(
    const COREWEBVIEW2_DOWNLOAD_INTERRUPT_REASON interrupt_reason)
{
    std::wstring interrupt_reason_string = L"";
    switch (interrupt_reason)
    {
    case COREWEBVIEW2_DOWNLOAD_INTERRUPT_REASON_NONE:
        interrupt_reason_string = L"None";
        break;
    case COREWEBVIEW2_DOWNLOAD_INTERRUPT_REASON_FILE_FAILED:
        interrupt_reason_string = L"下载失败";
        break;
    case COREWEBVIEW2_DOWNLOAD_INTERRUPT_REASON_FILE_ACCESS_DENIED:
        interrupt_reason_string = L"文件访问被拒绝";
        break;
    case COREWEBVIEW2_DOWNLOAD_INTERRUPT_REASON_FILE_NO_SPACE:
        interrupt_reason_string = L"文件没有空间";
        break;
    case COREWEBVIEW2_DOWNLOAD_INTERRUPT_REASON_FILE_NAME_TOO_LONG:
        interrupt_reason_string = L"文件名太长";
        break;
    case COREWEBVIEW2_DOWNLOAD_INTERRUPT_REASON_FILE_TOO_LARGE:
        interrupt_reason_string = L"文件太大";
        break;
    case COREWEBVIEW2_DOWNLOAD_INTERRUPT_REASON_FILE_MALICIOUS:
        interrupt_reason_string = L"文件恶意";
        break;
    case COREWEBVIEW2_DOWNLOAD_INTERRUPT_REASON_FILE_TRANSIENT_ERROR:
        interrupt_reason_string = L"文件瞬态错误";
        break;
    case COREWEBVIEW2_DOWNLOAD_INTERRUPT_REASON_FILE_BLOCKED_BY_POLICY:
        interrupt_reason_string = L"文件被策略阻止";
        break;
    case COREWEBVIEW2_DOWNLOAD_INTERRUPT_REASON_FILE_SECURITY_CHECK_FAILED:
        interrupt_reason_string = L"文件安全检查失败";
        break;
    case COREWEBVIEW2_DOWNLOAD_INTERRUPT_REASON_FILE_TOO_SHORT:
        interrupt_reason_string = L"文件太短";
        break;
    case COREWEBVIEW2_DOWNLOAD_INTERRUPT_REASON_FILE_HASH_MISMATCH:
        interrupt_reason_string = L"文件哈希不匹配";
        break;
    case COREWEBVIEW2_DOWNLOAD_INTERRUPT_REASON_NETWORK_FAILED:
        interrupt_reason_string = L"网络故障";
        break;
    case COREWEBVIEW2_DOWNLOAD_INTERRUPT_REASON_NETWORK_TIMEOUT:
        interrupt_reason_string = L"网络超时";
        break;
    case COREWEBVIEW2_DOWNLOAD_INTERRUPT_REASON_NETWORK_DISCONNECTED:
        interrupt_reason_string = L"网络已断开连接";
        break;
    case COREWEBVIEW2_DOWNLOAD_INTERRUPT_REASON_NETWORK_SERVER_DOWN:
        interrupt_reason_string = L"网络服务器故障";
        break;
    case COREWEBVIEW2_DOWNLOAD_INTERRUPT_REASON_NETWORK_INVALID_REQUEST:
        interrupt_reason_string = L"网络无效请求";
        break;
    case COREWEBVIEW2_DOWNLOAD_INTERRUPT_REASON_SERVER_FAILED:
        interrupt_reason_string = L"服务器失败";
        break;
    case COREWEBVIEW2_DOWNLOAD_INTERRUPT_REASON_SERVER_NO_RANGE:
        interrupt_reason_string = L"服务器无范围";
        break;
    case COREWEBVIEW2_DOWNLOAD_INTERRUPT_REASON_SERVER_BAD_CONTENT:
        interrupt_reason_string = L"服务器内容错误";
        break;
    case COREWEBVIEW2_DOWNLOAD_INTERRUPT_REASON_SERVER_UNAUTHORIZED:
        interrupt_reason_string = L"服务器未授权";
        break;
    case COREWEBVIEW2_DOWNLOAD_INTERRUPT_REASON_SERVER_CERTIFICATE_PROBLEM:
        interrupt_reason_string = L"服务器证书问题";
        break;
    case COREWEBVIEW2_DOWNLOAD_INTERRUPT_REASON_SERVER_FORBIDDEN:
        interrupt_reason_string = L"服务器被禁止";
        break;
    case COREWEBVIEW2_DOWNLOAD_INTERRUPT_REASON_SERVER_UNEXPECTED_RESPONSE:
        interrupt_reason_string = L"服务器意外响应";
        break;
    case COREWEBVIEW2_DOWNLOAD_INTERRUPT_REASON_SERVER_CONTENT_LENGTH_MISMATCH:
        interrupt_reason_string = L"服务器内容长度不匹配";
        break;
    case COREWEBVIEW2_DOWNLOAD_INTERRUPT_REASON_SERVER_CROSS_ORIGIN_REDIRECT:
        interrupt_reason_string = L"服务器跨源重定向";
        break;
    case COREWEBVIEW2_DOWNLOAD_INTERRUPT_REASON_USER_CANCELED:
        interrupt_reason_string = L"用户已取消";
        break;
    case COREWEBVIEW2_DOWNLOAD_INTERRUPT_REASON_USER_SHUTDOWN:
        interrupt_reason_string = L"用户关机";
        break;
    case COREWEBVIEW2_DOWNLOAD_INTERRUPT_REASON_USER_PAUSED:
        interrupt_reason_string = L"用户已暂停";
        break;
    case COREWEBVIEW2_DOWNLOAD_INTERRUPT_REASON_DOWNLOAD_PROCESS_CRASHED:
        interrupt_reason_string = L"下载进程崩溃";
        break;
    }
    return interrupt_reason_string;
}

HRESULT webview_item_ui::OnDownLoadChangeStateEvent(ICoreWebView2DownloadOperation* sender, IUnknown* args)
{
    if (!download)
        return S_FALSE;
    if (!DownloadStateChangedEvent)
        return S_OK;
    COREWEBVIEW2_DOWNLOAD_STATE state;
    download->get_State(&state);
    switch (state)
    {
    case COREWEBVIEW2_DOWNLOAD_STATE_IN_PROGRESS:
        DownloadStateChangedEvent(e_web_download_state::progress, L"正在下载中...");
        break;
    case COREWEBVIEW2_DOWNLOAD_STATE_INTERRUPTED:
        {
            COREWEBVIEW2_DOWNLOAD_INTERRUPT_REASON interrupt_reason;
            download->get_InterruptReason(&interrupt_reason);
            std::wstring msg = InterruptReasonToString(interrupt_reason);
            DownloadStateChangedEvent(e_web_download_state::interrupted, msg.c_str());
            download->remove_StateChanged(m_stateChangedToken);
            download->remove_BytesReceivedChanged(m_bytesReceivedChangedToken);
            download->remove_EstimatedEndTimeChanged(m_estimatedEndTimeChanged);
            download = nullptr;
        }
        break;
    case COREWEBVIEW2_DOWNLOAD_STATE_COMPLETED:
        DownloadStateChangedEvent(e_web_download_state::complete, L"下载已完成");
        download->remove_StateChanged(m_stateChangedToken);
        download->remove_BytesReceivedChanged(m_bytesReceivedChangedToken);
        download->remove_EstimatedEndTimeChanged(m_estimatedEndTimeChanged);
        break;
    }
    return S_OK;
}

HRESULT webview_item_ui::OnBytesReceivedChanged(ICoreWebView2DownloadOperation* sender, IUnknown* args)
{
    if (!download)
        return S_FALSE;
    //获取当前下载的字节数
    INT64 bytesReceived = 0;
    download->get_BytesReceived(&bytesReceived);
    //更新下载进度
    if (DownloadProgressEvent)
    {
        DownloadProgressEvent(bytesReceived, totalBytesToReceive);
    }
    return S_OK;
}

HRESULT webview_item_ui::OnEstimatedEndTimeChanged(ICoreWebView2DownloadOperation* sender, IUnknown* args)
{
    if (!download)
        return S_FALSE;
    wil::unique_cotaskmem_string estimatedEndTime;
    download->get_EstimatedEndTime(&estimatedEndTime);
    if (EstimatedDownloadTimeEvent)
    {
        EstimatedDownloadTimeEvent(estimatedEndTime.get());
    }
    return S_OK;
}

HRESULT webview_item_ui::messageReceived(ICoreWebView2* webview, ICoreWebView2WebMessageReceivedEventArgs* args)
{
    wil::unique_cotaskmem_string messageRaw;

    args->get_WebMessageAsJson(&messageRaw);
    std::wstring message = messageRaw.get();
    if (edge_ue_mgr::instance()->get_message_interface())
    {
        edge_ue_mgr::instance()->get_message_interface()->NoitfyReciveMessage(message.c_str(), message.size());
    }
    return S_OK;
}

HRESULT webview_item_ui::OnCoreWebView2NewWindowRequestedEventHandler(ICoreWebView2* sender,
                                                                      ICoreWebView2NewWindowRequestedEventArgs* args)
{
    wil::unique_cotaskmem_string uri;
    args->get_Uri(&uri);
    if (NewWindowRequestedEvent)
    {
        NewWindowRequestedEvent(uri.get());
    }
    //sender->Navigate(uri.get());
    args->put_Handled(TRUE);
    return S_OK;
}
