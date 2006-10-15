// ***************************************************************
//  InputScriptFunctionMap.h   version:  1.0   ·  date: 03/21/2006
//  -------------------------------------------------------------
//  IROV
//  -------------------------------------------------------------
//  Copyright (C) 2006 - All Rights Reserved
// ***************************************************************
// 
// ***************************************************************

#	pragma once

#	include <string>
#	include <map>

class TiXmlElement;


namespace Menge
{
	class InputEngine;
	class InputScriptFunctionCommand;

	class ScriptEngine;
	class ScriptFunction;
	class Node;


	class InputScriptFunctionMap
	{
	public:
		InputScriptFunctionMap(InputEngine *_inputEngine);
		~InputScriptFunctionMap();

	public:
		void Init(ScriptEngine *_scriptEngine);

		void Loader(TiXmlElement *Tree);

		InputScriptFunctionCommand * CreateInputControls(const std::string &Name);

		void AddChangeDelthaMouse( const std::string &Function );
		void AddChangeWhellMouse( const std::string &Function );

		void SetActive(bool Value);
		bool IsActive(){return m_Active;}

		void ActiveCommand(const std::string &Name);
		void DeactiveCommand(const std::string &Name);
		void Errase(const std::string &Name);

		void Update(Node *ObjectIputed);

	private:

		void InitDIK();
		short int GetDIK(const std::string &Name);
	private:
		typedef std::map<std::string, InputScriptFunctionCommand * > TMapUserInputCommand;	
		typedef std::map<std::string, short int > TMapDIK;	

		TMapUserInputCommand	m_MapUserInputCommand;

		InputEngine *			m_InputEngine;
		ScriptEngine *			m_ScriptEngine;
		ScriptFunction *		m_ScriptFunctionChangeDelthaMouse;
		ScriptFunction *		m_ScriptFunctionChangeWhellMouse;
		bool					m_Active;

		TMapDIK		m_MapDIK;
	};
}