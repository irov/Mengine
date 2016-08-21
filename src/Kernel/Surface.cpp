#	include "Surface.h"

#	include "Logger/Logger.h"

namespace Menge
{
	//////////////////////////////////////////////////////////////////////////
	Surface::Surface()
		: m_serviceProvider(nullptr)
	{
	}
	//////////////////////////////////////////////////////////////////////////
	Surface::~Surface()
	{
	}
	//////////////////////////////////////////////////////////////////////////
	void Surface::setServiceProvider( ServiceProviderInterface * _serviceProvider )
	{
		m_serviceProvider = _serviceProvider;
	}
	//////////////////////////////////////////////////////////////////////////
	ServiceProviderInterface * Surface::getServiceProvider() const
	{
		return m_serviceProvider;
	}
	//////////////////////////////////////////////////////////////////////////
	PyObject * Surface::setEventListener( PyObject * _args, PyObject * _kwds )
	{
		(void)_args;

		if( _kwds == nullptr )
		{
			LOGGER_ERROR( m_serviceProvider )("Surface::setEventListener %s wait kwds"
				, m_name.c_str()
				);

			return pybind::ret_none();
		}

		pybind::dict py_kwds( _kwds );
		this->_setEventListener( py_kwds );

		PyObject * py_none = pybind::ret_none();

		return py_none;
	}
	//////////////////////////////////////////////////////////////////////////
	void Surface::removeEventListener()
	{
		pybind::dict py_invalid = pybind::make_invalid_dict_t();
		this->_setEventListener( py_invalid );
	}
	//////////////////////////////////////////////////////////////////////////
	void Surface::_setEventListener( const pybind::dict & _listener )
	{
		(void)_listener;
	}
}