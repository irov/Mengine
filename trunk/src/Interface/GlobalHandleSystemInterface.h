#   pragma once

#	include "Interface/InputSystemInterface.h"

namespace Menge
{    
    class GlobalHandleSystemInterface
		: public InputSystemHandler
    {
    public:
		virtual uint32_t addGlobalHandler( InputSystemHandler * _handler ) = 0;
		virtual InputSystemHandler * removeGlobalHandler (uint32_t _id) = 0;

    public:
        virtual void update() = 0;
        virtual void clear() = 0;
    };
}