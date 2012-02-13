///*
// *	TaskLoadPak.cpp
// *
// *	Created by _Berserk_ on 8.5.2009
// *	Copyright 2009 Menge. All rights reserved.
// *
// */
//
//#	include "TaskLoadPak.h"
//#	include "FileEngine.h"
//#	include "Game.h"
//#	include "ScriptEngine.h"
//
//namespace Menge
//{
//	//////////////////////////////////////////////////////////////////////////
//	TaskLoadPak::TaskLoadPak( const ConstString & _pakName, const ConstString & _pakType, PyObject* _doneCallback )
//		: m_pakName(_pakName)
//		, m_pakType(_pakType)
//		, m_doneCallback(_doneCallback)
//	{
//		pybind::incref( m_doneCallback );
//	}
//	//////////////////////////////////////////////////////////////////////////
//	TaskLoadPak::~TaskLoadPak()
//	{
//		pybind::decref( m_doneCallback );
//	}
//	//////////////////////////////////////////////////////////////////////////
//	void TaskLoadPak::preMain()
//	{
//		m_pakPath = Game::get()
//			->getPakPath( m_pakName );
//	}
//	//////////////////////////////////////////////////////////////////////////
//	void TaskLoadPak::main()
//	{
//		FileEngine::get()
//			->mountFileSystem( m_pakName, m_pakPath, m_pakType, false );
//
//		m_complete = true;
//	}
//	//////////////////////////////////////////////////////////////////////////
//	void TaskLoadPak::postMain()
//	{
//		Game::get()
//			->loadPakFromName( m_pakName );
//
//		if( m_doneCallback != NULL
//			&& pybind::is_none(m_doneCallback) == false )
//		{
//			pybind::call( m_doneCallback, "()" );
//		}
//	}
//	//////////////////////////////////////////////////////////////////////////
//}	// namespace Menge
