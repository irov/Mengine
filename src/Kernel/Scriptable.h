#	pragma once

#   ifdef MENGINE_SCRIPTABLE
#	include "pybind/bindable.hpp"
#   endif

namespace Menge
{
	class ScriptWrapperInterface;

	class Scriptable
#   ifdef MENGINE_SCRIPTABLE 
		: public pybind::bindable
#   endif
	{	
	public:
		Scriptable();
		~Scriptable();

	public:
		void setScriptWrapper( ScriptWrapperInterface * _scriptWrapper );		
		ScriptWrapperInterface * getScriptWrapper() const;
        	
#   ifdef MENGINE_SCRIPTABLE
    protected:
		PyObject * _embedded() override;
#   else
        virtual void setEmbed(void * _dummy);
#   endif

	protected:
		ScriptWrapperInterface * m_scriptWrapper;
	};
}
