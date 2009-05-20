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
	TaskLoadPak::TaskLoadPak( const String &_pakName, const String &_resourceXml, PyObject* _doneCallback )
		: m_pakName( _pakName )
		, m_resourceXml( _resourceXml )
		, m_doneCallback( _doneCallback )
	{
		pybind::incref( m_doneCallback );

		this->setName( _pakName );
	}
	//////////////////////////////////////////////////////////////////////////
	TaskLoadPak::~TaskLoadPak()
	{
		pybind::decref( m_doneCallback );
	}
	//////////////////////////////////////////////////////////////////////////
	void TaskLoadPak::preMain()
	{
		bool exist = Holder<FileEngine>::hostage()
							->existFile( m_pakName );
		if( exist == false )	// try to add .pak
		{
			m_pakName += ".pak";
			/*exist = Holder<FileEngine>::hostage()
					->existFile( m_pakName );
			if( exist == false )
			{

			}*/
		}
	}
	//////////////////////////////////////////////////////////////////////////
	void TaskLoadPak::main()
	{
		Holder<FileEngine>::hostage()
			->loadPak( m_pakName );
		m_complete = true;
	}
	//////////////////////////////////////////////////////////////////////////
	void TaskLoadPak::postMain()
	{
		Holder<Game>::hostage()
			->loadPak( m_pakName + "/" + m_resourceXml );

		if( m_doneCallback != NULL
			&& m_doneCallback != Py_None )
		{
			pybind::call( m_doneCallback, "()" );
		}
	}
	//////////////////////////////////////////////////////////////////////////
}	// namespace Menge
