#	pragma once

#	include "Config/Typedef.h"

#	include "pybind/types.hpp"

namespace Menge
{
	class LogServiceInterface;

	class ScriptLogger
	{
	public:
		ScriptLogger( LogServiceInterface * _logService );

	public:
		virtual void write( const String& _msg );

	public:
		PyObject * py_write( PyObject * _args, PyObject * _kwds );
		
	public:
		void setSoftspace( int _softspace );
		int getSoftspace() const;

	public:
		virtual PyObject * embedding();

	protected:
		LogServiceInterface * m_logService;

		int m_softspace;
	};

	class ErrorScriptLogger
		: public ScriptLogger
	{
	public:
		ErrorScriptLogger( LogServiceInterface * _logService );

	public:
		void write( const String& _msg ) override;
		
	public:
		PyObject * embedding() override;
	};
}