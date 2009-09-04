/*
 *	TaskLoadPak.h
 *
 *	Created by _Berserk_ on 8.5.2009
 *	Copyright 2009 Menge. All rights reserved.
 *
 */

#	pragma once

#	include "Config/Typedef.h"
#	include "Task.h"

extern "C"
{
	struct _object; 
	typedef _object PyObject;
}

namespace Menge
{
	class TaskLoadPak
		: public Task
	{
	public:
		TaskLoadPak( const String& _pakName, const String& _pakPath, const String& _resourceXml, PyObject* _doneCallback );
		~TaskLoadPak();

	public:
		void preMain() override;
		void main() override;
		void postMain() override;

	protected:
		String m_pakName;
		String m_pakPath;
		String m_resourceXml;
		PyObject* m_doneCallback;
	};
}	// namespace Menge
