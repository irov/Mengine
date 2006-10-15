// ***************************************************************
//  InputEngine.cpp   version:  1.0   ·  date: 03/21/2006
//  -------------------------------------------------------------
//  IROV
//  -------------------------------------------------------------
//  Copyright (C) 2006 - All Rights Reserved
// ***************************************************************
// 
// ***************************************************************

#	include "InputEngine.h"

#	include "InputSystemInterface.h"

#	include "ScriptEngine.h"

#	include "InputScriptFunctionMap.h"

#	include "Manager/XmlManager.h"
#	include "Manager/ErrorMessage.h"

#	include "Utility/algorithm.h"

namespace Menge
{
	////////////////////////////////////////////////////////////////////////////////////////////////////
	InputEngine::InputEngine(const std::string &_dllModule)
		: DllModuleInterface<InputSystemInterface>(_dllModule)
	{
		Keeper<InputEngine>::keep(this);
	}
	////////////////////////////////////////////////////////////////////////////////////////////////////
	InputEngine::~InputEngine()
	{
		Utility::erase_ptr_container(m_MapInputScriptFunctionMap);
	}
	////////////////////////////////////////////////////////////////////////////////////////////////////
	bool InputEngine::Init()
	{
		//m_interface->Init()
		
		//CWindowEngine *WndEng = GetWindowEngine();

		//HWND hWnd = WndEng->hWnd();

		//m_InputManager = new CInputManager();

		//m_InputManager->Init(hWnd,INPUT_ALL);
		return true;
	}
	////////////////////////////////////////////////////////////////////////////////////////////////////
	void InputEngine::Update()
	{
		m_interface->update();
	}
	////////////////////////////////////////////////////////////////////////////////////////////////////
	bool InputEngine::LoadFunctionMap(const std::string &File)
	{
		//<INPUT_CONTROLS NAME = "MainPlayer" SCRIPT_FILE = "InputMainPlayer.lua" >

		//std::string InputPath = NPath::TemplateObject(File);

		//XML_PARSE_FILE(InputPath)
		//{
		//	XML_CHECK_NODE("INPUT_CONTROLS")
		//	{
		//		XML_DEF_ATTRIBUTES_NODE(NAME);

		//		TMapInputScriptFunctionMap::iterator it_find = 
		//			m_MapInputScriptFunctionMap.find(NAME);

		//		if( it_find != m_MapInputScriptFunctionMap.end() )
		//		{
		//			break;
		//		}

		//		CInputScriptFunctionMap *FunctionMap =
		//			new CInputScriptFunctionMap(this);

		//		m_MapInputScriptFunctionMap
		//			.insert(std::make_pair(NAME,FunctionMap));

		//		CScriptEngine *ScriptEngine = GetScriptEngine();
		//		FunctionMap->Init(ScriptEngine);

		//		XML_DEF_ATTRIBUTES_NODE(SCRIPT_FILE);

		//		if( SCRIPT_FILE.empty() == false )
		//		{
		//			std::string ScriptPath = NPath::TemplateObject(SCRIPT_FILE);
		//			ScriptEngine->DoFile(ScriptPath);
		//		}

		//		FunctionMap->Loader(XML_CURRENT_NODE);
		//	}
		//}
		//XML_INVALID_PARSE()
		//{
		//	ErrorMessage("Invalid parse xml input file '%s'"
		//		,InputPath.c_str()
		//		);

		//	return false;
		//}

		return true;
	}
	////////////////////////////////////////////////////////////////////////////////////////////////////
	InputScriptFunctionMap * InputEngine::GetFunctionMap(const std::string &Name)
	{
		TMapInputScriptFunctionMap::iterator it_find =
			m_MapInputScriptFunctionMap.find(Name);

		if( it_find == m_MapInputScriptFunctionMap.end() )
		{
			return 0;
		}

		return it_find->second;
	}
	////////////////////////////////////////////////////////////////////////////////////////////////////
	bool InputEngine::SetActiveFunctionMap(const std::string &Name, bool Value)
	{
		TMapInputScriptFunctionMap::iterator it_find =
			m_MapInputScriptFunctionMap.find(Name);

		if( it_find == m_MapInputScriptFunctionMap.end() )
		{
			return false;
		}

		it_find->second->SetActive(Value);

		return true;
	}
	////////////////////////////////////////////////////////////////////////////////////////////////////
	bool InputEngine::IsKey(unsigned int index,int key_state)const
	{
		return m_interface->isKey(index,key_state);
	}
	////////////////////////////////////////////////////////////////////////////////////////////////////
	bool InputEngine::IsAnyKey()const
	{
		return m_interface->isAnyKey();
	}
	////////////////////////////////////////////////////////////////////////////////////////////////////
	bool InputEngine::GetKey(char *ch,int key_state)const
	{
		return m_interface->getKey(ch,key_state);
	}
	//////////////////////////////////////////////////////////////////////////
	//mouse
	////////////////////////////////////////////////////////////////////////////////////////////////////
	void InputEngine::SetPositionAndSpeed (int X, int Y, int Z, int Speed)
	{
		m_interface->setPositionAndSpeed (X,Y,Z,Speed);
	}
	////////////////////////////////////////////////////////////////////////////////////////////////////
	void InputEngine::SetRange (int min_x, int min_y, int min_z, int max_x, int max_y, int max_z)
	{
		m_interface->setRange(min_x,min_y,min_z,max_x,max_y,max_z);
	}
	////////////////////////////////////////////////////////////////////////////////////////////////////
	void InputEngine::SetPosition (int axis, int value)
	{
		m_interface->setPosition(axis,value);
	}
	////////////////////////////////////////////////////////////////////////////////////////////////////
	int InputEngine::GetPosition (int axis)const
	{
		return m_interface->getPosition(axis);
	}
	////////////////////////////////////////////////////////////////////////////////////////////////////
	int InputEngine::GetDelta	(int axis)const
	{
		return m_interface->getDelta(axis);
	}
	////////////////////////////////////////////////////////////////////////////////////////////////////
	bool InputEngine::IsMove()const
	{
		return m_interface->isMove();
	}
	////////////////////////////////////////////////////////////////////////////////////////////////////
	bool InputEngine::IsAnyButton()const
	{
		return m_interface->isAnyButton();
	}
	////////////////////////////////////////////////////////////////////////////////////////////////////
	bool InputEngine::IsButton(int but,int but_state)const
	{
		return m_interface->isButton(but,but_state);
	}
}