//*************************************************************************
//	创建日期:	2014-8-7   13:37
//	文件名称:	DataArchive.h
//  创 建 人:   王宏张 FreeKnight	
//	版权所有:	MIT
//	说    明:	
//			使用：CArchive需要外部提供缓冲区，传入一个局部buffer变量，如:
//			char buf[1024];
//			CDataArchive archive(buf,1024);
//			archive.Write("hello",5);
//*************************************************************************

#pragma once

//-------------------------------------------------------------------------
#include "../FKCommonDef.h"
//-------------------------------------------------------------------------
class CDataArchive
{
protected:
	CDataArchive()
		: m_nOffset( 0 )
		, m_pBindBuffer( NULL )
		, m_nMaxSize( 0 )
	{

	}

	void _Init( void* p_szBindBuffer, int p_nMaxSize )
	{
		if( p_szBindBuffer == NULL || p_nMaxSize <= 0 )
			return;
		m_nOffset = 0;
		m_pBindBuffer = p_szBindBuffer;
		m_nMaxSize = p_nMaxSize;
	}

public:
	CDataArchive( void* p_szBindBuf, int p_nMaxSize )
	{
		_Init( p_szBindBuf, p_nMaxSize );
	}
	~CDataArchive(){

	}

public:
	// 写入数据
	virtual bool Write( const void* p_pBuf, int p_nLen )
	{
		if( p_pBuf == NULL || p_nLen <= 0 || m_nOffset + p_nLen > m_nMaxSize )
			return false;

		memcpy( (char*)m_pBindBuffer + m_nOffset, p_pBuf, p_nLen );
		m_nOffset += p_nLen;
		return true;
	}

	// 读出数据
	virtual bool Read( FK_OUT_PARAM void* p_pBuf, int p_nLen )
	{
		if( p_pBuf == NULL || p_nLen <= 0 || m_nOffset + p_nLen > m_nMaxSize )
			return false;

		memcpy( p_pBuf, (char*)m_pBindBuffer + m_nOffset, p_nLen );
		m_nOffset += p_nLen;
		return true;
	}

	// 获取当前缓冲指针
	virtual void* GetCurBufferHead()
	{
		return (char*)m_pBindBuffer + m_nOffset;
	}

	// 获取当前偏移值
	virtual int GetBufferOffset()
	{
		return m_nOffset;
	}

	// 获取剩余值（未偏移值）
	// 读数据时，获取剩余数据长度。
	// 写数据时，获取可写的剩余缓冲长度。
	virtual int GetLeftBufferLen()
	{
		return m_nMaxSize - m_nOffset;
	}

	// 将指针向后移动指定字节，返回移动前的指针
	// 若需要获得移动后的指针，seek之后 GetCurBufferHead()即可
	// 若移动失败，则返回 -1
	virtual int Seek( int p_nOffset )
	{
		if( m_nOffset + p_nOffset > m_nMaxSize || m_nOffset + p_nOffset < 0 )
			return -1;
		int nOld = m_nOffset;
		m_nOffset += p_nOffset;
		return nOld;
	}

	// 将指针移动到指定位置，返回移动前的指针
	virtual int SeekTo( int p_nPtr = 0 )
	{
		if( p_nPtr > m_nMaxSize || p_nPtr < 0 )
			return -1;

		int nOld = m_nOffset;
		m_nOffset = p_nPtr;
		return nOld;
	}
private:
	int			m_nOffset;
	void*		m_pBindBuffer;
	int			m_nMaxSize;
};
//-------------------------------------------------------------------------