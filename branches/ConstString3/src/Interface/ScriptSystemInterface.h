#	pragma once

#	include "ServiceInterface.h"

#	include "Config/Typedef.h"

#	include <vector>

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
		virtual void registerMethod( const String & _name, ScriptMethodInterface * _method ) = 0;
	};
}