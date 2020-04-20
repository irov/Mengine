#pragma once

#include "Kernel/Factorable.h"

namespace Mengine
{
    class Entity;

    class Component
        : public Factorable
    {
    public:
        Component();
        ~Component() override;

    public:
        virtual uint32_t getComponentId() const = 0;

    public:
        virtual void activate( Entity * _entity ) = 0;
        virtual void deactivate( Entity * _entity ) = 0;

    public:
        virtual bool compile( Entity * _entity ) = 0;
        virtual void release( Entity * _entity ) = 0;
    
    };
    //////////////////////////////////////////////////////////////////////////
    typedef IntrusivePtr<Component> ComponentPtr;
    //////////////////////////////////////////////////////////////////////////
}
//////////////////////////////////////////////////////////////////////////
#define COMPONENT_DECLARE(T)\
        uint32_t getComponentId() const override\
        {\
            static uint32_t id = COMPONENT_SERVICE()->getComponentId<decltype(self)>();\
            return id;\
        }
//////////////////////////////////////////////////////////////////////////
