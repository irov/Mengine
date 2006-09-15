// ***************************************************************
//  InputScriptFunctionCommand.h   version:  1.0   ·  date: 03/21/2006
//  -------------------------------------------------------------
//  IROV
//  -------------------------------------------------------------
//  Copyright (C) 2006 - All Rights Reserved
// ***************************************************************
// 
// ***************************************************************

#	pragma once

#	include <list>
#	include <string>


namespace RvEngine
{
	class ScriptFunction;
	class ScriptEngine;

	class Node;

	struct UserKeyState
	{
		UserKeyState()
			: m_Key(0)
			, m_State(0)
		{
		}

		UserKeyState(short int Key, short int State)
			:m_Key(Key)
			,m_State(State)
		{
		}

		short int m_Key;
		short int m_State;
	};

	typedef std::list<UserKeyState> TListUserKeyState;

	class InputScriptFunctionCommand
	{
	public:
		InputScriptFunctionCommand();
		InputScriptFunctionCommand(ScriptEngine *_scriptEngine);

		~InputScriptFunctionCommand();

		void BindFunction( const std::string & FunctionName);

		void AddKey( short int Key, short int State);
		void AddMouseButton(short int Button, short int State);

		void Execute(Node *_node);

		void Active();
		void Deactive();

		bool IsActive(){return m_Active;}

		bool Valid();

		TListUserKeyState::iterator BeginKey();
		TListUserKeyState::iterator EndKey();

		TListUserKeyState::iterator BeginMouse();
		TListUserKeyState::iterator EndMouse();

	private:
		ScriptFunction *		m_ScriptFunction;
		TListUserKeyState		m_ListUserKey;
		TListUserKeyState		m_ListUserMouse;
		bool					m_Active;
	};

}