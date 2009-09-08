/*
 *	TaskLoadPak.cpp
 *
 *	Created by _Berserk_ on 8.5.2009
 *	Copyright 2009 Menge. All rights reserved.
 *
 */

#	include "TaskLoadPak.h"
#	include "FileEngine.h"
#	include "Game.h"
#	include "ScriptEngine.h"

namespace Menge
{
	//////////////////////////////////////////////////////////////////////////
	TaskLoadPak::TaskLoadPak( const String &_pakName, PyObject* _doneCallback )
		: m_pakName( _pakName )
		, m_doneCallback( _doneCallback )
	{
		pybind::incref( m_doneCallback );
	}
	//////////////////////////////////////////////////////////////////////////
	TaskLoadPak::~TaskLoadPak()
	{
		pybind::decref( m_doneCallback );
	}
	//////////////////////////////////////////////////////////////////////////
	void TaskLoadPak::preMain()
	{
		m_pakPath = Holder<Game>::hostage()
						->getPakPath( m_pakName );
	}
	//////////////////////////////////////////////////////////////////////////
	void TaskLoadPak::main()
	{
		//Holder<FileEngine>::hostage()
		//	->loadPak( m_pakName );
		Holder<FileEngine>::hostage()
			->mountFileSystem( m_pakName, m_pakPath, false );
		m_complete = true;
	}
	//////////////////////////////////////////////////////////////////////////
	void TaskLoadPak::postMain()
	{
		Holder<Game>::hostage()
			->loadPakFromName( m_pakName );

		if( m_doneCallback != NULL
			&& m_doneCallback != Py_None )
		{
			pybind::call( m_doneCallback, "()" );
		}
	}
	//////////////////////////////////////////////////////////////////////////
}	// namespace Menge
