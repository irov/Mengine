#	include "ScriptEngine.h"
#	include "ScriptEngineError.h"

#	include "ScriptFunction.h"

#	include "Node.h"

#	include "LuaScript/LuaScript.h"

using namespace RvEngine;

ScriptEngine::ScriptEngine()
: m_luaScript(0)
{

}
//////////////////////////////////////////////////////////////////////////
ScriptEngine::~ScriptEngine()
{
	delete m_luaScript;
}
//////////////////////////////////////////////////////////////////////////
void ScriptEngine::init()
{
	m_luaScript = new CLuaScript();

	TRY_SCRIPT_FUNCTION(m_luaScript->Init());

	//TODO
	//TRY_SCRIPT_FUNCTION(NScriptObjectFactory::ImplementAll(m_LuaScript));

	//	m_luaScript->SetLuaObject("ScriptEngine",this);
}
//////////////////////////////////////////////////////////////////////////
CLuaScript* ScriptEngine::getLuaScript()
{
	return m_luaScript;
}
//////////////////////////////////////////////////////////////////////////
void ScriptEngine::setLuaObject(const std::string &_name, Node *_object )
{
	TLuaObject *luaObject = _object->getScriptable();

	m_luaScript->SetLuaObject(_name,*luaObject);
}
//////////////////////////////////////////////////////////////////////////
void ScriptEngine::include(const std::string &_file)
{
	m_queueLuaFile.insert(_file);
}
//////////////////////////////////////////////////////////////////////////
int ScriptEngine::doString(const std::string &_string)
{
	int errorCode = -1;

	TRY_SCRIPT_FUNCTION(errorCode = m_luaScript->DoString(_string));

	if( errorCode != 0 )
	{
		ErrorMessage("Bad parse string: \n %s \n error code [%d]"
			,_string.c_str()
			,errorCode);

		return errorCode;
	}

	return 0;
}
//////////////////////////////////////////////////////////////////////////
int ScriptEngine::doFile(const std::string &_file)
{
	TSetLuaFile::iterator it_find = m_setLuaFile.find(_file);
	if( it_find != m_setLuaFile.end() )
	{
		return 0;
	}

	int errorCode = -1;
	TRY_SCRIPT_FUNCTION( errorCode = m_luaScript->DoFile(_file) );

	if( errorCode == 0 )
	{
		m_setLuaFile.insert(_file);
	}
	else
	{
		ErrorMessage("Bad parse file [%s] error code [%d]"
			,_file.c_str()
			,errorCode);

		return errorCode;
	}

	TSetLuaFile::iterator it_file = m_queueLuaFile.begin();
	if( it_file != m_queueLuaFile.end())
	{
		std::string nextFile = (*it_file);

		m_queueLuaFile.erase(it_file);

		doFile(nextFile);
	}

	return errorCode;
}
//////////////////////////////////////////////////////////////////////////
ScriptFunction * ScriptEngine::genEvent(const std::string &_name)
{
	return new ScriptFunction(this,_name);
}
//////////////////////////////////////////////////////////////////////////
template<class T> inline
T ScriptEngine::callFunction(const std::string &_function, Node *_object)
{
	if( _object == 0 )
	{
		ErrorMessage("CallFunction[%s] Object is null",_function.c_str());
		return T(0);
	}

	if( m_luaScript->IsLuaObject(_function) == false )
	{
		ErrorMessage("CallFunction[%s] Function is null",_function.c_str());
		return T(0);
	}

	TLuaObject *luaThisObject = _object->getScriptable();

	TRY_SCRIPT_ERROR
		return m_luaScript->CallFunction<T>(_function,*luaThisObject);
	CATCH_SCRIPT_ERROR

		return T(0);
}
//////////////////////////////////////////////////////////////////////////
void ScriptEngine::callFunctionVoid(const std::string &_function, Node *_object)
{
	callFunction<void>(_function,_object);
}
//////////////////////////////////////////////////////////////////////////
int ScriptEngine::callFunctionInt(const std::string &_function, Node *_object)
{
	return callFunction<int>(_function,_object);
}
//////////////////////////////////////////////////////////////////////////
template<class T> inline
T ScriptEngine::callFunction(const std::string &_function)
{
	if( m_luaScript->IsLuaObject(_function) == false )
	{
		ErrorMessage("CallFunction[%s] Function is null",_function.c_str());
		return T(0);
	}

	TRY_SCRIPT_FUNCTION( return m_luaScript->CallFunction<T>(_function) );

	return T(0);
}
//////////////////////////////////////////////////////////////////////////
void ScriptEngine::callFunctionVoid(const std::string &_function)
{
	callFunction<void>(_function);
}
//////////////////////////////////////////////////////////////////////////
int ScriptEngine::callFunctionInt(const std::string &_function)
{
	return callFunction<int>(_function);
}
//////////////////////////////////////////////////////////////////////////
const std::string & ScriptEngine::getErrorString()
{
	return m_luaScript->GetError();
}
