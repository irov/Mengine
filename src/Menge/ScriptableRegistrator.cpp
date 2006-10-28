// ***************************************************************
//  ScriptObjectFactory.cpp   version:  1.0   ·  date: 05/10/2006
//  -------------------------------------------------------------
//  IROV
//  -------------------------------------------------------------
//  Copyright (C) 2006 - All Rights Reserved
// ***************************************************************
// 
// ***************************************************************

#	include "ScriptableRegistrator.h"

#	include "Utility/factory.h"

namespace Menge
{
	namespace ScriptableRegistrator
	{
		typedef Utility::Factory<std::string,void,TGenFunc> TClassFactory;
		//////////////////////////////////////////////////////////////////////////
		void registration(const std::string &_key, TGenFunc _func)
		{
			TClassFactory::Registration(_key,_func);
		}
		//////////////////////////////////////////////////////////////////////////
		void implement(const std::string &_key, CLuaScript * _luaScript)
		{
			TClassFactory::Implement(_key,_luaScript);
		}
		//////////////////////////////////////////////////////////////////////////
		void ImplementAll(CLuaScript * _luaScript)
		{
			TClassFactory::ImplementAll(_luaScript);
		}
		//////////////////////////////////////////////////////////////////////////
		Auto::Auto(const std::string &_key, TGenFunc _func)
		{
			TClassFactory::Registration(_key,_func);
		}

	}
}