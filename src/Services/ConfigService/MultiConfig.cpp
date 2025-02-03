#include "MultiConfig.h"

#include "Kernel/AssertionMemoryPanic.h"
#include "Kernel/AssertionNotImplemented.h"
#include "Kernel/Logger.h"
#include "Kernel/Stringalized.h"
#include "Kernel/ArrayString.h"
#include "Kernel/ThreadMutexScope.h"

//////////////////////////////////////////////////////////////////////////
namespace Mengine
{   
    //////////////////////////////////////////////////////////////////////////
    MultiConfig::MultiConfig()
    {
    }
    //////////////////////////////////////////////////////////////////////////
    MultiConfig::~MultiConfig()
    {
    }
    //////////////////////////////////////////////////////////////////////////
    void MultiConfig::setMutex( const ThreadMutexInterfacePtr & _mutex )
    {
        m_mutex = _mutex;
    }
    //////////////////////////////////////////////////////////////////////////
    const ThreadMutexInterfacePtr & MultiConfig::getMutex() const
    {
        return m_mutex;
    }
    //////////////////////////////////////////////////////////////////////////
    void MultiConfig::addConfig( const ConfigInterfacePtr & _config )
    {
        MENGINE_THREAD_MUTEX_SCOPE( m_mutex );

        m_configs.push_back( _config );
    }
    //////////////////////////////////////////////////////////////////////////
    void MultiConfig::foreachConfig( const LambdaConfigs & _lambda )
    {
        MENGINE_THREAD_MUTEX_SCOPE( m_mutex );

        for( const ConfigInterfacePtr & config : m_configs )
        {
            _lambda( config );
        }
    }
    //////////////////////////////////////////////////////////////////////////
    void MultiConfig::setPlatformTags( const Tags & _platformTags )
    {
        MENGINE_THREAD_MUTEX_SCOPE( m_mutex );

        m_platformTags = _platformTags;
    }
    //////////////////////////////////////////////////////////////////////////
    const Tags & MultiConfig::getPlatformTags() const
    {
        return m_platformTags;
    }
    //////////////////////////////////////////////////////////////////////////
    bool MultiConfig::existValue( const Char * _section, const Char * _key ) const
    {
        MENGINE_THREAD_MUTEX_SCOPE( m_mutex );

        for( const ConfigInterfacePtr & config : m_configs )
        {
            if( config->existValue( _section, _key ) == true )
            {
                return true;
            }
        }

        return false;
    }
    //////////////////////////////////////////////////////////////////////////
    bool MultiConfig::load( const InputStreamInterfacePtr & _stream, const DocumentInterfacePtr & _doc )
    {
        MENGINE_THREAD_MUTEX_SCOPE( m_mutex );

        MENGINE_UNUSED( _stream );
        MENGINE_UNUSED( _doc );

        MENGINE_ASSERTION_NOT_IMPLEMENTED();

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    void MultiConfig::unload()
    {
        MENGINE_THREAD_MUTEX_SCOPE( m_mutex );

        m_configs.clear();
    }
    //////////////////////////////////////////////////////////////////////////
    bool MultiConfig::hasValue( const Char * _section, const Char * _key, bool _default, bool * const _value ) const
    {
        MENGINE_THREAD_MUTEX_SCOPE( m_mutex );

        for( const ConfigInterfacePtr & config : m_configs )
        {
            if( config->hasValue( _section, _key, _default, _value ) == true )
            {
                return true;
            }
        }

        *_value = _default;

        return false;
    }
    //////////////////////////////////////////////////////////////////////////
    bool MultiConfig::hasValue( const Char * _section, const Char * _key, int64_t _default, int64_t * const _value ) const
    {
        MENGINE_THREAD_MUTEX_SCOPE( m_mutex );

        for( const ConfigInterfacePtr & config : m_configs )
        {
            if( config->hasValue( _section, _key, _default, _value ) == true )
            {
                return true;
            }
        }

        *_value = _default;

        return false;
    }
    //////////////////////////////////////////////////////////////////////////
    bool MultiConfig::hasValue( const Char * _section, const Char * _key, double _default, double * const _value ) const
    {
        MENGINE_THREAD_MUTEX_SCOPE( m_mutex );

        for( const ConfigInterfacePtr & config : m_configs )
        {
            if( config->hasValue( _section, _key, _default, _value ) == true )
            {
                return true;
            }
        }

        *_value = _default;

        return false;
    }
    //////////////////////////////////////////////////////////////////////////
    bool MultiConfig::hasValue( const Char * _section, const Char * _key, const Char * _default, const Char ** const _value ) const
    {
        MENGINE_THREAD_MUTEX_SCOPE( m_mutex );

        for( const ConfigInterfacePtr & config : m_configs )
        {
            if( config->hasValue( _section, _key, _default, _value ) == true )
            {
                return true;
            }
        }

        *_value = _default;

        return false;
    }
    //////////////////////////////////////////////////////////////////////////
    bool MultiConfig::hasValue( const Char * _section, const Char * _key, const ConstString & _default, ConstString * const _value ) const
    {
        MENGINE_THREAD_MUTEX_SCOPE( m_mutex );

        for( const ConfigInterfacePtr & config : m_configs )
        {
            if( config->hasValue( _section, _key, _default, _value ) == true )
            {
                return true;
            }
        }

        *_value = _default;

        return false;
    }
    //////////////////////////////////////////////////////////////////////////
    bool MultiConfig::hasValue( const Char * _section, const Char * _key, const FilePath & _default, FilePath * const _value ) const
    {
        MENGINE_THREAD_MUTEX_SCOPE( m_mutex );

        for( const ConfigInterfacePtr & config : m_configs )
        {
            if( config->hasValue( _section, _key, _default, _value ) == true )
            {
                return true;
            }
        }

        *_value = _default;

        return false;
    }
    //////////////////////////////////////////////////////////////////////////
    bool MultiConfig::hasValue( const Char * _section, const Char * _key, const Tags & _default, Tags * const _value ) const
    {
        MENGINE_THREAD_MUTEX_SCOPE( m_mutex );

        for( const ConfigInterfacePtr & config : m_configs )
        {
            if( config->hasValue( _section, _key, _default, _value ) == true )
            {
                return true;
            }
        }

        *_value = _default;

        return false;
    }
    //////////////////////////////////////////////////////////////////////////
    bool MultiConfig::hasValue( const Char * _section, const Char * _key, const Resolution & _default, Resolution * const _value ) const
    {
        MENGINE_THREAD_MUTEX_SCOPE( m_mutex );

        for( const ConfigInterfacePtr & config : m_configs )
        {
            if( config->hasValue( _section, _key, _default, _value ) == true )
            {
                return true;
            }
        }

        *_value = _default;

        return false;
    }
    //////////////////////////////////////////////////////////////////////////
    bool MultiConfig::hasValue( const Char * _section, const Char * _key, const Color & _default, Color * const _value ) const
    {
        MENGINE_THREAD_MUTEX_SCOPE( m_mutex );

        for( const ConfigInterfacePtr & config : m_configs )
        {
            if( config->hasValue( _section, _key, _default, _value ) == true )
            {
                return true;
            }
        }

        *_value = _default;

        return false;
    }
    //////////////////////////////////////////////////////////////////////////
    bool MultiConfig::setValue( const Char * _section, const Char * _key, bool _value )
    {
        MENGINE_UNUSED( _section );
        MENGINE_UNUSED( _key );
        MENGINE_UNUSED( _value );

        MENGINE_ASSERTION_NOT_IMPLEMENTED();

        return false;
    }
    //////////////////////////////////////////////////////////////////////////
    bool MultiConfig::setValue( const Char * _section, const Char * _key, int64_t _value )
    {
        MENGINE_UNUSED( _section );
        MENGINE_UNUSED( _key );
        MENGINE_UNUSED( _value );

        MENGINE_ASSERTION_NOT_IMPLEMENTED();

        return false;
    }
    //////////////////////////////////////////////////////////////////////////
    bool MultiConfig::setValue( const Char * _section, const Char * _key, double _value )
    {
        MENGINE_UNUSED( _section );
        MENGINE_UNUSED( _key );
        MENGINE_UNUSED( _value );

        MENGINE_ASSERTION_NOT_IMPLEMENTED();

        return false;
    }
    //////////////////////////////////////////////////////////////////////////
    bool MultiConfig::setValue( const Char * _section, const Char * _key, const Char * _value )
    {
        MENGINE_UNUSED( _section );
        MENGINE_UNUSED( _key );
        MENGINE_UNUSED( _value );

        MENGINE_ASSERTION_NOT_IMPLEMENTED();

        return false;
    }
    //////////////////////////////////////////////////////////////////////////
    bool MultiConfig::setValue( const Char * _section, const Char * _key, const ConstString & _value )
    {
        MENGINE_UNUSED( _section );
        MENGINE_UNUSED( _key );
        MENGINE_UNUSED( _value );

        MENGINE_ASSERTION_NOT_IMPLEMENTED();

        return false;
    }
    //////////////////////////////////////////////////////////////////////////
    bool MultiConfig::setValue( const Char * _section, const Char * _key, const FilePath & _value )
    {
        MENGINE_UNUSED( _section );
        MENGINE_UNUSED( _key );
        MENGINE_UNUSED( _value );

        MENGINE_ASSERTION_NOT_IMPLEMENTED();

        return false;
    }
    //////////////////////////////////////////////////////////////////////////
    bool MultiConfig::setValue( const Char * _section, const Char * _key, const Tags & _value )
    {
        MENGINE_UNUSED( _section );
        MENGINE_UNUSED( _key );
        MENGINE_UNUSED( _value );

        MENGINE_ASSERTION_NOT_IMPLEMENTED();

        return false;
    }
    //////////////////////////////////////////////////////////////////////////
    bool MultiConfig::setValue( const Char * _section, const Char * _key, const Resolution & _value )
    {
        MENGINE_UNUSED( _section );
        MENGINE_UNUSED( _key );
        MENGINE_UNUSED( _value );

        MENGINE_ASSERTION_NOT_IMPLEMENTED();

        return false;
    }
    //////////////////////////////////////////////////////////////////////////
    bool MultiConfig::setValue( const Char * _section, const Char * _key, const Color & _value )
    {
        MENGINE_UNUSED( _section );
        MENGINE_UNUSED( _key );
        MENGINE_UNUSED( _value );

        MENGINE_ASSERTION_NOT_IMPLEMENTED();

        return false;
    }
    //////////////////////////////////////////////////////////////////////////
    void MultiConfig::getValues( const Char * _section, const Char * _key, VectorAspectRatioViewports * const _values ) const
    {
        MENGINE_THREAD_MUTEX_SCOPE( m_mutex );

        for( const ConfigInterfacePtr & config : m_configs )
        {
            config->getValues( _section, _key, _values );
        }
    }
    //////////////////////////////////////////////////////////////////////////
    void MultiConfig::getValues( const Char * _section, const Char * _key, VectorFilePath * const _values ) const
    {
        MENGINE_THREAD_MUTEX_SCOPE( m_mutex );

        for( const ConfigInterfacePtr & config : m_configs )
        {
            config->getValues( _section, _key, _values );
        }
    }
    //////////////////////////////////////////////////////////////////////////
    void MultiConfig::getValues( const Char * _section, const Char * _key, VectorConstString * const _values ) const
    {
        MENGINE_THREAD_MUTEX_SCOPE( m_mutex );

        for( const ConfigInterfacePtr & config : m_configs )
        {
            config->getValues( _section, _key, _values );
        }
    }
    //////////////////////////////////////////////////////////////////////////
    void MultiConfig::getValues( const Char * _section, const Char * _key, VectorString * const _values ) const
    {
        MENGINE_THREAD_MUTEX_SCOPE( m_mutex );

        for( const ConfigInterfacePtr & config : m_configs )
        {
            config->getValues( _section, _key, _values );
        }
    }
    //////////////////////////////////////////////////////////////////////////
    bool MultiConfig::hasSection( const Char * _section ) const
    {
        MENGINE_THREAD_MUTEX_SCOPE( m_mutex );

        for( const ConfigInterfacePtr & config : m_configs )
        {
            if( config->hasSection( _section ) == true )
            {
                return true;
            }
        }

        return false;
    }
    //////////////////////////////////////////////////////////////////////////
}
