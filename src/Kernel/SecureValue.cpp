#include "SecureValue.h"
#include "CRC32.h"

#include "Config/StdIO.h"
#include "Config/StdLib.h"
#include "Config/StdString.h"
#include "Config/Abs.h"

namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    SecureValue::SecureValue()
        : m_value( 0 )
        , m_hash( 0 )
    {
    }
    //////////////////////////////////////////////////////////////////////////
    SecureValue::~SecureValue()
    {
    }
    //////////////////////////////////////////////////////////////////////////
    void SecureValue::setUnprotectedValue( uint32_t _value )
    {
        uint32_t hash = Helper::make_crc32_pod( _value );
        m_hash = hash;
        m_value = _value ^ hash;

        MENGINE_SPRINTF( m_buffer, "%x%x", m_value, hash );
    }
    //////////////////////////////////////////////////////////////////////////
    bool SecureValue::getUnprotectedValue( uint32_t * _value ) const
    {
        uint32_t hash = Helper::make_crc32_pod( m_value ^ m_hash );

        if( m_hash != hash )
        {
            return false;
        }

        Char buffer[20];
        MENGINE_SPRINTF( buffer, "%x%x", m_value, hash );

        uint32_t unprotected_value = (m_value ^ hash) ^ (MENGINE_ABS( MENGINE_STRCMP( m_buffer, buffer ) ) * ~0U);

        if( (unprotected_value ^ hash) != m_value )
        {
            return false;
        }

        *_value = unprotected_value;

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    bool SecureValue::setupSecureValue( const SecureValuePtr & _setup )
    {
        uint32_t setup_unprotected_value;
        if( _setup->getUnprotectedValue( &setup_unprotected_value ) == false )
        {
            return false;
        }

        this->setUnprotectedValue( setup_unprotected_value );

        uint32_t total_unprotected_value;
        if( this->getUnprotectedValue( &total_unprotected_value ) == false )
        {
            return false;
        }

        if( total_unprotected_value != setup_unprotected_value )
        {
            return false;
        }

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    bool SecureValue::additiveSecureValue( const SecureValuePtr & _add )
    {
        uint32_t base_unprotected_value;
        if( this->getUnprotectedValue( &base_unprotected_value ) == false )
        {
            return false;
        }

        uint32_t add_unprotected_value;
        if( _add->getUnprotectedValue( &add_unprotected_value ) == false )
        {
            return false;
        }

        this->setUnprotectedValue( base_unprotected_value + add_unprotected_value );

        uint32_t total_unprotected_value;
        if( this->getUnprotectedValue( &total_unprotected_value ) == false )
        {
            return false;
        }

        if( total_unprotected_value != base_unprotected_value + add_unprotected_value )
        {
            return false;
        }

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    bool SecureValue::substractSecureValue( const SecureValuePtr & _sub )
    {
        uint32_t base_unprotected_value;
        if( this->getUnprotectedValue( &base_unprotected_value ) == false )
        {
            return false;
        }

        uint32_t sub_unprotected_value;
        if( _sub->getUnprotectedValue( &sub_unprotected_value ) == false )
        {
            return false;
        }

        this->setUnprotectedValue( base_unprotected_value - sub_unprotected_value );

        uint32_t total_unprotected_value;
        if( this->getUnprotectedValue( &total_unprotected_value ) == false )
        {
            return false;
        }

        if( total_unprotected_value != base_unprotected_value - sub_unprotected_value )
        {
            return false;
        }

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    bool SecureValue::additive2SecureValue( const SecureValuePtr & _add, const SecureValuePtr & _pow )
    {
        uint32_t base_unprotected_value;
        if( this->getUnprotectedValue( &base_unprotected_value ) == false )
        {
            return false;
        }

        uint32_t add_unprotected_value;
        if( _add->getUnprotectedValue( &add_unprotected_value ) == false )
        {
            return false;
        }

        uint32_t pow_unprotected_value;
        if( _pow->getUnprotectedValue( &pow_unprotected_value ) == false )
        {
            return false;
        }

        this->setUnprotectedValue( base_unprotected_value + add_unprotected_value * pow_unprotected_value );

        uint32_t total_unprotected_value;
        if( this->getUnprotectedValue( &total_unprotected_value ) == false )
        {
            return false;
        }

        if( total_unprotected_value != base_unprotected_value + add_unprotected_value * pow_unprotected_value )
        {
            return false;
        }

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    bool SecureValue::cmpSecureValue( const SecureValuePtr & _value, int32_t * _result ) const
    {
        uint32_t base_unprotected_value;
        if( this->getUnprotectedValue( &base_unprotected_value ) == false )
        {
            return false;
        }

        uint32_t test_unprotected_value;
        if( _value->getUnprotectedValue( &test_unprotected_value ) == false )
        {
            return false;
        }

        *_result = test_unprotected_value - base_unprotected_value;

        return true;
    }
}