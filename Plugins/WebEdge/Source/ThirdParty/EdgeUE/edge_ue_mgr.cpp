#include "edge_ue_mgr.h"
#include "message_process_host.h"
#include "ue_message_interface.h"
#include "webview_item_ui.h"
#include <DispatcherQueue.h>

#include <Urlmon.h>
#pragma comment(lib,"urlmon.lib")

winrt::Windows::System::DispatcherQueueController m_dispatcherQueueController{nullptr};

edge_ue_mgr* edge_ue_mgr::g_instance = nullptr;
static bool b_init_success = false;
bool edge_ue_mgr::is_initialized()
{
    return b_init_success;
}
edge_ue_mgr* edge_ue_mgr::instance()
{
    if(!g_instance)
    {
        g_instance = new edge_ue_mgr;
    }
    return g_instance;
}


void edge_ue_mgr::init(ue_message_interface* message_interface)
{
    if(b_init_success)
    {
        return;
    }
    b_init_success = true;

    //如果没有安装WebView2 Runtime，请创建新线程进行安装/下载。
    wil::unique_cotaskmem_string version_info;
    HRESULT hr = GetAvailableCoreWebView2BrowserVersionString(nullptr, &version_info);
    if (hr != S_OK)
    {
        AddRef();
        // 创建一个新的线程，将myObject.memberFunction作为线程的执行体
        std::thread newThread(&edge_ue_mgr::download_webview_runtime,this);
        // 将新的线程标记为后台运行
        newThread.detach();
    }

    m_ue_message_interface_ptr = message_interface;
    
    static decltype(::CreateDispatcherQueueController)* fnCreateDispatcherQueueController =
       nullptr;
    if (fnCreateDispatcherQueueController == nullptr)
    {
        HMODULE hmod = ::LoadLibraryEx(L"CoreMessaging.dll", nullptr, 0);
        if (hmod != nullptr)
        {
            fnCreateDispatcherQueueController =
                reinterpret_cast<decltype(::CreateDispatcherQueueController)*>(
                    ::GetProcAddress(hmod, "CreateDispatcherQueueController"));
        }
    }
    if (fnCreateDispatcherQueueController != nullptr)
    {
        DispatcherQueueOptions options{
            sizeof(DispatcherQueueOptions), DQTYPE_THREAD_CURRENT, DQTAT_COM_STA
        };
        fnCreateDispatcherQueueController(
            options, reinterpret_cast<ABI::Windows::System::IDispatcherQueueController**>(
                winrt::put_abi(m_dispatcherQueueController)));
    }
}

void edge_ue_mgr::shutdown()
{
    if(b_init_success)
    {
        for (auto it = message_process_host_map.begin(); it!= message_process_host_map.end(); ++it)
        {
            if (it->second.lock())
            {
                it->second.lock()->DestroyWinCompVisualTree();
            }
        }
        message_process_host_map.clear();
        b_init_success=false;

        if(g_instance)
        {
            delete g_instance;
            g_instance = nullptr;
        }
    }
  
}

void edge_ue_mgr::set_root_path(const wchar_t* path)
{
    m_RootPath = path;
}

std::shared_ptr<webview_interface> edge_ue_mgr::create_webview(HWND hwnd,const wchar_t* uniqueID, const wchar_t* url,POINT offset,POINT size)
{
    std::shared_ptr<webview_item_ui> webviewui = std::make_shared<webview_item_ui>(hwnd,uniqueID,url);
    webviewui->SetBounds(offset,size);
    return webviewui;
}

std::shared_ptr<webview_interface> edge_ue_mgr::create_webview(HWND hwnd,const wchar_t* uniqueID, const wchar_t* url, RECT rect)
{
    std::shared_ptr<webview_item_ui> webviewui = std::make_shared<webview_item_ui>(hwnd,uniqueID,url);
    webviewui->SetBounds(rect);
    return webviewui;
}

std::shared_ptr<webview_interface> edge_ue_mgr::create_webview(HWND hwnd,const wchar_t* uniqueID, const wchar_t* url)
{
    std::shared_ptr<webview_item_ui> webviewui = std::make_shared<webview_item_ui>(hwnd,uniqueID,url);
    return webviewui;
}


std::shared_ptr<message_process_host> edge_ue_mgr::get_message_process(HWND hwnd)
{
    auto find_iterator = message_process_host_map.find(hwnd);
    if(find_iterator != message_process_host_map.end())
    {
       return find_iterator->second.lock();
    }
    std::shared_ptr<message_process_host> message_process = std::make_shared<message_process_host>(hwnd,m_dispatcherQueueController);
    message_process->initialize();
    message_process_host_map[hwnd] = message_process;
    return message_process;
}

void edge_ue_mgr::remove_message_process(HWND hwnd)
{
    auto find_iterator = message_process_host_map.find(hwnd);
    if(find_iterator != message_process_host_map.end())
    {
        message_process_host_map.erase(find_iterator);
    }
}

bool edge_ue_mgr::handle_window_message(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
    if (message_process_host_map.size() == 0)
        return false;
    //消息单例
    auto find_iterator = message_process_host_map.find(hWnd);
    if (find_iterator != message_process_host_map.end())
    {
        if (find_iterator->second.lock())
        {
            return find_iterator->second.lock()->OnMouseMessage(message, wParam, lParam);
        }
   
    }

    return false;
}

void edge_ue_mgr::AddRef()
{
    InterlockedIncrement((LONG*)&m_refCount);
}

void edge_ue_mgr::Release()
{
    uint32_t refCount = InterlockedDecrement((LONG*)&m_refCount);
    //if (refCount == 0)
    //{
    //    delete this;
    //}
}

void edge_ue_mgr::download_webview_runtime()
{
    HRESULT hr = URLDownloadToFile(
        NULL, L"https://go.microsoft.com/fwlink/p/?LinkId=2124703",
        L".\\MicrosoftEdgeWebview2Setup.exe", 0, 0);
    if (hr == S_OK)
    {
        //将WebView2 Bootstrapper与您的应用程序打包，或使用fwlink下载
        //然后在运行时调用install。
        SHELLEXECUTEINFO shExInfo = { 0 };
        shExInfo.cbSize = sizeof(shExInfo);
        shExInfo.fMask = SEE_MASK_NOASYNC;
        shExInfo.hwnd = 0;
        shExInfo.lpVerb = L"runas";
        shExInfo.lpFile = L"MicrosoftEdgeWebview2Setup.exe";
        shExInfo.lpParameters = L" /silent /install";
        shExInfo.lpDirectory = 0;
        shExInfo.nShow = 0;
        shExInfo.hInstApp = 0;
        ShellExecuteEx(&shExInfo);
    }
    Release();
}
