#pragma once
#include <functional>
#include <wil/com.h>
#include <winrt/Windows.UI.Composition.Desktop.h>

class webview_item_ui;
/*
 * 接受外部（UE的SWindow）消息，处理后分配个各个webvie_item_ui类
 */
class message_process_host
{
    friend class edge_ue_mgr;
public:
    message_process_host(HWND hwnd, winrt::Windows::System::DispatcherQueueController QueueController);
    ~message_process_host();
    
    void initialize();
    //创建一个webview的容器，用于放界面
    void CreateWebViewVisual(std::shared_ptr<webview_item_ui> item_ui);
    void RefreshWebViewVisual(std::shared_ptr<webview_item_ui> item_ui);
    bool OnMouseMessage(UINT message, WPARAM wParam, LPARAM lParam);
    //删除所有网页
    void DestroyWinCompVisualTree();
    void DestroyVisualAsWebview(std::shared_ptr<webview_item_ui> item_ui);
    HWND GetMainWindowHandle() const;
    /*
     * 从id中获取网页控件
     */
    std::shared_ptr<webview_item_ui> find_webview_from_id(const wchar_t*uniqueID);

    void RunAsync(std::function<void()> callback);
protected:
    /*
    * 寻找当前鼠标位置的网页控件
    */
    std::list<std::shared_ptr<webview_item_ui>> find_webview_from_point(POINT point);
    
    void CreateDesktopWindowTarget();
    //创建组合根
    void CreateCompositionRoot();
    //追踪鼠标事件
    void TrackMouseEvents(DWORD mouseTrackingFlags);

private:
    winrt::Windows::UI::Composition::Compositor m_compositor{nullptr};
    winrt::Windows::System::DispatcherQueueController m_dispatcherQueueController{nullptr};
    winrt::Windows::UI::Composition::Desktop::DesktopWindowTarget m_target{nullptr};
    winrt::Windows::UI::Composition::ContainerVisual m_RootVisual{nullptr};
    //2DUI存放的插槽
    winrt::Windows::UI::Composition::ContainerVisual m_2DVisual{nullptr};
    //3DUI存放的插槽
    winrt::Windows::UI::Composition::ContainerVisual m_3DVisual{nullptr};
    HWND m_main_hwnd;
    bool m_isTrackingMouse;
    std::map<std::wstring,std::shared_ptr<webview_item_ui>> m_webview_map;
    bool m_isCapturingMouse;
    //点击的层级ID
    long OnClickLayerID=0;
};
