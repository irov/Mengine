#	pragma once

#	include "Interface/COMInterface.h"

#	include "Factory/FactorablePtr.h"

#	include "Kernel/Identity.h"
#	include "Kernel/Updatable.h"
#	include "Kernel/Materialable.h"
#	include "Kernel/Eventable.h"
#	include "Kernel/Scriptable.h"
#	include "Kernel/Animatable.h"
#	include "Kernel/Comable.h"

namespace Menge
{
	class Surface
		: public FactorablePtr
		, public Identity
		, public Updatable
		, public Materialable
		, public Eventable
		, public Scriptable
		, public Animatable
		, public Comable
	{
	public:
		Surface();
		~Surface();

	public:
		void setServiceProvider( ServiceProviderInterface * _serviceProvider );
		ServiceProviderInterface * getServiceProvider() const;

	public:
		PyObject * setEventListener( PyObject * _args, PyObject * _kwds );
		void removeEventListener();

	protected:
		virtual void _setEventListener( const pybind::dict & _listener );

	protected:
		ServiceProviderInterface * m_serviceProvider;
	};
	//////////////////////////////////////////////////////////////////////////
	typedef stdex::intrusive_ptr<Surface> SurfacePtr;
}