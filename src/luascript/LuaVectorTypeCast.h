#	pragma once

#	include "LuaScriptInclude.h"

#	include "utility/singleton.h"

#	include <vector>

class CLuaVectorTypeCast
	:	public Utility::CSingleton<CLuaVectorTypeCast>
{
public:
	typedef TLuaObject (*TFunctionTypeCast)(void *);

	CLuaVectorTypeCast()
	{
		m_VectorFunctionTypeCast.resize(256);
	}

public:
	inline TLuaObject type_cast(size_t id, void * Object)
	{
		return m_VectorFunctionTypeCast[id](Object);
	}

	inline void insert( size_t id , TFunctionTypeCast Function)
	{
		size_t Size = m_VectorFunctionTypeCast.size();
		if( Size <= id )
		{
			m_VectorFunctionTypeCast.resize(Size*2);
		}
		m_VectorFunctionTypeCast[id] = Function;
	}

private: 
	std::vector<TFunctionTypeCast> m_VectorFunctionTypeCast;
};