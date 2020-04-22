#pragma once

#include "Kernel/Factorable.h"

#include "Config/Typedef.h"
#include "Config/Char.h"

namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    typedef IntrusivePtr<class SecureValue> SecureValuePtr;
    //////////////////////////////////////////////////////////////////////////
    class SecureValue
        : public Factorable
    {
    public:
        SecureValue();
        ~SecureValue() override;

    public:
        void setUnprotectedValue( uint32_t _value );
        bool getUnprotectedValue( uint32_t * _value ) const;

    public:
        bool setupSecureValue( const SecureValuePtr & _value );
        bool additiveSecureValue( const SecureValuePtr & _add );
        bool substractSecureValue( const SecureValuePtr & _sub );

        bool additive2SecureValue( const SecureValuePtr & _add, const SecureValuePtr & _pow );

        bool cmpSecureValue( const SecureValuePtr & _value, int32_t * _result ) const;

    public:
        uint32_t m_value;
        uint32_t m_hash;
        Char m_buffer[20];
    };
    //////////////////////////////////////////////////////////////////////////
    typedef IntrusivePtr<SecureValue> SecureValuePtr;
    //////////////////////////////////////////////////////////////////////////
}