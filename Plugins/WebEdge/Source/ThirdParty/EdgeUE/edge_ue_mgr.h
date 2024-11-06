#pragma once
#pragma pack (push, 8)
#define EDGESTDCALL   __stdcall
#define EDGEEXPORT    __declspec(dllexport)
#define EDGEIMPORT    __declspec(dllimport)
#define BUILD_EDGE_DLL 1
#if BUILD_EDGE_DLL
#define EDGEAPI EDGEEXPORT
#define EDGEAPI_CALL EDGEEXPORT EDGESTDCALL
#else
#define EDGEAPI  EDGEIMPORT
#define EDGEAPI_CALL EDGEEXPORT EDGESTDCALL
#endif

#include <map>
#include <memory>
#include <string>
#include "Windows.h"


class ue_message_interface;
class webview_item_ui;
class message_process_host;
class webview_interface;
class EDGEAPI edge_ue_mgr
{
public:
    static edge_ue_mgr* instance();
    /*
    * 初始化函数
    */
    void init(ue_message_interface* message_interface = nullptr);
    static bool is_initialized();
    /*
    * 释放资源
    */
    void shutdown();
    /*
     * 设置引擎运行的根目录
     * @path:Binaries\Win64
     */
    void set_root_path(const wchar_t* path);

    std::wstring get_root_path() const {return m_RootPath;}
    /*
     * 创建或者重新加载网页
     * @uniqueID:网页唯一id
     * @url:url地址，必须带有http、https或者file://前缀
     */
    std::shared_ptr<webview_interface> create_webview(HWND hwnd,const wchar_t*uniqueID,const wchar_t* url,POINT offset,POINT size);
    std::shared_ptr<webview_interface> create_webview(HWND hwnd,const wchar_t*uniqueID,const wchar_t* url,RECT rect);
    std::shared_ptr<webview_interface> create_webview(HWND hwnd,const wchar_t*uniqueID,const wchar_t* url);
    /*
    * @return:消息接受类（json、string类型的消息通讯）
     */
    ue_message_interface* get_message_interface() const { return m_ue_message_interface_ptr; }
    /*
     * @return:根据句柄获取消息处理类（鼠标键盘之类的消息），没有则构建一个新的
     */
    std::shared_ptr<message_process_host> get_message_process(HWND hwnd);
    /*
     * 移除消息处理类
     */
     void remove_message_process(HWND hwnd);
    /*
     * 消息处理函数（window平台）
     */
    bool handle_window_message(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);
private:
    void download_webview_runtime();
    
    void AddRef();
    void Release();
    
    explicit edge_ue_mgr()
     : m_ue_message_interface_ptr(nullptr)
    {
     
    }

    static edge_ue_mgr* g_instance;

    ue_message_interface* m_ue_message_interface_ptr;
    std::wstring m_RootPath;
    std::map<HWND,std::weak_ptr<message_process_host>> message_process_host_map;

    float opacityValue =1.f; 
    int m_refCount = 1;
};
#pragma pack (pop)