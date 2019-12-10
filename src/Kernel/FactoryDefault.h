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
            Type * ptr = Helper::allocateT<Type>();

            return ptr;
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
        FactoryPtr makeFactoryDefault( const DocumentPtr & _doc )
        {
            Factory * factory = new FactoryDefault<T>();

#ifdef MENGINE_DEBUG
            if( factory != nullptr )
            {
                factory->setDocument( _doc );
            }
#endif

            return FactoryPtr( factory );
        }
    }
}
