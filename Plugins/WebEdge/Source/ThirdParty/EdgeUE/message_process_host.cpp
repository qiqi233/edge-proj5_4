#include "message_process_host.h"
#include <winrt/Windows.Foundation.Collections.h>
#include <DispatcherQueue.h>
#include <sstream>
#include <Windowsx.h>
#include "WebView2EnvironmentOptions.h"
#include <windows.ui.composition.interop.h>
#include "edge_ue_mgr.h"
#include "webview_item_ui.h"

using namespace winrt;
using namespace winrt::Windows::System;
using namespace winrt::Windows::UI;
using namespace winrt::Windows::UI::Composition;
using namespace winrt::Windows::UI::Composition::Desktop;
using namespace winrt::Windows::Foundation::Numerics;


static constexpr UINT s_runAsyncWindowMessage = WM_APP;

message_process_host::message_process_host(HWND hwnd, winrt::Windows::System::DispatcherQueueController QueueController)
    : m_main_hwnd(hwnd)
      , m_dispatcherQueueController(QueueController)
{

    if (m_dispatcherQueueController)
    {
        m_compositor = Compositor();
    }
}

message_process_host::~message_process_host()
{
    DestroyWinCompVisualTree();
    edge_ue_mgr::instance()->remove_message_process(m_main_hwnd);
}

void message_process_host::initialize()
{
    CreateDesktopWindowTarget();
    //跟图元，里面用于放多个webview空间
    CreateCompositionRoot();
}

void message_process_host::CreateCompositionRoot()
{
    //创建根容器
    m_RootVisual = m_compositor.CreateContainerVisual();
    m_RootVisual.RelativeSizeAdjustment({1.0f, 1.0f});
    m_RootVisual.Offset({0, 0, 0});
    m_target.Root(m_RootVisual);

    //创建拥于存放3DUI的容器
    m_3DVisual = m_compositor.CreateContainerVisual();
    m_RootVisual.Children().InsertAtTop(m_3DVisual);
    m_3DVisual.RelativeSizeAdjustment({1.0f, 1.0f});
    m_3DVisual.Offset({0, 0, 0});

    //创建拥于存放2DUI的容器，2DUI的网页容器层级要高于3DUI容器
    m_2DVisual = m_compositor.CreateContainerVisual();
    m_RootVisual.Children().InsertAtTop(m_2DVisual);
    m_2DVisual.RelativeSizeAdjustment({1.0f, 1.0f});
    m_2DVisual.Offset({0, 0, 0});
}


void message_process_host::TrackMouseEvents(DWORD mouseTrackingFlags)
{
    TRACKMOUSEEVENT tme;
    tme.cbSize = sizeof(tme);
    tme.dwFlags = mouseTrackingFlags;
    tme.hwndTrack = m_main_hwnd;
    tme.dwHoverTime = 0;
    ::TrackMouseEvent(&tme);
}

void message_process_host::CreateWebViewVisual(std::shared_ptr<webview_item_ui> item_ui)
{
    assert(m_2DVisual);
    assert(m_3DVisual);
    if (item_ui->m_compositionController)
    {

        auto& Visual = item_ui->GetIs3DUI() ? m_3DVisual : m_2DVisual;
        ContainerVisual webViewVisual = m_compositor.CreateContainerVisual();
        Visual.Children().InsertAtTop(webViewVisual);
        RECT bound = item_ui->GetBounds();
        webViewVisual.Offset({static_cast<float>(bound.left), static_cast<float>(bound.top), 0});
        const float2 webViewSize = {
            (static_cast<float>(bound.right - bound.left)),
            (static_cast<float>(bound.bottom - bound.top))
        };
        webViewVisual.Size(webViewSize);
        // webViewVisual.CompositeMode()
        item_ui->SetContainerVisual(webViewVisual);
        item_ui->m_compositionController->put_RootVisualTarget(webViewVisual.as<IUnknown>().get());
        m_webview_map[item_ui->m_uniqueID] = item_ui;
        //每创建一个新的UI，则增加一下层级ID
        OnClickLayerID++;
        item_ui->SetLayerID(OnClickLayerID);
    }
}

void message_process_host::RefreshWebViewVisual(std::shared_ptr<webview_item_ui> item_ui)
{
    if(!item_ui)
        return;
    if(item_ui->GetIs3DUI())
    {
        m_2DVisual.Children().Remove(item_ui->m_webViewVisual);
        m_3DVisual.Children().InsertAtTop(item_ui->m_webViewVisual);
    }
    else
    {
        m_3DVisual.Children().Remove(item_ui->m_webViewVisual);
        m_2DVisual.Children().InsertAtTop(item_ui->m_webViewVisual);
    }
}

bool message_process_host::OnMouseMessage(UINT message, WPARAM wParam, LPARAM lParam)
{
    if (!m_RootVisual)
    {
        return false;
    }
    if(message==WM_CLOSE)
        return false;
    //自定义下载路径后，延迟下载方法到此激活下载。
    if(s_runAsyncWindowMessage==message)
    {
        auto* task = reinterpret_cast<std::function<void()>*>(wParam);
        (*task)();
        delete task;
        return true;
    }
    POINT point;
    POINTSTOPOINT(point, lParam);
    if (message == WM_MOUSEWHEEL ||
        message == WM_MOUSEHWHEEL
        || message == WM_NCRBUTTONDOWN || message == WM_NCRBUTTONUP
    )
    {
        //鼠标滚轮消息以屏幕坐标传递。
        //SendMouseInput需要WebView的客户端坐标，因此转换
        //从屏幕到客户端的点。
        ::ScreenToClient(m_main_hwnd, &point);
    }

    std::list<std::shared_ptr<webview_item_ui>> focus_ui = find_webview_from_point(point);
    if (focus_ui.size()==0)
        return false;

    std::shared_ptr<webview_item_ui> TopUI2D;
    std::shared_ptr<webview_item_ui> TopUI3D;
    for(auto It = focus_ui.begin();It!=focus_ui.end();++It)
    {
        if((*It)->GetIs3DUI())
        {
            if (!TopUI3D)
            {
                TopUI3D = (*It);
                continue;
            }
            if (TopUI3D->GetLayerID() < (*It)->GetLayerID())
            {
                TopUI3D = (*It);
            }
        }
        else
        {
            if (!TopUI2D)
            {
                TopUI2D = (*It);
                continue;
            }
            if (TopUI2D->GetLayerID() < (*It)->GetLayerID())
            {
                TopUI2D = (*It);
            }
        }
        
    }
    if (message == WM_LBUTTONDOWN)
    {
        if(TopUI3D && TopUI3D->GetIs3DUI())
        {
            m_3DVisual.Children().Remove(TopUI3D->m_webViewVisual);
            m_3DVisual.Children().InsertAtTop(TopUI3D->m_webViewVisual);
            OnClickLayerID++;
            TopUI3D->SetLayerID(OnClickLayerID);
        }
    }
    
    DWORD mouseData = 0;
    switch (message)
    {
    case WM_MOUSEWHEEL:
    case WM_MOUSEHWHEEL:
        mouseData = GET_WHEEL_DELTA_WPARAM(wParam);
        break;
    // case WM_XBUTTONDBLCLK:
    // case WM_XBUTTONDOWN:
    // case WM_XBUTTONUP:
    //     mouseData = GET_XBUTTON_WPARAM(wParam);
    //     break;
    // case WM_MOUSEMOVE:
    //     if (!m_isTrackingMouse)
    //     {
    //         TrackMouseEvents(TME_LEAVE);
    //         m_isTrackingMouse = true;
    //     }
    //     break;
    // case WM_MOUSELEAVE:
    //     m_isTrackingMouse = false;
    //     break;
    }

    // //我们需要捕捉鼠标，以防用户拖动鼠标超出窗口边界，我们仍需要发送
    // //将鼠标消息发送到WebView进程。这对拖动滚动条或平移地图等场景。
    // //这与指针消息的情况非常相似，其中媒体在WebView内部启动。
    // if (message == WM_LBUTTONDOWN || message == WM_MBUTTONDOWN ||
    //     message == WM_RBUTTONDOWN || message == WM_XBUTTONDOWN)
    // {
    //     if (::GetCapture() != m_main_hwnd)
    //     {
    //         m_isCapturingMouse = true;
    //         ::SetCapture(m_main_hwnd);
    //     }
    // }
    // else if (message == WM_LBUTTONUP || message == WM_MBUTTONUP ||
    //     message == WM_RBUTTONUP || message == WM_XBUTTONUP)
    // {
    //     if (::GetCapture() == m_main_hwnd)
    //     {
    //         m_isCapturingMouse = false;
    //         ::ReleaseCapture();
    //     }
    // }

    if (message != WM_MOUSELEAVE)
    {
        if(TopUI2D)
        {
            RECT m_appBounds = TopUI2D->GetBounds();
            point.x -= m_appBounds.left;
            point.y -= m_appBounds.top;
            TopUI2D->m_compositionController->SendMouseInput(
    static_cast<COREWEBVIEW2_MOUSE_EVENT_KIND>(message),
    static_cast<COREWEBVIEW2_MOUSE_EVENT_VIRTUAL_KEYS>(GET_KEYSTATE_WPARAM(wParam)), mouseData,
    point);
        }

        if (TopUI3D)
        {
            RECT m_appBounds = TopUI3D->GetBounds();
            point.x -= m_appBounds.left;
            point.y -= m_appBounds.top;
            TopUI3D->m_compositionController->SendMouseInput(
                static_cast<COREWEBVIEW2_MOUSE_EVENT_KIND>(message),
                static_cast<COREWEBVIEW2_MOUSE_EVENT_VIRTUAL_KEYS>(GET_KEYSTATE_WPARAM(wParam)), mouseData,
                point);
        }
        return true;
    }

    return false;
}

void message_process_host::DestroyWinCompVisualTree()
{
    for (auto it = m_webview_map.begin(); it != m_webview_map.end(); ++it)
    {
        if (it->second)
        {
            auto& Visual = it->second->GetIs3DUI() ? m_3DVisual : m_2DVisual;
            if (Visual)
            {
                Visual.Children().Remove(it->second->m_webViewVisual);
            }
            it->second->Clear();
        }
    }
    m_webview_map.clear();
    if (m_RootVisual)
    {
        m_RootVisual.Children().RemoveAll();
        m_RootVisual = nullptr;

        m_target.Root(nullptr);
        m_target = nullptr;
    }
}

void message_process_host::DestroyVisualAsWebview(std::shared_ptr<webview_item_ui> item_ui)
{
    if (item_ui)
    {
        auto find_iterator = m_webview_map.find(item_ui->m_uniqueID);
        if (find_iterator != m_webview_map.end())
        {
            m_webview_map.erase(find_iterator);
        }
    }
}

HWND message_process_host::GetMainWindowHandle() const
{
    return m_main_hwnd;
}

std::list<std::shared_ptr<webview_item_ui>> message_process_host::find_webview_from_point(POINT point)
{
    std::list<std::shared_ptr<webview_item_ui>> item_uis;
    for (auto it = m_webview_map.begin(); it != m_webview_map.end(); ++it)
    {
        if (!it->second)
            continue;
        if (it->second->m_webViewVisual)
        {
            auto offset = it->second->m_webViewVisual.Offset();
            auto size = it->second->m_webViewVisual.Size();
            if ((point.x >= offset.x) && (point.x < offset.x + size.x) && (point.y >= offset.y) &&
                (point.y < offset.y + size.y))
            {
                if(it->second->GetVisible()&&it->second->IsActiveMouse())
                {
                    item_uis.push_back(it->second);
                }
            }
        }
    }
    return item_uis;
}

std::shared_ptr<webview_item_ui> message_process_host::find_webview_from_id(const wchar_t* uniqueID)
{
    auto find_iterator = m_webview_map.find(uniqueID);
    if (find_iterator != m_webview_map.end())
    {
        return find_iterator->second;
    }
    return std::shared_ptr<webview_item_ui>();
}

void message_process_host::RunAsync(std::function<void()> callback)
{
    auto* task = new std::function<void()>(std::move(callback));
    PostMessage(GetMainWindowHandle(), s_runAsyncWindowMessage, reinterpret_cast<WPARAM>(task), 0);
}

void message_process_host::CreateDesktopWindowTarget()
{
    namespace abi = ABI::Windows::UI::Composition::Desktop;

    auto interop = m_compositor.as<abi::ICompositorDesktopInterop>();
    check_hresult(interop->CreateDesktopWindowTarget(
        m_main_hwnd, false, reinterpret_cast<abi::IDesktopWindowTarget**>(put_abi(m_target))));
}
