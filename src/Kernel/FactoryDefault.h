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

    protected:
        Factorable * _createObject() override
        {
            Type * t = Helper::allocateT<Type>();

            return t;
        }

        void _destroyObject( Factorable * _obj ) override
        {
            Helper::freeT<Type>( static_cast<Type *>(_obj) );
        }
    };
    //////////////////////////////////////////////////////////////////////////
    namespace Helper
    {
        template<class T>
        FactoryPtr makeFactoryDefault()
        {
            return FactoryPtr( new FactoryDefault<T>() );
        }
    }
}
