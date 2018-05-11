#pragma once

#include "Core/IntrusivePtr.h"

#include "pybind/bindable.hpp"

namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    typedef IntrusivePtr<class ScriptWrapperInterface> ScriptWrapperInterfacePtr;
    //////////////////////////////////////////////////////////////////////////
	class Scriptable
		: public pybind::bindable
	{	
	public:
		Scriptable();
		~Scriptable();

	public:
		void setScriptWrapper( const ScriptWrapperInterfacePtr & _scriptWrapper );		
        const ScriptWrapperInterfacePtr & getScriptWrapper() const;
        	
    protected:
		PyObject * _embedded() override;

	protected:
		ScriptWrapperInterfacePtr m_scriptWrapper;
	};
}
