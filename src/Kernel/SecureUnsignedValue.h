#pragma once

#include "Kernel/Factorable.h"

#include "Config/Typedef.h"
#include "Config/Char.h"

namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    typedef IntrusivePtr<class SecureUnsignedValue> SecureUnsignedValuePtr;
    //////////////////////////////////////////////////////////////////////////
    class SecureUnsignedValue
        : public Factorable
    {
        DECLARE_FACTORABLE( SecureUnsignedValue );

    public:
        SecureUnsignedValue();
        ~SecureUnsignedValue() override;

    public:
        void setUnprotectedValue( uint32_t _value );
        bool getUnprotectedValue( uint32_t * const _value ) const;

    public:
        bool setupSecureValue( const SecureUnsignedValuePtr & _value );
        bool additiveSecureValue( const SecureUnsignedValuePtr & _add );
        bool substractSecureValue( const SecureUnsignedValuePtr & _sub );

        bool additive2SecureValue( const SecureUnsignedValuePtr & _add, const SecureUnsignedValuePtr & _pow );

        bool cmpSecureValue( const SecureUnsignedValuePtr & _value, int32_t * const _result ) const;

    public:
        uint32_t m_value;
        uint32_t m_hash;
        Char m_buffer[20] = {'\0'};
    };
    //////////////////////////////////////////////////////////////////////////
    typedef IntrusivePtr<SecureUnsignedValue> SecureUnsignedValuePtr;
    //////////////////////////////////////////////////////////////////////////
}