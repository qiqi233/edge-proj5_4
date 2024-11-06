#pragma once
#include "ue_message_interface.h"

//用于接受js给UE发送的消息
class FEdgeMessageRecive : public ue_message_interface
{
public:
	FEdgeMessageRecive()=default;
	/*
	 * 接受json\string类型的消息
	 */
	virtual void NoitfyReciveMessage(const wchar_t* data, size_t size) override;

	/*--------------------------------注册给前端使用的函数--------------------------------*/
	virtual void QuitGame() override;
};

