#	include "ScriptFunction.h"

#	include "ScriptEngine.h"
#	include "ScriptEngineError.h"

#	include "Node.h"

#	include "LuaScript/LuaScript.h"

using namespace Menge;

//////////////////////////////////////////////////////////////////////////
ScriptFunction::ScriptFunction(ScriptEngine *_scriptEngine)
: CLuaScriptFunction(_scriptEngine->getLuaScript())
{
}
//////////////////////////////////////////////////////////////////////////
ScriptFunction::ScriptFunction(ScriptEngine *_scriptEngine, const std::string &_function)
: CLuaScriptFunction(_scriptEngine->getLuaScript(),_function)
{
}
//////////////////////////////////////////////////////////////////////////
void ScriptFunction::callFunctionVoid()
{
	TRY_SCRIPT_ERROR;
	CLuaScriptFunction::CallFunctionVoid();
	CATCH_SCRIPT_ERROR;
}
//////////////////////////////////////////////////////////////////////////
int ScriptFunction::callFunctionInt()
{
	TRY_SCRIPT_ERROR;
	return CLuaScriptFunction::CallFunctionInt();
	CATCH_SCRIPT_ERROR;

	return 0;
}
//////////////////////////////////////////////////////////////////////////
bool ScriptFunction::callFunctionBool()
{
	TRY_SCRIPT_ERROR;
	return CLuaScriptFunction::CallFunctionBool();
	CATCH_SCRIPT_ERROR;

	return false;
}
//////////////////////////////////////////////////////////////////////////
void ScriptFunction::callFunctionVoidFloat(float _value)
{
	TRY_SCRIPT_ERROR
	CLuaScriptFunction::CallFunctionVoidFloat(_value);
	CATCH_SCRIPT_ERROR
}
//////////////////////////////////////////////////////////////////////////
void ScriptFunction::callFunctionVoidFloatFloat(float _value1, float _value2)
{
	TRY_SCRIPT_ERROR
	CLuaScriptFunction::CallFunctionVoidFloatFloat(_value1,_value2);
	CATCH_SCRIPT_ERROR
}
//////////////////////////////////////////////////////////////////////////
template<class T_Ret> inline
T_Ret ScriptFunction::callFunctionObject(Node *_node)
{
	if( IsValid() == true )
	{
		TLuaObject * luaThisObject = _node->getScriptable();

		TRY_SCRIPT_ERROR;
		return m_LuaScript->CallFunction<T_Ret>(*m_FunctionObject,*luaThisObject);
		CATCH_SCRIPT_ERROR;
	}

	ErrorMessage("Function '%s' is Invalid"
		, m_FunctionName.c_str()
		);

	return T_Ret(0);
}

//////////////////////////////////////////////////////////////////////////
void ScriptFunction::callFunctionVoidObject( Node *_node)
{
	return callFunctionObject<void>(_node);
}
//////////////////////////////////////////////////////////////////////////
int ScriptFunction::callFunctionIntObject( Node *_node)
{
	return callFunctionObject<int>(_node);
}
//////////////////////////////////////////////////////////////////////////
bool ScriptFunction::callFunctionBoolObject(Node *_node)
{
	return callFunctionObject<bool>(_node);
}
//////////////////////////////////////////////////////////////////////////
template<class T_Ret, class T_Value> inline
T_Ret ScriptFunction::callFunctionObjectValue(Node *_node, T_Value _value)
{
	if( IsValid() == true )
	{
		TLuaObject * luaThisObject = _node->getScriptable();

		TRY_SCRIPT_ERROR
			return m_LuaScript->CallFunction<T_Ret>(*m_FunctionObject,*luaThisObject,_value);
		CATCH_SCRIPT_ERROR
	}

	ErrorMessage("Function '%s' is Invalid"
		, m_FunctionName.c_str()
		);

	return T_Ret(0);
}
//////////////////////////////////////////////////////////////////////////
void ScriptFunction::callFunctionVoidObjectFloat(Node *_node, float _value)
{
	return callFunctionObjectValue<void>(_node,_value);
}
//////////////////////////////////////////////////////////////////////////
int ScriptFunction::callFunctionIntObjectFloat(Node *_node, float _value)
{
	return callFunctionObjectValue<int>(_node,_value);
}
//////////////////////////////////////////////////////////////////////////
void ScriptFunction::callFunctionVoidObjectString(Node *_node, const std::string & _value)
{
	return callFunctionObjectValue<void>(_node,_value);
}
//////////////////////////////////////////////////////////////////////////
int ScriptFunction::callFunctionIntObjectString(Node *_node, const std::string & _value)
{
	return callFunctionObjectValue<int>(_node,_value);
}
//////////////////////////////////////////////////////////////////////////
void ScriptFunction::callFunctionVoidObjectInt(Node *_node, int _value)
{
	return callFunctionObjectValue<void>(_node,_value);
}
//////////////////////////////////////////////////////////////////////////
int ScriptFunction::callFunctionIntObjectInt(Node *_node, int _value)
{
	return callFunctionObjectValue<int>(_node,_value);
}
//////////////////////////////////////////////////////////////////////////
template<class T_Ret, class T_Value_First, class T_Value_Second>
T_Ret ScriptFunction::callFunctionObjectValueValue(Node *_node, T_Value_First _value1, T_Value_Second _value2)
{
	if( IsValid() == true )
	{
		TLuaObject * luaThisObject = _node->getScriptable();

		TRY_SCRIPT_ERROR
			return m_LuaScript->CallFunction<T_Ret>(*m_FunctionObject,*luaThisObject,_value1,_value2);
		CATCH_SCRIPT_ERROR
	}

	ErrorMessage("Function '%s' is Invalid"
		, m_FunctionName.c_str()
		);

	return T_Ret(0);
}
//////////////////////////////////////////////////////////////////////////
void ScriptFunction::callFunctionVoidObjectFloatFloat(Node *_node, float _value1, float _value2)
{
	return callFunctionObjectValueValue<void>(_node,_value1,_value2);
}
//////////////////////////////////////////////////////////////////////////
int ScriptFunction::callFunctionIntObjectFloatFloat(Node *_node, float _value1, float _value2)
{
	return callFunctionObjectValueValue<int>(_node,_value1,_value2);
}
//////////////////////////////////////////////////////////////////////////
template<class T_Ret, class T_Value> inline
T_Ret ScriptFunction::callFunctionObjectObjectValue(Node *_node1,Node *_node2, T_Value _value)
{
	if( IsValid() == true )
	{
		TLuaObject * luaThisObject1 = _node1->getScriptable();
		TLuaObject * luaThisObject2 = _node2->getScriptable();

		TRY_SCRIPT_ERROR
			return m_LuaScript->CallFunction<T_Ret>(
			(*m_FunctionObject),
			*luaThisObject1,
			*luaThisObject2,
			_value);
		CATCH_SCRIPT_ERROR
	}

	ErrorMessage("Function '%s' is Invalid"
		, m_FunctionName.c_str()
		);

	return T_Ret(0);
}
//////////////////////////////////////////////////////////////////////////
void ScriptFunction::callFunctionVoidObjectObjectFloat(Node *_node1,Node *_node2, float _value)
{
	return callFunctionObjectObjectValue<void,float>(_node1,_node2,_value);
}
//////////////////////////////////////////////////////////////////////////
int ScriptFunction::callFunctionIntObjectObjectFloat(Node *_node1,Node *_node2, float _value)
{
	return callFunctionObjectObjectValue<int,float>(_node1,_node2,_value);
}
//////////////////////////////////////////////////////////////////////////
void ScriptFunction::callFunctionVoidObjectObjectInt(Node *_node1,Node *_node2, int _value)
{
	return callFunctionObjectObjectValue<void,int>(_node1,_node2,_value);
}
//////////////////////////////////////////////////////////////////////////
int ScriptFunction::callFunctionIntObjectObjectInt(Node *_node1,Node *_node2, int _value)
{
	return callFunctionObjectObjectValue<int,int>(_node1,_node2,_value);
}
//////////////////////////////////////////////////////////////////////////
template<class T_Ret>
T_Ret ScriptFunction::callFunctionObjectObject(Node *_node1,Node *_node2)
{
	if( IsValid() == true )
	{
		TLuaObject * luaThisObject1 = _node1->getScriptable();
		TLuaObject * luaThisObject2 = _node2->getScriptable();

		TRY_SCRIPT_ERROR
			return m_LuaScript->CallFunction<T_Ret>(
			(*m_FunctionObject),
			*luaThisObject1,
			*luaThisObject2);
		CATCH_SCRIPT_ERROR
	}
	
	ErrorMessage("Function '%s' is Invalid"
		, m_FunctionName.c_str()
		);

	return T_Ret(0);
}
//////////////////////////////////////////////////////////////////////////
void ScriptFunction::callFunctionVoidObjectObject(Node *_node1,Node *_node2)
{
	return callFunctionObjectObject<void>(_node1,_node2);
}
//////////////////////////////////////////////////////////////////////////
int ScriptFunction::callFunctionIntObjectObject(Node *_node1,Node *_node2)
{
	return callFunctionObjectObject<int>(_node1,_node2);
}