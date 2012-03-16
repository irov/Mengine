#	pragma once

#	include "ServiceInterface.h"

#	include "Config/Typedef.h"
#	include "Core/ConstString.h"
#	include <vector>

extern "C" 
{ 
	struct _object;
	typedef _object PyObject;
}

namespace Menge
{
	enum EScriptMethodArgument
	{
		ESMA_BOOL,
		ESMA_INT,
		ESMA_UINT,
		ESMA_FLOAT,
		ESMA_STRING,
	};
	
	class ScriptMethodArgumentInterface
	{
	public:
		virtual void * data() = 0;
		virtual EScriptMethodArgument type() = 0;
	};
	
	typedef std::vector<ScriptMethodArgumentInterface *> TVectorScriptMethodArguments;

	class ScriptMethodInterface
	{
	public:
		virtual void call( const TVectorScriptMethodArguments & _args ) = 0;
	};
		
	class ScriptServiceInterface
		: public ServiceInterface
	{
	public:
		typedef std::list<String> TListModulePath;
		virtual void registerMethod( const String & _name, ScriptMethodInterface * _method ) = 0;
		virtual void addModulePath( const TListModulePath& _listPath ) = 0;
		virtual PyObject * importModule( const ConstString& _name ) = 0;
		virtual void setCurrentModule( PyObject * _module ) = 0;
		virtual PyObject * getModuleFunction( PyObject * _module, const char * _name ) = 0;
		virtual void callFunction( PyObject * _object, const char * _params, ... ) = 0;
		virtual void callFunction( PyObject * _object, const char * _params, va_list ) = 0;
	};
}