#	pragma once

#	include "Config/Typedef.h"

namespace Menge
{
	class ScriptLogger
	{
	public:
		ScriptLogger();

	public:
		virtual void write( const String& _msg );

	public:
		void setSoftspace( int _softspace );
		int getSoftspace() const;

	public:
		virtual PyObject * embedding();

	protected:
		int m_softspace;
	};

	class ErrorScriptLogger
		: public ScriptLogger
	{
	public:
		void write( const String& _msg ) override;

	public:
		PyObject * embedding() override;
	};
}