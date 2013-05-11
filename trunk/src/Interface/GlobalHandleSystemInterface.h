#   pragma once

#	include "Interface/InputSystemInterface.h"

namespace Menge
{
    class Arrow;

    class GlobalHandleSystemInterface
        : public GlobalMouseHandler
        , public GlobalKeyHandler
    {
    public:
        virtual size_t addGlobalMouseEventable( GlobalMouseHandler * _handler ) = 0;
        virtual GlobalMouseHandler * removeGlobalMouseEventable( size_t _id ) = 0;

        virtual size_t addGlobalKeyEventable( GlobalKeyHandler * _handler ) = 0;
        virtual GlobalKeyHandler * removeGlobalKeyEventable( size_t _id ) = 0;

    public:
        virtual void update() = 0;
        virtual void clear() = 0;
    };
}