#	pragma once

#	include "Config/Typedef.h"

#	include "pybind/types.hpp"

namespace Menge
{
	class ServiceProviderInterface;

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
		void setSoftspace( int _softspace );
		int getSoftspace() const;

	public:
		virtual PyObject * embedding();

	protected:
		ServiceProviderInterface * m_serviceProvider;

		int m_softspace;
	};

	class ScriptLoggerError
		: public ScriptLogger
	{
	public:
		ScriptLoggerError( ServiceProviderInterface * _serviceProvider );

	public:
		void write( const char * _msg, size_t _size ) override;
		
	public:
		PyObject * embedding() override;
	};
}