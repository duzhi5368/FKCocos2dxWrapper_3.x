//*************************************************************************
//	创建日期:	2014-8-4   17:53
//	文件名称:	IFKMessageDispatch.h
//  创 建 人:   王宏张 FreeKnight	
//	版权所有:	MIT
//	说    明:	
//*************************************************************************

#pragma once

//-------------------------------------------------------------------------
#include "FKCommonDef.h"
//-------------------------------------------------------------------------
// 网络消息处理器
struct IMessageHandler
{
	// 网络消息处理器
	virtual void		OnMessage( const char* p_szData, int p_nLen ) = 0;
};
//-------------------------------------------------------------------------
// 消息分发器
struct IMessageDispatch
{
	// 释放
	virtual void		Release() = 0;

public:
	// 消息分发函数
	virtual void		DispatchMessage( const char* p_szData, int p_nLen ) = 0;
	// 注册消息处理器
	virtual bool		RegisterMessageHandler( IMessageHandler* p_pHandler, uint8 p_ucMainMsgCode, uint16 p_usSubMsgCode ) = 0;
	// 注销消息处理器
	virtual bool		UnregisterMessageHandler( IMessageHandler* p_pHandler, uint8 p_ucMainMsgCode, uint16 p_usSubMsgCode ) = 0;
};
//-------------------------------------------------------------------------