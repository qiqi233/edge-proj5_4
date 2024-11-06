#pragma once
#include "edge_ue_mgr.h"
/*
 * 消息接受类，可接受字符串或者json数据，这是一个单例接口，所有的网页消息最终都会调用到这里
 */
class EDGEAPI ue_message_interface
{
public:
    ue_message_interface() = default;
    virtual ~ue_message_interface() = default;
    virtual void NoitfyReciveMessage(const wchar_t* data, size_t size) = 0;
    virtual void QuitGame() = 0;
    
};
