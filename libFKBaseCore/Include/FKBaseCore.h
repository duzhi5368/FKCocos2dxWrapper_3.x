//*************************************************************************
//	创建日期:	2014-8-6   14:01
//	文件名称:	FKBaseCore.h
//  创 建 人:   王宏张 FreeKnight	
//	版权所有:	MIT
//	说    明:	
//*************************************************************************

#pragma once

//-------------------------------------------------------------------------
#include "../../FKCommon/IFKBaseCore.h"
//-------------------------------------------------------------------------
class CBaseCore : public IBaseCore
{
public:
	CBaseCore();
	virtual ~CBaseCore();
public:
	// 销毁
	virtual void	Release();
	// 创建
	virtual bool	Create();

public:
	// 创建socket
	virtual bool	CreateClientSocket( IClientSocketSink* p_pClientSocketSink,
		IClientSocket** p_ppClientSocket, bool p_bEncrypt = false );
	// 创建时间轴
	virtual bool	CreateTimerAxis( ITimerAxis** p_ppTimerAxis );
	// 创建事件引擎
	virtual bool	CreateEventEngine( IEventEngine** p_ppEventEngine );
	// 创建脚本系统
	virtual bool	CreateScriptLoader( IScriptLoader** p_ppScriptLoader );
	// 创建消息分发器
	virtual bool	CreateMessageDispath( IMessageDispatch** p_ppMessageDispath );
public:
	// 更新函数
	virtual void	Update();
private:
	CClientMgr				m_ClientMgr;
};