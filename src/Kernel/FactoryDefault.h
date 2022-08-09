#pragma once

#include "Kernel/Factory.h"
#include "Kernel/FactorableUnique.h"
#include "Kernel/Typename.h"

namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    template<class Type>
    class FactoryDefault
        : public Factory
    {
    public:
        FactoryDefault()
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
            Helper::deleteT( static_cast<Type *>(_obj) );
        }
    };
    //////////////////////////////////////////////////////////////////////////
    namespace Helper
    {
        template<class Type>
        FactoryInterfacePtr makeFactoryDefault( const DocumentPtr & _doc )
        {
            MENGINE_UNUSED( _doc );

            FactoryPtr factory = Helper::makeFactorableUnique<FactoryDefault<Type>>( _doc );

            MENGINE_ASSERTION_MEMORY_PANIC( factory );

            const ConstString & type = Type::getFactorableType();

            factory->initialize( type );

            return factory;
        }
    }
    //////////////////////////////////////////////////////////////////////////
}
