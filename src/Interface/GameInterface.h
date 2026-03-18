#pragma once

#include "Kernel/Mixin.h"

#include "Kernel/ConstString.h"

namespace Mengine
{
    class GameInterface
        : public Mixin
    {
    public:
        virtual void setName( const ConstString & _name ) = 0;
        virtual const ConstString & getName() const = 0;

    public:
        virtual bool initializeGame() = 0;
        virtual void finalizeGame() = 0;

    public:
        virtual bool isAvailableGame() const = 0;
    };

    typedef IntrusivePtr<GameInterface> GameInterfacePtr;
}