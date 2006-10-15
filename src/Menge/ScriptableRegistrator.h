// ***************************************************************
//  ScriptObjectFactory.h   version:  1.0   ·  date: 05/10/2006
//  -------------------------------------------------------------
//  IROV
//  -------------------------------------------------------------
//  Copyright (C) 2006 - All Rights Reserved
// ***************************************************************
// 
// ***************************************************************

#	include <string>

class CLuaScript;
namespace Menge
{
	namespace ScriptableRegistrator
	{
		typedef void (*TGenFunc)(CLuaScript*);

		void implement(const std::string &_key, CLuaScript * _luaScript);
		void implementAll(CLuaScript * _luaScript);

		void registration(const std::string &_key, TGenFunc _func);

		class Auto
		{
		public:
			Auto(const std::string &_key, TGenFunc _func);
		};
	}
}