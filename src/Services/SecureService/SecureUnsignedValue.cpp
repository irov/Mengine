#include "SecureUnsignedValue.h"

#include "Interface/SecureServiceInterface.h"

#include "Kernel/CRC32.h"
#include "Kernel/Blobject.h"
#include "Kernel/ContainerReader.h"
#include "Kernel/ContainerWriter.h"
#include "Kernel/Ravingcode.h"
#include "Kernel/Hexadecimal.h"

#include "Config/StdIO.h"
#include "Config/StdLib.h"
#include "Config/StdString.h"
#include "Config/Abs.h"

namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    SecureUnsignedValue::SecureUnsignedValue()
        : m_value( 0 )
        , m_hash( 0 )
    {
    }
    //////////////////////////////////////////////////////////////////////////
    SecureUnsignedValue::~SecureUnsignedValue()
    {
    }
    //////////////////////////////////////////////////////////////////////////
    void SecureUnsignedValue::setUnprotectedValue( uint32_t _value )
    {
        uint32_t hash = Helper::makeCRC32Pod( _value );
        m_hash = hash;
        m_value = _value ^ hash;

        MENGINE_SPRINTF( m_buffer, "%x%x", m_value, hash );
    }
    //////////////////////////////////////////////////////////////////////////
    bool SecureUnsignedValue::getUnprotectedValue( uint32_t * const _value ) const
    {
        uint32_t hash = Helper::makeCRC32Pod( m_value ^ m_hash );

        if( m_hash != hash )
        {
            return false;
        }

        Char buffer[20] = {'\0'};
        MENGINE_SNPRINTF( buffer, 20, "%x%x", m_value, hash );

        uint32_t unprotected_value = (m_value ^ hash) ^ (MENGINE_ABS( MENGINE_STRCMP( m_buffer, buffer ) ) * ~0U);

        if( (unprotected_value ^ hash) != m_value )
        {
            return false;
        }

        *_value = unprotected_value;

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    bool SecureUnsignedValue::setupSecureValue( const SecureUnsignedValuePtr & _setup )
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
    bool SecureUnsignedValue::additiveSecureValue( const SecureUnsignedValuePtr & _add )
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
    bool SecureUnsignedValue::substractSecureValue( const SecureUnsignedValuePtr & _sub )
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
    bool SecureUnsignedValue::additive2SecureValue( const SecureUnsignedValuePtr & _add, const SecureUnsignedValuePtr & _pow )
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
    bool SecureUnsignedValue::cmpSecureValue( const SecureUnsignedValuePtr & _value, int32_t * const _result ) const
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
    //////////////////////////////////////////////////////////////////////////
    void SecureUnsignedValue::saveHexadecimal( String * const _hexadecimal ) const
    {
        Blobject blob;
        ContainerWriter<Blobject> writer( blob );

        writer.writePOD( m_hash );
        writer.writePOD( m_value );
        writer.writeBuffer( m_buffer, sizeof( Char ) * 20 );

        HashType secureHash = SECURE_SERVICE()
            ->getSecureHash();

        uint32_t parrot = (uint32_t)secureHash;

        Blobject blob_raving;
        blob_raving.resize( blob.size() );

        Helper::ravingcode( parrot, blob.data(), blob.size(), blob_raving.data() );

        _hexadecimal->resize( blob.size() * 2 );

        Helper::encodeHexadecimal( blob_raving.data(), blob_raving.size(), _hexadecimal->data(), _hexadecimal->size(), nullptr );
    }
    //////////////////////////////////////////////////////////////////////////
    void SecureUnsignedValue::loadHexadecimal( const String & _hexadecimal )
    {
        Blobject blob_raving;
        blob_raving.resize( _hexadecimal.size() );

        Helper::decodeHexadecimal( _hexadecimal.c_str(), _hexadecimal.size(), blob_raving.data(), blob_raving.size(), nullptr );

        Blobject blob;
        blob.resize( _hexadecimal.size() );

        HashType secureHash = SECURE_SERVICE()
            ->getSecureHash();

        uint32_t parrot = (uint32_t)secureHash;

        Helper::ravingcode( parrot, blob_raving.data(), blob_raving.size(), blob.data() );

        ContainerReader<Blobject> reader( blob );

        reader.readPOD( m_hash );
        reader.readPOD( m_value );
        reader.readBuffer( m_buffer, 20 );
    }
    //////////////////////////////////////////////////////////////////////////
}