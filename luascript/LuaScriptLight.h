# pragma once

#	include <string>

class CLuaScript;

namespace NLuaScriptLight
{
	int DoFile(CLuaScript *LuaScript, const std::string &File);
}