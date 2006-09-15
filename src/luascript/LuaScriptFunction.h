#	pragma once

#	include <string>

class CLuaScript;
class CObjectAbstract;

namespace luabind{
	namespace adl{
	class object;
	}
}

class CLuaScriptFunction
{
public:
	CLuaScriptFunction(CLuaScript * LuaScript, const std::string &Name);
	CLuaScriptFunction(CLuaScript * LuaScript);

public:
	inline CLuaScript * GetLuaScript(){return m_LuaScript;}
	inline luabind::adl::object * GetLuaObject(){return m_FunctionObject;}
	inline const std::string & GetScriptFunctionName(){return m_FunctionName;}

	void RegistryFunction(const std::string &Name);
	bool Accept();

	bool IsValid();

	void BindFunction(const std::string &Name);

	template<class T>
		T CallFunction();

	void CallFunctionVoid();
	int CallFunctionInt();
	bool CallFunctionBool();

	void CallFunctionVoidFloat(float Value1);
	void CallFunctionVoidFloatFloat(float Value1, float Value2);

protected:
	CLuaScript *		m_LuaScript;
	luabind::adl::object *	m_FunctionObject;
	std::string			m_FunctionName;
};