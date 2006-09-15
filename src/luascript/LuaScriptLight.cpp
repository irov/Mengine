#	include "LuaScriptLight.h"

#	include "LuaScript.h"

namespace NLuaScriptLight
{
	int DoFile(CLuaScript *LuaScript, const std::string &File)
	{
		return LuaScript->DoFile(File);
	}
}