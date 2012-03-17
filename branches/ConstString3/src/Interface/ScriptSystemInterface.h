#	pragma once

#	include "ServiceInterface.h"

#	include "Config/Typedef.h"
#	include "Core/ConstString.h"
#	include <vector>

#	include <stdarg.h>

extern "C" 
{ 
	struct _object;
	typedef _object PyObject;
}

namespace Menge
{	
	class ScriptServiceInterface
		: public ServiceInterface
	{
	public:
		virtual void addModulePath( const TVectorString & _listPath ) = 0;
		virtual PyObject * importModule( const ConstString& _name ) = 0;
		virtual void setCurrentModule( PyObject * _module ) = 0;
		virtual PyObject * getModuleFunction( PyObject * _module, const char * _name ) = 0;
		virtual void callFunction( PyObject * _object, const char * _params, ... ) = 0;
		virtual void callFunction( PyObject * _object, const char * _params, va_list ) = 0;
	};
}