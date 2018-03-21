#pragma once

#include "Interface/LoggerInterface.h"

#include "Config/Typedef.h"

#include "pybind/types.hpp"
#include "pybind/kernel.hpp"

namespace Mengine
{
	class XlsScriptLogger
	{
	public:
		XlsScriptLogger( EMessageLevel _level );
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
		virtual PyObject * embedding( pybind::kernel_interface * _kernel, PyObject * _module );

	protected:
		EMessageLevel m_level;

		int m_softspace;
	};
}