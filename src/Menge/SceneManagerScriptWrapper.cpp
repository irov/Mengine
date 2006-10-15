#	include "SceneManager.h"

#	include "ScriptEngine.h"

#	include "LuaScript/LuaScript.h"

namespace Menge
{
	void SceneManager::RegisterScriptClass()
	{
		CLuaScript *LuaScript = 
			Keeper<ScriptEngine>::hostage()
			->getLuaScript();

		LuaScript->DefineClass()
			[
				luabind::class_<SceneManager>("SceneManager")
			];

		LuaScript->SetLuaObject("SceneManager",boost::ref(*this));
	}
}