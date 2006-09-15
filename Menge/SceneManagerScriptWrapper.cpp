#	include "SceneManager.h"

#	include "ScriptEngine.h"

#	include "LuaScript/LuaScript.h"

namespace RvEngine
{
	void SceneManager::RegisterScriptClass()
	{

		CLuaScript *LuaScript = m_scriptEngine->getLuaScript();

		LuaScript->DefineClass()
			[
				luabind::class_<SceneManager>("SceneManager")
				.def("getPath",&SceneManager::getPath)
			];

		LuaScript->SetLuaObject("SceneManager",boost::ref(*this));
	}
}