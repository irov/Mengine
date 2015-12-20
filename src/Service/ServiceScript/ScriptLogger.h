#	pragma once

#	include "Interface/LoggerInterface.h"

#	include "pybind/types.hpp"

namespace Menge
{
	class ScriptLogger
	{
	public:
		ScriptLogger( ServiceProviderInterface * _serviceProvider );
        virtual ~ScriptLogger();

	public:
		virtual void write( const char * _msg, size_t _size );

	public:
		PyObject * py_write( PyObject * _args, PyObject * _kwds );
		
	public:
		void setMessageLevel( EMessageLevel _level );
		EMessageLevel getMessageLevel() const;

		void setSoftspace( int _softspace );
		int getSoftspace() const;

	protected:
		ServiceProviderInterface * m_serviceProvider;

		EMessageLevel m_level;

		int m_softspace;
	};
}