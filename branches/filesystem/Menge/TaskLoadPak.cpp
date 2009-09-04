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
	TaskLoadPak::TaskLoadPak( const String &_pakName, const String& _pakPath, const String &_resourceXml, PyObject* _doneCallback )
		: m_pakName( _pakName )
		, m_pakPath( _pakPath )
		, m_resourceXml( _resourceXml )
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
		//bool exist = Holder<FileEngine>::hostage()
		//					->existFile( m_pakName );
		//if( exist == false )	// try to add .pak
		//{
		//	m_pakName += ".pak";
		//}
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
			->loadPak( m_pakName, m_pakPath, m_resourceXml );

		if( m_doneCallback != NULL
			&& m_doneCallback != Py_None )
		{
			pybind::call( m_doneCallback, "()" );
		}
	}
	//////////////////////////////////////////////////////////////////////////
}	// namespace Menge
