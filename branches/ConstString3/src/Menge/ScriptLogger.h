#	pragma once

#	include "Config/Typedef.h"

#	include "pybind/types.hpp"

namespace Menge
{
	class LogSystemInterface;

	class ScriptLogger
	{
	public:
		ScriptLogger( LogSystemInterface * _logSystem );

	public:
		virtual void write( const String& _msg );

	public:
		void setSoftspace( int _softspace );
		int getSoftspace() const;

	public:
		virtual PyObject * embedding();

	protected:
		LogSystemInterface * m_logSystem;

		int m_softspace;
	};

	class ErrorScriptLogger
		: public ScriptLogger
	{
	public:
		ErrorScriptLogger( LogSystemInterface * _logSystem );

	public:
		void write( const String& _msg ) override;

	public:
		PyObject * embedding() override;
	};
}