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

#	include "XmlParser.h"
#	include "ErrorMessage.h"

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
	bool InputEngine::init()
	{
		HWND hWnd = GetActiveWindow();

		return m_interface->init( hWnd, 0x00000003 );
	}
	////////////////////////////////////////////////////////////////////////////////////////////////////
	void InputEngine::update()
	{
		m_interface->update();
	}
	////////////////////////////////////////////////////////////////////////////////////////////////////
	bool InputEngine::loadFunctionMap(const std::string &File)
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
	InputScriptFunctionMap * InputEngine::getFunctionMap(const std::string &Name)
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
	bool InputEngine::setActiveFunctionMap(const std::string &Name, bool Value)
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
	bool InputEngine::isKey(unsigned int index,int key_state)const
	{
		return m_interface->isKey(index,key_state);
	}
	////////////////////////////////////////////////////////////////////////////////////////////////////
	bool InputEngine::isAnyKeyDown()const
	{
		return m_interface->isAnyKeyDown();
	}
	////////////////////////////////////////////////////////////////////////////////////////////////////
	bool InputEngine::getChar(char *ch,int key_state)const
	{
		return m_interface->getChar(ch,key_state);
	}
	//////////////////////////////////////////////////////////////////////////
	//mouse
	void InputEngine::setPosition( float _x, float _y, float _whell )
	{
		m_interface->setPosition( _x, _y, _whell);
	}
	//////////////////////////////////////////////////////////////////////////
	void InputEngine::setSensitivity( float _sensitivity )
	{
		m_interface->setSensitivity( _sensitivity );
	}
	//////////////////////////////////////////////////////////////////////////
	void InputEngine::setRange( const mt::vec3f &_minRange, const mt::vec3f & _maxRange )
	{
		m_interface->setRange( _minRange.m, _maxRange.m );
	}
	//////////////////////////////////////////////////////////////////////////
	const mt::vec3f & InputEngine::getPosition() const
	{
		return *(const mt::vec3f *)m_interface->getPosition();
	}
	const mt::vec3f & InputEngine::getDelta() const
	{
		return *(const mt::vec3f *)m_interface->getDelta();
	}
	////////////////////////////////////////////////////////////////////////////////////////////////////
	bool InputEngine::isMove()const
	{
		return m_interface->isMove();
	}
	////////////////////////////////////////////////////////////////////////////////////////////////////
	bool InputEngine::isAnyButtonDown()const
	{
		return m_interface->isAnyButtonDown();
	}
	////////////////////////////////////////////////////////////////////////////////////////////////////
	bool InputEngine::isButton(int but,int but_state)const
	{
		return m_interface->isButton(but,but_state);
	}
}