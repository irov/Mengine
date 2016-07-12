#	pragma once

#   include "Interface/ServiceInterface.h"

#   include "Factory/FactorablePtr.h"

namespace Menge
{
	//////////////////////////////////////////////////////////////////////////
	class SocketInterface
		: public FactorablePtr
	{
	public:
		virtual bool initialize( const Char * _ip, const Char * _port ) = 0;
		virtual void finalize() = 0;

	public:
		virtual bool send( const void * _buffer, size_t _len ) = 0;
		virtual bool receive( void * _buffer, size_t _capacity, size_t & _receiv ) = 0;
	};
	//////////////////////////////////////////////////////////////////////////
	typedef stdex::intrusive_ptr<SocketInterface> SocketInterfacePtr;
	//////////////////////////////////////////////////////////////////////////
	class SocketSystemInterface
		: public ServiceInterface
	{
		SERVICE_DECLARE( "SocketSystem" )

	public:
		virtual SocketInterfacePtr createSocket() = 0;
	};
	//////////////////////////////////////////////////////////////////////////
#   define TIMELINE_SERVICE( serviceProvider )\
    ((Menge::TimelineServiceInterface*)SERVICE_GET(serviceProvider, Menge::TimelineServiceInterface))
}