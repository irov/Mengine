#pragma once

#include "pybind/bindable.hpp"

#include "stdex/intrusive_ptr.h"

namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    typedef stdex::intrusive_ptr<class ScriptWrapperInterface> ScriptWrapperInterfacePtr;
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
