//*************************************************************************
//	创建日期:	2014-8-4   16:25
//	文件名称:	IFKEventEngine.h
//  创 建 人:   王宏张 FreeKnight	
//	版权所有:	MIT
//	说    明:	
//*************************************************************************

#pragma once

//-------------------------------------------------------------------------
#include "FKCommonDef.h"
//-------------------------------------------------------------------------
// 投票事件
struct IEventVoteSink
{
	/** 
	@param   p_ucEventID	：事件ID
	@param   p_ucSrcType	：发送源类型
	@param   p_unSrcID		: 发送源标识（实体为UID中"序列号"部份，非实体就为0）
	@param   p_szContext	: 上下文
	@param   p_nLen			: 上下文长度
	@return  如果返回false	: 则中断执行，否则继续向下执行
	*/
	virtual bool	OnVote( uint8 p_ucEventID, uint8 p_ucSrcType, uint32 p_unSrcID,
		const char* p_szContext, int p_nLen ) = 0;
};
//-------------------------------------------------------------------------
// 执行事件
struct IEventExecuteSink
{
	/** 
	@param   p_ucEventID	：事件ID
	@param   p_ucSrcType	：发送源类型
	@param   p_unSrcID		: 发送源标识（实体为UID中"序列号"部份，非实体就为0）
	@param   p_szContext	: 上下文
	@param   p_nLen			: 上下文长度
	*/
	virtual void	OnExecute( uint8 p_ucEventID, uint8 p_ucSrcType, uint32 p_unSrcID,
		const char* p_szContext, int p_nLen ) = 0;
};
//-------------------------------------------------------------------------
// 事件引擎
struct IEventEngine
{
	// 创建
	virtual bool	Create() = 0;
	// 释放
	virtual void	Release() = 0;
	
public:
	// 发送投票事件   
	virtual bool	FireVote( uint8 p_ucEventID, uint8 p_ucSrcType, uint32 p_unSrcID,
		const char* p_szContext, int p_nLen ) = 0;
	// 发送执行事件
	virtual void	FireExecute( uint8 p_ucEventID, uint8 p_ucSrcType, uint32 p_unSrcID,
		const char* p_szContext, int p_nLen ) = 0;

	/** 订阅投票事件
	@param   p_pSink		: 回调sink
	@param   p_ucEventID	: 事件ID
	@param   p_ucSrcType	：发送源类型
	@param	 p_unSrcID		: 发送源标识（实体为UID中"序列号"部份，非实体就为0），如果为0，则表示订阅所有
	@param	 p_szDesc		: 订阅者描述，用来调试程序
	@return  成功返回true,失败返回false 
	*/
	virtual bool	Subscibe( IEventVoteSink* p_pSink, uint8 p_ucEventID, uint8 p_ucSrcType,
		uint32 p_unSrcID, const char* p_szDesc ) = 0;

	/** 订阅执行事件
	@param   p_pSink		: 回调sink
	@param   p_ucEventID	: 事件ID
	@param   p_ucSrcType	：发送源类型
	@param	 p_unSrcID		: 发送源标识（实体为UID中"序列号"部份，非实体就为0），如果为0，则表示订阅所有
	@param	 p_szDesc		: 订阅者描述，用来调试程序
	@return  成功返回true,失败返回false 
	*/
	virtual bool	Subscibe( IEventExecuteSink* p_pSink, uint8 p_ucEventID, uint8 p_ucSrcType,
		uint32 p_unSrcID, const char* p_szDesc ) = 0;

	/** 取消订阅投票事件
	@param   p_pSink		: 回调sink
	@param   p_ucEventID	: 事件ID
	@param   p_ucSrcType	：发送源类型
	@param	 p_unSrcID		: 发送源标识（实体为UID中"序列号"部份，非实体就为0），如果为0，则表示订阅所有
	@return  成功返回true,失败返回false 
	*/
	virtual bool	UnSubscibe( IEventVoteSink* p_pSink, uint8 p_ucEventID, uint8 p_ucSrcType,
		uint32 p_unSrcID ) = 0;

	/** 取消订阅执行事件
	@param   p_pSink		: 回调sink
	@param   p_ucEventID	: 事件ID
	@param   p_ucSrcType	：发送源类型
	@param	 p_unSrcID		: 发送源标识（实体为UID中"序列号"部份，非实体就为0），如果为0，则表示订阅所有
	@return  成功返回true,失败返回false 
	*/
	virtual bool	UnSubscibe( IEventExecuteSink* p_pSink, uint8 p_ucEventID, uint8 p_ucSrcType,
		uint32 p_unSrcID ) = 0;
};
//-------------------------------------------------------------------------