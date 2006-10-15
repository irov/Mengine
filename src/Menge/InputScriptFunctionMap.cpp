// ***************************************************************
//  InputScriptFunctionMap.cpp   version:  1.0   ·  date: 03/21/2006
//  -------------------------------------------------------------
//  IROV
//  -------------------------------------------------------------
//  Copyright (C) 2006 - All Rights Reserved
// ***************************************************************
// 
// ***************************************************************

#	include "InputScriptFunctionMap.h"
#	include "InputScriptFunctionCommand.h"
#	include "ScriptFunction.h"

#	include "InputEngine.h"

#	include "Utility/algorithm.h"

namespace Menge
{

	//////////////////////////////////////////////////////////////////////////
	InputScriptFunctionMap::InputScriptFunctionMap(InputEngine *_inputEngine)
		: m_InputEngine(_inputEngine)
		, m_ScriptEngine(0)
		, m_ScriptFunctionChangeDelthaMouse(0)
		, m_ScriptFunctionChangeWhellMouse(0)
		, m_Active(false)
	{
		InitDIK();
	}
	//////////////////////////////////////////////////////////////////////////
	InputScriptFunctionMap::~InputScriptFunctionMap()
	{
		delete m_ScriptFunctionChangeDelthaMouse;
		delete m_ScriptFunctionChangeWhellMouse;

		Utility::erase_ptr_container(m_MapUserInputCommand);
	}
	//////////////////////////////////////////////////////////////////////////
	void InputScriptFunctionMap::Init(ScriptEngine *_scriptEngine)
	{
		m_ScriptEngine = _scriptEngine;

		m_ScriptFunctionChangeDelthaMouse = 
			new ScriptFunction(m_ScriptEngine);

		m_ScriptFunctionChangeWhellMouse =
			new ScriptFunction(m_ScriptEngine);
	}
	//////////////////////////////////////////////////////////////////////////
	InputScriptFunctionCommand * InputScriptFunctionMap::CreateInputControls(const std::string &Name)
	{
		InputScriptFunctionCommand * FunctionKey = 
			new InputScriptFunctionCommand(m_ScriptEngine);

		m_MapUserInputCommand.insert(std::make_pair(Name,FunctionKey));

		return FunctionKey;
	}
	//////////////////////////////////////////////////////////////////////////
	void InputScriptFunctionMap::SetActive(bool Value)
	{
		m_Active = Value;
	}
	//////////////////////////////////////////////////////////////////////////
	void InputScriptFunctionMap::ActiveCommand(const std::string &Name)
	{
		TMapUserInputCommand::iterator it_f = m_MapUserInputCommand.find(Name);
		if( it_f != m_MapUserInputCommand.end() )
		{
			it_f->second->Active();
		}
	}
	//////////////////////////////////////////////////////////////////////////
	void InputScriptFunctionMap::DeactiveCommand(const std::string &Name)
	{
		TMapUserInputCommand::iterator it_f = m_MapUserInputCommand.find(Name);
		if( it_f != m_MapUserInputCommand.end() )
		{
			it_f->second->Deactive();
		}
	}
	//////////////////////////////////////////////////////////////////////////
	void InputScriptFunctionMap::Errase(const std::string &Name)
	{
		TMapUserInputCommand::iterator it_f = m_MapUserInputCommand.find(Name);
		if( it_f != m_MapUserInputCommand.end() )
		{
			delete it_f->second;
			m_MapUserInputCommand.erase(it_f);
		}
	}
	//////////////////////////////////////////////////////////////////////////
	void InputScriptFunctionMap::AddChangeDelthaMouse(const std::string &Function )
	{
		m_ScriptFunctionChangeDelthaMouse->BindFunction(Function);
		m_ScriptFunctionChangeDelthaMouse->Accept();
	}
	//////////////////////////////////////////////////////////////////////////
	void InputScriptFunctionMap::AddChangeWhellMouse(const std::string &Function )
	{
		m_ScriptFunctionChangeWhellMouse->BindFunction(Function);
		m_ScriptFunctionChangeWhellMouse->Accept();
	}
	//////////////////////////////////////////////////////////////////////////
	void InputScriptFunctionMap::Update(Node *_node)
	{
		if( m_Active == false )
		{
			return;
		}

		TMapUserInputCommand::iterator it_b = m_MapUserInputCommand.begin();
		TMapUserInputCommand::iterator it_e = m_MapUserInputCommand.end();
		for(;it_b!=it_e;++it_b)
		{
			InputScriptFunctionCommand * UserKeyList = it_b->second;
			if( UserKeyList->IsActive() == false || UserKeyList->Valid() == false )
			{
				continue;
			}

			bool Done = true;
			if( Done )
			{
				TListUserKeyState::iterator UKL_ITB = 
					UserKeyList->BeginKey();

				TListUserKeyState::iterator UKL_ITE = 
					UserKeyList->EndKey();

				for( ;UKL_ITB!=UKL_ITE;++UKL_ITB)
				{
					if (m_InputEngine->IsKey(UKL_ITB->m_Key,UKL_ITB->m_State) == false)
					{
						Done = false;
						break;
					}
				}
			}

			if( Done )
			{
				TListUserKeyState::iterator UKL_ITB = 
					UserKeyList->BeginMouse();

				TListUserKeyState::iterator UKL_ITE = 
					UserKeyList->EndMouse();

				for( ;UKL_ITB!=UKL_ITE;++UKL_ITB)
				{
					if (m_InputEngine->IsButton(UKL_ITB->m_Key,UKL_ITB->m_State) == false)
					{
						Done = false;
						break;
					}
				}
			}

			if( Done )
			{
				UserKeyList->Execute(_node);
			}
		}

		if( m_ScriptFunctionChangeDelthaMouse )
		{
			int dm_x = m_InputEngine->GetDelta(0);
			int dm_y = m_InputEngine->GetDelta(1);

			if ( dm_x || dm_y )
			{
				m_ScriptFunctionChangeDelthaMouse
					->callFunctionVoidObjectFloatFloat(
					_node,
					float(dm_x),
					float(dm_y));

			}
		}

		if( m_ScriptFunctionChangeWhellMouse )
		{
			int dm_z = m_InputEngine->GetDelta(2);

			if( dm_z )
			{
				m_ScriptFunctionChangeWhellMouse
					->callFunctionVoidObjectFloat(
					_node,
					float(dm_z));

			}
		}
	}
}