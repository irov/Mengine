#	include "LuaScriptFunction.h"

#	include "LuaScript/LuaScript.h"
//////////////////////////////////////////////////////////////////////////
CLuaScriptFunction::CLuaScriptFunction(CLuaScript * LuaScript)
: m_LuaScript(LuaScript)
, m_FunctionObject(0)
{

}
//////////////////////////////////////////////////////////////////////////
CLuaScriptFunction::CLuaScriptFunction(CLuaScript * LuaScript, const std::string &Name)
: m_LuaScript(LuaScript)
, m_FunctionObject(0)
, m_FunctionName(Name)
{
}
//////////////////////////////////////////////////////////////////////////
void CLuaScriptFunction::BindFunction(const std::string &Name)
{
	if( m_FunctionObject == 0 )
	{
		m_FunctionName = Name;
	}
}
//////////////////////////////////////////////////////////////////////////
void CLuaScriptFunction::RegistryFunction(const std::string &Name)
{
	m_FunctionName = Name;
}
//////////////////////////////////////////////////////////////////////////
bool CLuaScriptFunction::Accept()
{
	if( m_FunctionObject == 0 )
	{
		m_FunctionObject = new luabind::object;

		bool Done = m_LuaScript->IsLuaObject(m_FunctionName);
		//bool Done =  true;
		
		if( Done )
		{
			(*m_FunctionObject) = m_LuaScript->GetLuaObject(m_FunctionName);
		}

		return Done;
	}
	return false;
}
//////////////////////////////////////////////////////////////////////////
bool CLuaScriptFunction::IsValid()
{
	if( m_FunctionObject == 0 )
	{
		return false;
	}

	if( m_FunctionObject->is_valid() == false )
	{
		return false;
	}

	return true;
}
//////////////////////////////////////////////////////////////////////////
template<class T> inline
T CLuaScriptFunction::CallFunction()
{
	if( IsValid() == false )
	{
		return T(0);
	}

	return m_LuaScript->CallFunction<T>((*m_FunctionObject));
}
//////////////////////////////////////////////////////////////////////////
void CLuaScriptFunction::CallFunctionVoid()
{
	CallFunction<void>();
}
//////////////////////////////////////////////////////////////////////////
int CLuaScriptFunction::CallFunctionInt()
{
	return CallFunction<int>();
}
//////////////////////////////////////////////////////////////////////////
bool CLuaScriptFunction::CallFunctionBool()
{
	return CallFunction<bool>();
}
//////////////////////////////////////////////////////////////////////////
void CLuaScriptFunction::CallFunctionVoidFloat(float Value1)
{
	if( IsValid() == false )
	{
		return;
	}

	m_LuaScript->CallFunction<void>((*m_FunctionObject),Value1);
}
//////////////////////////////////////////////////////////////////////////
void CLuaScriptFunction::CallFunctionVoidFloatFloat(float Value1, float Value2)
{
	if( IsValid() == false )
	{
		return;
	}
	
	m_LuaScript->CallFunction<void>((*m_FunctionObject),Value1,Value2);
}