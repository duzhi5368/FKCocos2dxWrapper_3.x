//*************************************************************************
//	创建日期:	2014-8-4   16:46
//	文件名称:	IFKScriptLoader.h
//  创 建 人:   王宏张 FreeKnight	
//	版权所有:	MIT
//	说    明:	
//*************************************************************************

#pragma once

//-------------------------------------------------------------------------
#include "FKCommonDef.h"
#include <string>
#include "../../../external/tinyxml2/tinyxml2.h"
//-------------------------------------------------------------------------
using std::string;
//-------------------------------------------------------------------------
#ifndef TIXML_USE_STL
#define TIXML_USE_STL
#endif
//-------------------------------------------------------------------------
enum ENUM_VariantType
{
	eVT_Empty	= 0,
	eVT_Int,			// 整数类型
	eVT_Float,			// 浮点类型
	eVT_String,			// 字符类型
	eVT_Iterator,		// 迭代器类型（支持多维脚本）
	
	eVT_Max,
};
//-------------------------------------------------------------------------
struct IFKScriptIterator;

// 复合变量单元
typedef struct _VARIANT
{
	ENUM_VariantType		m_eType;		// 数据类型

	union _VALUE_
	{
		long				m_lValue;		// 可能是整数类型
		double				m_dValue;		// 可能是浮点类型
		const char*			m_szValue;		// 可能是字符串类型
		IFKScriptIterator*	m_iteValue;		// 可能是迭代器子脚本（支持多维脚本）
	} m_Data;

}SVariant;
//-------------------------------------------------------------------------
struct IFKScriptIterator
{
	// 释放资源
	virtual void	Release() = 0;

public:
	// 获取数据个数
	virtual int		Size() const = 0;

public:
	virtual SVariant&	GetData( int p_nIndex ) = 0;
	virtual SVariant&	operator[] ( int p_nIndex ) = 0;
	virtual SVariant&	GetData( const char* p_szName ) = 0;
	virtual SVariant&	operator[] ( const char* p_szName ) = 0;
};
//-------------------------------------------------------------------------
// 复合类型封装，便于使用
class CVariant : protected SVariant
{
public:
	CVariant()
		: m_Variant(*((SVariant *)this))
	{
		m_Variant.m_eType = eVT_Empty;
		m_Variant.m_Data.m_lValue = 0;
	}

	CVariant( char* p_szStr ) 
		: m_Variant(*((SVariant *)this))
	{
		m_Variant.m_eType = eVT_String;
		m_Variant.m_Data.m_szValue = p_szStr;
	}

	CVariant( long p_lValue ) 
		: m_Variant(*((SVariant *)this))
	{
		m_Variant.m_eType = eVT_Int;
		m_Variant.m_Data.m_lValue = p_lValue;
	}

	CVariant( double p_dValue ) 
		: m_Variant(*((SVariant *)this))
	{
		m_Variant.m_eType = eVT_Float;
		m_Variant.m_Data.m_dValue = p_dValue;
	}

	CVariant( IFKScriptIterator* p_pIte ) 
		: m_Variant(*((SVariant *)this))
	{
		m_Variant.m_eType = eVT_Iterator;
		m_Variant.m_Data.m_iteValue = p_pIte;
	}

	CVariant(SVariant &value)
		: m_Variant(value)
	{
	}

	CVariant operator[]( int p_nIndex )
	{
		ASSERT( m_Variant.m_eType == eVT_Iterator );
		return (*m_Variant.m_Data.m_iteValue)[p_nIndex];
	}

	CVariant operator[](LPCSTR szName)
	{
		ASSERT( m_Variant.m_eType == eVT_Iterator );
		return (*m_Variant.m_Data.m_iteValue)[szName];
	}

	CVariant operator[](int8* szName)
	{
		ASSERT( m_Variant.m_eType == eVT_Iterator );
		return (*m_Variant.m_Data.m_iteValue)[szName];
	}

	int Size()
	{
		ASSERT( m_Variant.m_eType == eVT_Iterator );
		return m_Variant.m_Data.m_iteValue->Size();
	}

	operator const char*()
	{
		if ( m_Variant.m_eType != eVT_String)
		{
			_ChangeType( eVT_String );
		}

		return m_Variant.m_Data.m_szValue;
	}

	operator int()
	{
		if ( m_Variant.m_eType != eVT_Int )
		{
			_ChangeType( eVT_Int );
		}

		return m_Variant.m_Data.m_lValue;
	}

	operator long()
	{
		if ( m_Variant.m_eType != eVT_Int )
		{
			_ChangeType( eVT_Int );
		}

		return m_Variant.m_Data.m_lValue;
	}

	operator double()
	{
		if ( m_Variant.m_eType != eVT_Float )
		{
			_ChangeType( eVT_Float );
		}

		return m_Variant.m_Data.m_dValue;
	}

	ENUM_VariantType GetDataType()
	{
		return m_Variant.m_eType;
	}
protected:
	typedef void (CVariant::*ChangeTypeFunProc)();

	// 类型转换
	void _ChangeType( ENUM_VariantType p_eType )
	{
		static ChangeTypeFunProc sTypeChangePoc[eVT_Max][eVT_Max] =
		{
			{NULL, NULL, NULL, NULL, NULL},
			{NULL, NULL, &CVariant::_ChangeType_long2float, NULL, NULL},
			{NULL, &CVariant::_ChangeType_float2long, NULL, NULL, NULL},
			{NULL, &CVariant::_ChangeType_str2long, &CVariant::_ChangeType_str2float, NULL, NULL},
			{NULL, NULL, NULL, NULL, NULL}
		};

		if ( sTypeChangePoc[m_Variant.m_eType][p_eType] != NULL)
		{
			(this->*sTypeChangePoc[m_Variant.m_eType][p_eType])();
		}
	}

	// long -> float
	void _ChangeType_long2float()
	{
		m_Variant.m_Data.m_dValue = m_Variant.m_Data.m_lValue;
		m_Variant.m_eType = eVT_Float;
	}

	// float -> long
	void _ChangeType_float2long()
	{
		m_Variant.m_Data.m_lValue = (long)m_Variant.m_Data.m_dValue;
		m_Variant.m_eType = eVT_Int;
	}

	// str -> long
	void _ChangeType_str2long()
	{
		long lTemp = 0;
		lTemp = atoi(m_Variant.m_Data.m_szValue);
		m_Variant.m_Data.m_lValue = lTemp;
		m_Variant.m_eType = eVT_Int;
	}

	// str -> float
	void _ChangeType_str2float()
	{
		float dTemp = 0.0f;
		dTemp =(float) atof(m_Variant.m_Data.m_szValue);
		m_Variant.m_Data.m_dValue = dTemp;
		m_Variant.m_eType = eVT_Float;
	}
public:
	SVariant&		m_Variant;
};
//-------------------------------------------------------------------------
// Ini配置文件加载回调
struct IIniFileSink
{
	virtual void		OnLoadData( const char* p_szFileName, const char* p_szSection,
		const char* p_szKeyName, const char* p_szValue ) = 0;
};
//-------------------------------------------------------------------------
struct IIniFile
{
	virtual bool		isOpen() = 0;
	virtual bool		create(LPCSTR FileName) = 0;
	virtual bool		open(LPCSTR FileName) = 0;
	virtual bool		open(LPCSTR FileName,IIniFileSink *pSink) = 0;

	virtual bool		open(LPCSTR lpszData,int nSize) = 0;
	virtual bool		save() = 0;
	virtual bool		save(std::string &lpszData) = 0;
	virtual void		close() = 0;
	virtual void		clear() = 0;
	virtual void		clearSection(int8* lpSection) = 0;
	virtual void		getString(
		LPCSTR	lpSection,		// points to section name
		LPCSTR	lpKeyName,		// points to key name
		LPCSTR	lpDefault,		// points to default string
		int8*	lpRString,		// points to destination buffer
		unsigned long	dwSize			// size of string buffer
		) = 0;
	virtual void		getString(
		LPCSTR	lpSection,		// points to section name
		LPCSTR	lpKeyName,		// points to key name
		LPCSTR	lpDefault,		// points to default string
		std::string &RString	// points to destination buffer
		) = 0;

	virtual void		getInteger(
		LPCSTR	lpSection,		// points to section name
		LPCSTR	lpKeyName,		// points to key name
		int		nDefault,		// default value
		int		*Value          // return value
		) = 0;
	virtual void		getFloat(
		LPCSTR	lpSection,		// points to section name
		LPCSTR	lpKeyName,		// points to key name
		float32 fDefault,		// default value
		float32 *Value          // return value
		) = 0;
	virtual void		getStruct(
		LPCSTR	lpSection,		// pointer to section name
		LPCSTR	lpKeyName,		// pointer to key name
		void*	lpStruct,		// pointer to buffer that contains data to add
		unsigned long dwSize	// size, in bytes, of the buffer
		) = 0;

	virtual void		writeString(
		LPCSTR	lpSection,		// pointer to section name
		LPCSTR	lpKeyName,		// pointer to key name
		LPCSTR	lpString		// pointer to string to add
		) = 0;
	virtual void		writeInteger(
		LPCSTR	lpSection,		// pointer to section name
		LPCSTR	lpKeyName,		// pointer to key name
		int	    Value			// value to write
		) = 0;
	virtual void		writeFloat(
		LPCSTR	lpSection,		// pointer to section name
		LPCSTR	lpKeyName,		// pointer to key name
		float32   Value			// value to write
		) = 0;
	virtual void		writeStruct(
		LPCSTR	lpSection,		// pointer to section name
		LPCSTR	lpKeyName,		// pointer to key name
		void*	lpStruct,		// pointer to buffer that contains data to add
		unsigned long 	dwSize			// size, in bytes, of the buffer
		) = 0;
};
//-------------------------------------------------------------------------
// 文件加载回调
struct IScriptLoadSink
{
	// 文件加载 
	virtual bool	OnFileLoad( const char* p_szFile, IFKScriptIterator* p_pFilePointer, 
		tinyxml2::XMLDocument* p_pTiXmlDocument) = 0;
	// 文件更新
	virtual bool	OnFileUpdate( const char* p_szFile, IFKScriptIterator* p_pFilePointer, 
		tinyxml2::XMLDocument* p_pTiXmlDocument) = 0;
};
//-------------------------------------------------------------------------
// 文件加载器
struct IScriptLoader
{
	// 创建
	virtual bool	Create() = 0;
	// 销毁
	virtual void	Release() = 0;

public:
	// 功能: 加载一个文件(具有回调功能)
	// (*注意) 只有当你需要 您的程序具有 《脚本刷新》 功能的时候 才使用该函数 
	// 加载一个XML CSV获取其他的格式文件 通过回调获取文件数据指针
	// 注意：回调时获取的文件数据指针 不需要释放 ，这个函数内部处理了
	// 参数 : LPCSTR szFileName			-----文件名
	//		  IFileLoadSink *pSikn		-----文件加载后的回调指针，你的必须要实现该方法
	virtual bool LoadFile( const char* p_szFileName, IScriptLoadSink* p_pSink ) = 0;

	// 当需要刷脚本功能的时候 使用这个函数
	virtual bool UpdateFile( const char* p_szFileName) = 0;

	// 获取CSV脚本文件的迭代器
	virtual IFKScriptIterator*		LoadCSVFile( const char* p_szFileName,bool p_bEncrypt = false ) = 0;
	virtual IFKScriptIterator*		LoadCSVFile( const char* p_szFileName, const char* p_szBuffData, int p_nLen ) = 0;

	// 加载XML 文件自己释放啊
	virtual tinyxml2::XMLDocument*	LoadXmlFile( const char* p_szFileName,bool p_bEncrypt = false ) = 0;
	virtual tinyxml2::XMLDocument*	LoadXmlFile( const char* p_szFileName, const char* p_szBuffData, int p_nLen ) = 0;

	// 加载INI 文件 获取INI对象 
	virtual bool					LoadIniFile( const char* p_szFileName,IIniFileSink* p_pSink ) = 0;
	virtual IIniFile*				LoadIniFile( const char* p_szFileName ) = 0;

	// 更新INI
	virtual bool					UpdateIniFile( const char* p_szFileName ) = 0;
};
//-------------------------------------------------------------------------