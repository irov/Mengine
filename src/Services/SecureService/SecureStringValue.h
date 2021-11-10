#pragma once

#include "Interface/SecureValueInterface.h"

#include "Config/Char.h"

namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    typedef IntrusivePtr<class SecureStringValue> SecureStringValuePtr;
    //////////////////////////////////////////////////////////////////////////
    class SecureStringValue
        : public SecureValueInterface
    {
        DECLARE_FACTORABLE( SecureStringValue );

    public:
        SecureStringValue();
        ~SecureStringValue() override;

    public:
        void setUnprotectedValue( const String & _value );
        bool getUnprotectedValue( String * const _value ) const;

    public:
        bool setupSecureValue( const SecureStringValuePtr & _setup );

    public:
        bool cmpSecureValue( const SecureStringValuePtr & _value, int32_t * const _result ) const;

    public:
        void saveHexadecimal( String * const _hexadecimal ) const override;
        void loadHexadecimal( const String & _hexadecimal ) override;

    public:
        String m_value;
        uint32_t m_hash;
        String m_buffer;
    };
    //////////////////////////////////////////////////////////////////////////
    typedef IntrusivePtr<SecureStringValue> SecureStringValuePtr;
    //////////////////////////////////////////////////////////////////////////
}