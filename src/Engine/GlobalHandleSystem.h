#pragma once

#include "Interface/GlobalHandleSystemInterface.h"

#include "Core/ServiceBase.h"

#include "stdex/stl_vector.h"

namespace Mengine
{
	//////////////////////////////////////////////////////////////////////////
	struct GlobalHandlerDesc
	{
		uint32_t id;

		InputSystemHandler * handler;
        String doc;
		bool enable;
		bool dead;
	};
	//////////////////////////////////////////////////////////////////////////
	class GlobalHandleSystem
		: public ServiceBase<GlobalHandleSystemInterface>
	{
    public:
        GlobalHandleSystem();

	public:
		uint32_t addGlobalHandler( InputSystemHandler * _handler, const String & _doc ) override;
		InputSystemHandler * removeGlobalHandler( uint32_t _id ) override;

	public:
		bool enableGlobalHandler( uint32_t _id, bool _value ) override;

	public:
		void update() override;
        void clear() override;

	public:
		bool handleKeyEvent( const InputKeyEvent & _event ) override;
		bool handleTextEvent( const InputTextEvent & _event ) override;

	public:
		bool handleMouseButtonEvent( const InputMouseButtonEvent & _event ) override;
		bool handleMouseButtonEventBegin( const InputMouseButtonEvent & _event ) override;
		bool handleMouseButtonEventEnd( const InputMouseButtonEvent & _event ) override;
		bool handleMouseMove( const InputMouseMoveEvent & _event ) override;
		bool handleMouseWheel( const InputMouseWheelEvent & _event ) override;

	protected:      		
		typedef stdex::vector<GlobalHandlerDesc> TVectorGlobalHandler;
		TVectorGlobalHandler m_handlers;
		TVectorGlobalHandler m_handlersAdd;

        uint32_t m_handlersEnumerator;
	};
}
