#include "SecureStringValue.h"

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
    SecureStringValue::SecureStringValue()
        : m_hash( 0 )
    {
    }
    //////////////////////////////////////////////////////////////////////////
    SecureStringValue::~SecureStringValue()
    {
    }
    //////////////////////////////////////////////////////////////////////////
    void SecureStringValue::setUnprotectedValue( const String & _value )
    {
        const Char * value_str = _value.c_str();
        String::size_type value_size = _value.size();

        union
        {
            uint32_t hash32;
            uint8_t hash8[4];
        };

        hash32 = Helper::makeCRC32( value_str, value_size );

        m_value.resize( value_size );

        for( String::size_type index = 0; index != value_size; ++index )
        {
            m_value[index] = _value[index] ^ hash8[index % 4];
        }

        m_hash = hash32;

        m_buffer.resize( value_size );
        m_buffer.assign( m_value.rbegin(), m_value.rend() );
    }
    //////////////////////////////////////////////////////////////////////////
    bool SecureStringValue::getUnprotectedValue( String * const _value ) const
    {
        String::size_type value_size = m_value.size();

        for( String::size_type index = 0; index != value_size; ++index )
        {
            if( m_buffer[index] == m_value[value_size - index - 1] )
            {
                continue;
            }

            return false;
        }

        union
        {
            uint32_t hash32;
            uint8_t hash8[4];
        };

        hash32 = m_hash;

        _value->resize( value_size );

        Char * value_data = _value->data();

        for( String::size_type index = 0; index != value_size; ++index )
        {
            value_data[index] = m_value[index] ^ hash8[index % 4];
        }

        value_data[value_size] = '\0';

        const Char * value_str = _value->c_str();

        uint32_t hash = Helper::makeCRC32( value_str, value_size );

        if( m_hash != hash )
        {
            return false;
        }

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    bool SecureStringValue::setupSecureValue( const SecureStringValuePtr & _setup )
    {
        String setup_unprotected_value;
        if( _setup->getUnprotectedValue( &setup_unprotected_value ) == false )
        {
            return false;
        }

        this->setUnprotectedValue( setup_unprotected_value );

        String total_unprotected_value;
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
    bool SecureStringValue::cmpSecureValue( const SecureStringValuePtr & _value, int32_t * const _result ) const
    {
        String base_unprotected_value;
        if( this->getUnprotectedValue( &base_unprotected_value ) == false )
        {
            return false;
        }

        String test_unprotected_value;
        if( _value->getUnprotectedValue( &test_unprotected_value ) == false )
        {
            return false;
        }

        *_result = MENGINE_STRCMP( test_unprotected_value.c_str(), base_unprotected_value.c_str() );

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    void SecureStringValue::saveHexadecimal( String * const _hexadecimal ) const
    {
        uint32_t value_size = (uint32_t)m_value.size();

        Blobject blob;
        ContainerWriter<Blobject> writer( blob );

        writer.writePOD( m_hash );
        writer.writePOD( value_size );
        writer.writeBuffer( m_value.c_str(), m_value.size() );
        writer.writeBuffer( m_buffer.c_str(), m_buffer.size() );

        uint64_t secureHash = SECURE_SERVICE()
            ->getSecureHash();
        
        Blobject blob_raving;
        blob_raving.resize( blob.size() );

        Helper::ravingcode( secureHash, blob.data(), blob.size(), blob_raving.data() );

        _hexadecimal->resize( blob.size() * 2 );

        Helper::encodeHexadecimal( blob_raving.data(), blob_raving.size(), _hexadecimal->data(), _hexadecimal->size(), nullptr );
    }
    //////////////////////////////////////////////////////////////////////////
    bool SecureStringValue::loadHexadecimal( const String & _hexadecimal )
    {
        if( _hexadecimal.empty() == true )
        {
            return false;
        }

        Blobject blob_raving;
        blob_raving.resize( _hexadecimal.size() / 2 );

        size_t blob_raving_size;
        Helper::decodeHexadecimal( _hexadecimal.c_str(), _hexadecimal.size(), blob_raving.data(), blob_raving.size(), &blob_raving_size );

        Blobject blob;
        blob.resize( blob_raving_size );

        uint64_t secureHash = SECURE_SERVICE()
            ->getSecureHash();

        Helper::ravingcode( secureHash, blob_raving.data(), blob_raving_size, blob.data() );

        ContainerReader<Blobject> reader( blob );

        if( blob.size() <= 8 )
        {
            return false;
        }

        reader.readPOD( m_hash );

        uint32_t value_size;
        reader.readPOD( value_size );

        if( blob.size() <= 8 + value_size * 2 )
        {
            return false;
        }

        m_value.resize( value_size );

        void * value_data = m_value.data();
        size_t value_capacity = m_value.size();

        reader.readBuffer( value_data, value_capacity );

        m_buffer.resize( value_size );

        void * buffer_data = m_buffer.data();
        size_t buffer_capacity = m_buffer.size();

        reader.readBuffer( buffer_data, buffer_capacity );

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
}