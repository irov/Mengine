#pragma once

#include "Kernel/Factory.h"
#include "Kernel/FactorableUnique.h"
#include "Kernel/Typename.h"

namespace Mengine
{
    template<class Type>
    class FactoryDefault
        : public Factory
    {
    public:
        FactoryDefault()
            : Factory( Typename<Type>::value )
        {
        }

        ~FactoryDefault() override
        {
        }

    protected:
        Factorable * _createObject() override
        {
            Type * ptr = Helper::newT<Type>();

            return ptr;
        }

        void _destroyObject( Factorable * _obj ) override
        {
            Helper::deleteT<Type>( static_cast<Type *>(_obj) );
        }
    };
    //////////////////////////////////////////////////////////////////////////
    namespace Helper
    {
        template<class T>
        FactoryPtr makeFactoryDefault( const DocumentPtr & _doc )
        {
            MENGINE_UNUSED( _doc );

            Factory * factory = Helper::newT<FactoryDefault<T>>();

            MENGINE_ASSERTION_MEMORY_PANIC( factory );

#ifdef MENGINE_DEBUG
            factory->setDocument( _doc );
#endif

            return FactoryPtr( factory );
        }
    }
}
