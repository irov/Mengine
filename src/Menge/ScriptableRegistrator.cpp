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

namespace RvEngine
{
	namespace ScriptableRegistrator
	{
		typedef Utility::CFactory<std::string,void,TGenFunc> TClassFactory;
		typedef TClassFactory::TGlobalFactory TGlobalClassFactory;
		//////////////////////////////////////////////////////////////////////////
		static TClassFactory *getClassFactory()
		{
			TClassFactory *classFactory = 
				TGlobalClassFactory::GetInstance();

			return classFactory;
		}
		//////////////////////////////////////////////////////////////////////////
		void registration(const std::string &_key, TGenFunc _func)
		{
			getClassFactory()->Registration(_key,_func);
		}
		//////////////////////////////////////////////////////////////////////////
		void implement(const std::string &_key, CLuaScript * _luaScript)
		{
			getClassFactory()->Implement(_key,_luaScript);
		}
		//////////////////////////////////////////////////////////////////////////
		void ImplementAll(CLuaScript * _luaScript)
		{
			getClassFactory()->ImplementAll(_luaScript);
		}
		//////////////////////////////////////////////////////////////////////////
		Auto::Auto(const std::string &_key, TGenFunc _func)
		{
			getClassFactory()->Registration(_key,_func);
		}

	}
}