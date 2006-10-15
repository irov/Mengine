// ***************************************************************
//  InputScriptFunctionCommand.cpp   version:  1.0   ·  date: 03/21/2006
//  -------------------------------------------------------------
//  IROV
//  -------------------------------------------------------------
//  Copyright (C) 2006 - All Rights Reserved
// ***************************************************************
// 
// ***************************************************************

#	include "InputScriptFunctionCommand.h"

#	include "ScriptFunction.h"

namespace Menge
{
	//////////////////////////////////////////////////////////////////////////
	InputScriptFunctionCommand::InputScriptFunctionCommand()
		: m_Active(false)
		, m_ScriptFunction(0)
	{

	}
	//////////////////////////////////////////////////////////////////////////
	InputScriptFunctionCommand::InputScriptFunctionCommand(ScriptEngine *_scriptEngine)
		: m_Active(false)
		, m_ScriptFunction(new ScriptFunction(_scriptEngine))
	{

	}
	//////////////////////////////////////////////////////////////////////////
	InputScriptFunctionCommand::~InputScriptFunctionCommand()
	{
		delete m_ScriptFunction;
	}
	//////////////////////////////////////////////////////////////////////////
	void InputScriptFunctionCommand::BindFunction( const std::string & FunctionName)
	{
		m_ScriptFunction->BindFunction(FunctionName);
		m_ScriptFunction->Accept();
	}

	//////////////////////////////////////////////////////////////////////////
	void InputScriptFunctionCommand::Active()
	{
		m_Active = true;
	}
	//////////////////////////////////////////////////////////////////////////
	void InputScriptFunctionCommand::Deactive()
	{
		m_Active = false;
	}
	//////////////////////////////////////////////////////////////////////////
	void InputScriptFunctionCommand::AddKey(short int Key, short int State)
	{
		m_ListUserKey.push_back(UserKeyState(Key,State));
	}
	//////////////////////////////////////////////////////////////////////////
	void InputScriptFunctionCommand::AddMouseButton(short int Button, short int State)
	{
		m_ListUserMouse.push_back(UserKeyState(Button,State));
	}
	//////////////////////////////////////////////////////////////////////////
	TListUserKeyState::iterator InputScriptFunctionCommand::BeginKey()
	{
		return m_ListUserKey.begin();
	}
	//////////////////////////////////////////////////////////////////////////
	TListUserKeyState::iterator InputScriptFunctionCommand::EndKey()
	{
		return m_ListUserKey.end();
	}
	//////////////////////////////////////////////////////////////////////////
	TListUserKeyState::iterator InputScriptFunctionCommand::BeginMouse()
	{
		return m_ListUserMouse.begin();
	}
	//////////////////////////////////////////////////////////////////////////
	TListUserKeyState::iterator InputScriptFunctionCommand::EndMouse()
	{
		return m_ListUserMouse.end();

	}
	//////////////////////////////////////////////////////////////////////////
	void InputScriptFunctionCommand::Execute(Node *_node)
	{
		m_ScriptFunction->callFunctionVoidObject(_node);
	}
	//////////////////////////////////////////////////////////////////////////
	bool InputScriptFunctionCommand::Valid()
	{
		return (m_ListUserKey.size() + m_ListUserMouse.size()) > 0;
	}
}