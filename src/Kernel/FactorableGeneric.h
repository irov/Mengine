#pragma once

#include "Kernel/Factorable.h"
#include "Kernel/IntrusivePtr.h"

namespace Mengine
{
    template<class T>
    class FactorableGeneric
        : public Factorable
    {
    public:
        FactorableGeneric( T _value )
            : m_value( _value )
        {
        }

        ~FactorableGeneric() override
        {
        }

    public:
        void setValue( T _value )
        {
            m_value = _value;
        }

        T getValue() const
        {
            return m_value;
        }

    protected:
        T m_value;
    };
    //////////////////////////////////////////////////////////////////////////
    namespace Helper
    {
        template<class Type>
        IntrusivePtr<FactorableGeneric<Type>> makeFactorableGeneric( const DocumentInterfacePtr & _doc, Type _value )
        {
            MENGINE_UNUSED( _doc );

            FactorableGeneric<Type> * factorable = Helper::newT<FactorableUnique<FactorableGeneric<Type>>>( _value );

            MENGINE_ASSERTION_MEMORY_PANIC( factorable );

#if defined(MENGINE_DOCUMENT_ENABLE)
            factorable->setDocument( _doc );
#endif

            return IntrusivePtr<FactorableGeneric<Type>>( factorable );
        }
    }
    //////////////////////////////////////////////////////////////////////////
}