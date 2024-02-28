#pragma once

#include "Kernel/FactoryWithoutMutex.h"
#include "Kernel/FactorableUnique.h"
#include "Kernel/Typename.h"

namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    template<class Type>
    class FactoryDefault
        : public FactoryWithoutMutex
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
        template<class Type, class ... Args>
        FactoryInterfacePtr makeFactoryDefault( const DocumentInterfacePtr & _doc, Args && ... _args )
        {
            MENGINE_UNUSED( _doc );

            FactoryInterfacePtr factory = Helper::makeFactorableUnique<FactoryDefault<Type>>( _doc, std::forward<Args &&>( _args ) ... );

            MENGINE_ASSERTION_MEMORY_PANIC( factory );

            const ConstString & type = Type::getFactorableType();

            factory->setType( type );

            return factory;
        }
    }
    //////////////////////////////////////////////////////////////////////////
}
