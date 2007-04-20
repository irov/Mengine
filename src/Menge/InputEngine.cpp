// ***************************************************************
//  InputEngine.cpp   version:  1.0   �  date: 03/21/2006
//  -------------------------------------------------------------
//  IROV
//  -------------------------------------------------------------
//  Copyright (C) 2006 - All Rights Reserved
// ***************************************************************
// 
// ***************************************************************

#	include "InputEngine.h"

#	include "ScriptEngine.h"

namespace Menge
{
	////////////////////////////////////////////////////////////////////////////////////////////////////
	InputEngine::InputEngine(const std::string &_dllModule)
		: SystemDLL<InputSystemInterface>(_dllModule)
	{
		Holder<InputEngine>::keep(this);
	}
	////////////////////////////////////////////////////////////////////////////////////////////////////
	InputEngine::~InputEngine()
	{
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
	//////////////////////////////////////////////////////////////////////////
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