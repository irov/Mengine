#pragma once

#include "Kernel/Factorable.h"
#include "Kernel/String.h"

#include "Config/Char.h"

namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    typedef IntrusivePtr<class SecureStringValue> SecureStringValuePtr;
    //////////////////////////////////////////////////////////////////////////
    class SecureStringValue
        : public Factorable
    {
        DECLARE_FACTORABLE( SecureStringValue );

    public:
        SecureStringValue();
        ~SecureStringValue() override;

    public:
        void setUnprotectedValue( const String & _value );
        bool getUnprotectedValue( String * const _value ) const;

    public:
        bool cmpSecureValue( const SecureStringValuePtr & _value, int32_t * const _result ) const;

    public:
        String m_value;
        uint32_t m_hash;
        String m_buffer;
    };
    //////////////////////////////////////////////////////////////////////////
    typedef IntrusivePtr<SecureStringValue> SecureStringValuePtr;
    //////////////////////////////////////////////////////////////////////////
}