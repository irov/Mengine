#   pragma once

#	include "Interface/InputSystemInterface.h"

namespace Menge
{
    class Arrow;

    class GlobalHandleSystemInterface
        : public InputSystemHandler
    {
    public:
        virtual void regGlobalMouseEventable( GlobalMouseHandler * _handler ) = 0;
        virtual void unregGlobalMouseEventable( GlobalMouseHandler * _handler ) = 0;

        virtual void regGlobalKeyEventable( GlobalKeyHandler * _handler ) = 0;
        virtual void unregGlobalKeyEventable( GlobalKeyHandler * _handler ) = 0;

    public:
        virtual void update() = 0;
        virtual void clear() = 0;
    };
}