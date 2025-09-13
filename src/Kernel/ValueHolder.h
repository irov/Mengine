#pragma once

#include "Interface/DocumentInterface.h"

#include "Kernel/Factorable.h"
#include "Kernel/AssertionMemoryPanic.h"

namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    template<class Type>
    class ValueHolder
        : public Factorable
    {
        DECLARE_FACTORABLE( ValueHolder );

    public:
        ValueHolder()
        {
        }

        ValueHolder( const Type & _value )
            : m_value( _value )
        {
        }

        ValueHolder( Type && _value )
            : m_value( std::forward<Type>( _value ) )
        {
        }

        ~ValueHolder() override
        {
        }

    public:
        void setValue( const Type & _value )
        {
            m_value = _value;
        }

        void setValue( Type && _value )
        {
            m_value = std::forward<Type>( _value );
        }

        const Type & getValue() const
        {
            return m_value;
        }

        Type & getValue()
        {
            return m_value;
        }

    protected:
        void destroy() override
        {
            Helper::deleteT( this );
        }

    protected:
        Type m_value;
    };
    //////////////////////////////////////////////////////////////////////////
    template<class Type>
    using ValueHolderPtr = IntrusivePtr<ValueHolder<Type>>;
    //////////////////////////////////////////////////////////////////////////
    namespace Helper
    {
        template<class Type>
        ValueHolderPtr<Type> makeValueHolder( const DocumentInterfacePtr & _doc )
        {
            MENGINE_UNUSED( _doc );

            ValueHolder<Type> * factorable = Helper::newT<ValueHolder<Type>>();

            MENGINE_ASSERTION_MEMORY_PANIC( factorable );

#if defined(MENGINE_DOCUMENT_ENABLE)
            factorable->setDocument( _doc );
#endif

            return ValueHolderPtr<Type>::from( factorable );
        }
    }
    //////////////////////////////////////////////////////////////////////////
}