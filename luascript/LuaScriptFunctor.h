#	pragma once

#	include "LuaScriptInclude.h"

template<class F>
class CLuaFunctor
	:	public luabind::functor<F>
{
public:
	CLuaFunctor(TLuaState	LuaState, const std::string &Name)
		:	luabind::functor<F>(LuaState,Name.c_str())
		,	m_LuaState(LuaState)
	{
	}

	CLuaFunctor(TLuaState	LuaState)
		:	m_LuaState(LuaState)
	{
	}

	CLuaFunctor()
		:	m_LuaState(0)
	{
	}

	CLuaFunctor & operator = ( const CLuaFunctor& LuaFunctor )
	{
		m_LuaState = LuaFunctor.m_LuaState;
		luabind::functor<F>::operator = (LuaFunctor);
		return *this;
	}

	void Attach(const std::string &Name)
	{
		operator = (CLuaFunctor<F>(m_LuaState,Name));
	}

private:
	TLuaState	m_LuaState;
};