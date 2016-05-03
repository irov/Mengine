#	pragma once

#	include "Interface/LoggerInterface.h"

#	include "Config/Typedef.h"

#	include "pybind/types.hpp"

namespace Menge
{
	class ServiceProviderInterface;

	class XlsScriptLogger
	{
	public:
		XlsScriptLogger( ServiceProviderInterface * _serviceProvider, EMessageLevel _level );
        virtual ~XlsScriptLogger();

	public:
		virtual void write( const char * _msg, size_t _size );        

	public:
		PyObject * py_write( PyObject * _args, PyObject * _kwds );
        PyObject * py_flush( PyObject * _args, PyObject * _kwds );
		
	public:
		void setSoftspace( int _softspace );
		int getSoftspace() const;

	public:
		virtual PyObject * embedding( PyObject * _module );

	protected:
		ServiceProviderInterface * m_serviceProvider;

		EMessageLevel m_level;

		int m_softspace;
	};
}