#pragma once

#include "Interface/OptionInterface.h"

#include "Kernel/StaticString.h"

#ifndef MENGINE_OPTIONS_KEY_SIZE
#define MENGINE_OPTIONS_KEY_SIZE 128
#endif

#ifndef MENGINE_OPTIONS_VALUE_SIZE
#define MENGINE_OPTIONS_VALUE_SIZE 128
#endif

namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    class Option
        : public OptionInterface
    {
        DECLARE_FACTORABLE( Option );

    public:
        Option();
        ~Option() override;

    public:
        void setKey( const Char * _key );
        void setKey( const Char * _key, size_t _size );
        void addValue( const Char * _value );
        void addValue( const Char * _value, size_t _size );
        void setProtected( bool _protected );

    public:
        const Char * getKey() const override;
        const Char * getValue( uint32_t _index ) const override;
        uint32_t getValueCount() const override;
        bool isProtected() const override;

    protected:
        StaticString<MENGINE_OPTIONS_KEY_SIZE> m_key;
        StaticString<MENGINE_OPTIONS_VALUE_SIZE> m_values[MENGINE_OPTIONS_VALUES_MAX];
        uint32_t m_valueCount;
        bool m_protected;
    };
    //////////////////////////////////////////////////////////////////////////
    typedef IntrusivePtr<Option, OptionInterface> OptionPtr;
    //////////////////////////////////////////////////////////////////////////
}
