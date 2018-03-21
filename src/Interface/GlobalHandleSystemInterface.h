#   pragma once

#include "Interface/InputSystemInterface.h"

namespace Mengine
{    
    class GlobalHandleSystemInterface
        : public ServiceInterface
		, public InputSystemHandler
    {
        SERVICE_DECLARE( "GlobalHandleSystem" )

    public:
		virtual uint32_t addGlobalHandler( InputSystemHandler * _handler, const String & _doc ) = 0;
		virtual InputSystemHandler * removeGlobalHandler (uint32_t _id) = 0;

	public:
		virtual bool enableGlobalHandler( uint32_t _id, bool _value ) = 0;

    public:
        virtual void update() = 0;
        virtual void clear() = 0;
    };
}