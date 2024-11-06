#pragma once
#include "edge_ue_mgr.h"
#include <functional>
#include <string>

enum class e_web_download_state : unsigned char
{
    //正在下载
    progress,
    //下载完成
    complete,
    //下载中断
    interrupted
};

struct str_download_info
{
    //获取下载的 URL 地址
    std::wstring url;
    //下载文件的 MIME 类型
    std::wstring mimeType;
    //获取下载文件的内容描述
    std::wstring contentDisposition;
};
class EDGEAPI webview_interface
{
public:
    webview_interface(const wchar_t* uniqueID)
        : m_bIs3DUI(false)
        , b_activeMouse(false)
        , m_ActiveDebugTool(false)
        , m_uniqueID(uniqueID)
        , b_create_success(false)
    {
    }
    void set_download_path(const wchar_t* path) { m_download_path = path;}
    bool is_create_success()const {return b_create_success;}
    const str_download_info& get_download_info()const {return  m_download_info;}
    //加载网页 
    virtual void SetIs3DUI(bool bIs3DUI) { m_bIs3DUI = bIs3DUI; }
    bool GetIs3DUI()const {return  m_bIs3DUI; }
    //加载网页 
    virtual void LoadURL(const wchar_t* url) = 0;
    //前进
    virtual void GoForward() const = 0;
    //后退
    virtual void GoBack() const = 0;
    //重新加载
    virtual void ReLoad() const = 0;
    //停止加载
    virtual void Stop() const = 0;
    //设置webview大小
    virtual void SetBounds(RECT rect) = 0;
    //设置webview大小
    virtual void SetBounds(POINT offset, POINT size) = 0;
    //获取webview大小
    virtual RECT GetBounds() const = 0;
    //设置可见性
    virtual void SetVisible(bool is_visible) = 0;
    //获取可见性
    virtual bool GetVisible() const = 0;
    //禁用鼠标
    virtual void DisableMouse() { b_activeMouse = false; }
    //启用鼠标
    virtual void EnableMouse() { b_activeMouse = true; }
    //获取鼠标是都激活
    virtual bool IsActiveMouse() const { return b_activeMouse; }
    // 获取网页唯一标识
    std::wstring get_uniqueID() const { return m_uniqueID; }
    virtual void set_active_debug_tool(bool bIsEnableActiveTool) { m_ActiveDebugTool = bIsEnableActiveTool; }
    //@return：是否激活调试工具
    bool is_active_debug_tool() const { return m_ActiveDebugTool; }
    //给前端发送json类型的消息
    virtual void PostWebMessageAsJson(const wchar_t* Json) const = 0;
    //给前端发送string类型的消息
    virtual void PostWebMessageAsString(const wchar_t* InString) const = 0;
    virtual void Tick(float DeltaTime)=0;
    //执行前端脚本
    virtual void execute_script(const wchar_t* script,std::function<void(const wchar_t* resultObjectAsJson)> Callback=nullptr) = 0;
    //关闭网页
    virtual void Clear() =0;
    //获取句柄
    virtual HWND GetMainWindowHandle() = 0;
    
    virtual ~webview_interface()
    {
    }

    void SetNewWindowRequestedEvent(std::function<void(const wchar_t*)> Func)
    {
        NewWindowRequestedEvent = Func;
    }

    void SetNavigationCompletedEvent(std::function<void(bool)> Func)
    {
        NavigationCompletedEvent = Func;
    }

    void SetNavigationStartingEvent(std::function<void(const wchar_t*)> Func)
    {
        NavigationStartingEvent = Func;
    }
    void SetCursorChangedEvent(std::function<void(HCURSOR)> Func)
    {
        CursorChangedEvent = Func;
    }
    void SetDownloadStateChangedEvent( std::function<void(e_web_download_state download_state,const wchar_t* msg)> Func)
    {
        DownloadStateChangedEvent = Func;
    }
    
    void SetDownloadProgressEvent( std::function<void(INT64 received_size,INT64 total_size)> Func)
    {
        DownloadProgressEvent = Func;
    }
    void SetEstimatedDownloadTimeEvent(std::function<void(const wchar_t* time)> Func)
    {
        EstimatedDownloadTimeEvent = Func;
    }
protected:
    bool m_bIs3DUI;
    bool b_activeMouse;
    bool m_ActiveDebugTool;
    bool b_create_success;
    std::wstring m_uniqueID;
    std::wstring m_download_path;
    str_download_info m_download_info;
    std::function<void(const wchar_t* URL)> NewWindowRequestedEvent;
    std::function<void(bool)> NavigationCompletedEvent;
    std::function<void(const wchar_t* URL)> NavigationStartingEvent;
    std::function<void(HCURSOR)> CursorChangedEvent;
    std::function<void(e_web_download_state download_state,const wchar_t* msg)> DownloadStateChangedEvent;
    std::function<void(INT64 received_size,INT64 total_size)> DownloadProgressEvent;
    std::function<void(const wchar_t* time)> EstimatedDownloadTimeEvent;
};
