#	pragma once

#	include "Interface/GlobalHandleSystemInterface.h"

#	include <stdex/stl_vector.h>

namespace Menge
{
	//////////////////////////////////////////////////////////////////////////
	struct GlobalHandlerDesc
	{
		uint32_t id;

		InputSystemHandler * handler;
		bool dead;
	};
	//////////////////////////////////////////////////////////////////////////
	class GlobalHandleSystem
		: public GlobalHandleSystemInterface
	{
    public:
        GlobalHandleSystem( ServiceProviderInterface * _serviceProvider );

	public:
		uint32_t addGlobalHandler( InputSystemHandler * _handler ) override;
		InputSystemHandler * removeGlobalHandler( uint32_t _id ) override;

	public:
		void update() override;
        void clear() override;

	public:
		bool handleKeyEvent( const InputKeyEvent & _event ) override;

	public:
		bool handleMouseButtonEvent( const InputMouseButtonEvent & _event ) override;
		bool handleMouseButtonEventBegin( const InputMouseButtonEvent & _event ) override;
		bool handleMouseButtonEventEnd( const InputMouseButtonEvent & _event ) override;
		bool handleMouseMove( const InputMouseMoveEvent & _event ) override;
		bool handleMouseWheel( const InputMouseWheelEvent & _event ) override;

	protected:
        ServiceProviderInterface * m_serviceProvider;
        		
		typedef stdex::vector<GlobalHandlerDesc> TVectorGlobalHandler;
		TVectorGlobalHandler m_handlers;
		TVectorGlobalHandler m_handlersAdd;

        uint32_t m_handlersEnumerator;
	};
}
