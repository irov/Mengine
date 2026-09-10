#include "AttributionService.h"

#include "Kernel/AssertionMemoryPanic.h"
#include "Kernel/Logger.h"
#include "Kernel/ThreadMutexHelper.h"
#include "Kernel/ThreadMutexScope.h"

#include "Config/StdAlgorithm.h"
#include "Config/StdLimits.h"

//////////////////////////////////////////////////////////////////////////
SERVICE_FACTORY( AttributionService, Mengine::AttributionService );
//////////////////////////////////////////////////////////////////////////
namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    constexpr uint32_t ATTRIBUTION_MAX_COUNT = 10;
    //////////////////////////////////////////////////////////////////////////
#if defined(MENGINE_DEBUG)
    namespace Detail
    {
        //////////////////////////////////////////////////////////////////////////
        static bool isValidAttributionName( const ConstString & _name )
        {
            if( _name.empty() == true )
            {
                return false;
            }

            for( Char character : _name )
            {
                if( character >= 'a' && character <= 'z' )
                {
                    continue;
                }

                if( character >= 'A' && character <= 'Z' )
                {
                    continue;
                }

                if( character >= '0' && character <= '9' )
                {
                    continue;
                }

                if( character == '_' )
                {
                    continue;
                }

                return false;
            }

            return true;
        }
        //////////////////////////////////////////////////////////////////////////
        static bool isValidAttributionValue( EAttributionType _type, const ParamVariant & _value )
        {
            if( Helper::is<ParamNull>( _value ) == true )
            {
                return true;
            }

            switch( _type )
            {
            case EAttributionType::EAT_BOOLEAN:
                {
                    bool successful = Helper::is<ParamBool>( _value );

                    return successful;
                }
            case EAttributionType::EAT_INTEGER:
                {
                    bool successful = Helper::is<ParamInteger>( _value );

                    return successful;
                }
            case EAttributionType::EAT_DOUBLE:
                {
                    ParamDouble value;
                    if( Helper::get( _value, &value ) == false )
                    {
                        return false;
                    }

                    double maxValue = StdLimits::numeric_limits<double>::max();
                    bool valid = value >= -maxValue && value <= maxValue;

                    return valid;
                }
            case EAttributionType::EAT_STRING:
                {
                    bool successful = Helper::is<ParamString>( _value );

                    return successful;
                }
            default:
                return false;
            }
        }
        //////////////////////////////////////////////////////////////////////////
    }
#endif
    //////////////////////////////////////////////////////////////////////////
    AttributionService::AttributionService()
    {
    }
    //////////////////////////////////////////////////////////////////////////
    AttributionService::~AttributionService()
    {
    }
    //////////////////////////////////////////////////////////////////////////
    bool AttributionService::_initializeService()
    {
        m_mutex = Helper::createThreadMutex( MENGINE_DOCUMENT_FACTORABLE );

        MENGINE_ASSERTION_MEMORY_PANIC( m_mutex, "invalid create attribution mutex" );

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    void AttributionService::_finalizeService()
    {
        m_providers.clear();
        m_values.clear();

        m_mutex = nullptr;
    }
    //////////////////////////////////////////////////////////////////////////
    bool AttributionService::registerAttribution( const ConstString & _name, EAttributionType _type )
    {
        MENGINE_THREAD_MUTEX_SCOPE( m_mutex );

#if defined(MENGINE_DEBUG)
        if( Detail::isValidAttributionName( _name ) == false )
        {
            LOGGER_ERROR( "invalid attribution name '%s'"
                , _name.c_str()
            );

            return false;
        }
#endif

        MapAttributions::const_iterator it = m_values.find( _name );
        if( it != m_values.end() )
        {
            LOGGER_ERROR( "attribution '%s' already registered"
                , _name.c_str()
            );

            return false;
        }

        if( m_values.size() >= ATTRIBUTION_MAX_COUNT )
        {
            LOGGER_ERROR( "cannot register attribution '%s': limit of %u custom properties reached"
                , _name.c_str()
                , ATTRIBUTION_MAX_COUNT
            );

            return false;
        }

        AttributionDesc attribution;
        attribution.value = nullptr;
        attribution.type = _type;

        m_values.emplace( _name, attribution );

        this->notifyProviders_( _name, _type, attribution.value );

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    bool AttributionService::setAttribution( const ConstString & _name, const ParamVariant & _value )
    {
        MENGINE_THREAD_MUTEX_SCOPE( m_mutex );

        MapAttributions::iterator it = m_values.find( _name );
        if( it == m_values.end() )
        {
            LOGGER_ERROR( "unknown attribution '%s'"
                , _name.c_str()
            );

            return false;
        }

        AttributionDesc & attribution = it->second;

#if defined(MENGINE_DEBUG)
        if( Detail::isValidAttributionValue( attribution.type, _value ) == false )
        {
            LOGGER_ERROR( "invalid attribution value for '%s'"
                , _name.c_str()
            );

            return false;
        }
#endif

        if( attribution.value == _value )
        {
            return true;
        }

        attribution.value = _value;

        this->notifyProviders_( _name, attribution.type, _value );

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    ParamVariant AttributionService::getAttribution( const ConstString & _name ) const
    {
        MENGINE_THREAD_MUTEX_SCOPE( m_mutex );

        MapAttributions::const_iterator it = m_values.find( _name );
        if( it == m_values.end() )
        {
            return nullptr;
        }

        const AttributionDesc & attribution = it->second;

        return attribution.value;
    }
    //////////////////////////////////////////////////////////////////////////
    void AttributionService::clearAttributions()
    {
        MENGINE_THREAD_MUTEX_SCOPE( m_mutex );

        MapAttributions values = m_values;

        for( MapAttributions::value_type & entry : m_values )
        {
            AttributionDesc & attribution = entry.second;

            attribution.value = nullptr;
        }

        ParamVariant emptyValue = nullptr;

        for( const MapAttributions::value_type & entry : values )
        {
            const AttributionDesc & attribution = entry.second;

            if( Helper::is<ParamNull>( attribution.value ) == true )
            {
                continue;
            }

            this->notifyProviders_( entry.first, attribution.type, emptyValue );
        }
    }
    //////////////////////////////////////////////////////////////////////////
    void AttributionService::addProvider( const AttributionProviderInterfacePtr & _provider )
    {
        MENGINE_THREAD_MUTEX_SCOPE( m_mutex );

        if( _provider == nullptr )
        {
            return;
        }

        if( StdAlgorithm::container_has( m_providers, _provider ) == true )
        {
            return;
        }

        m_providers.emplace_back( _provider );

        AttributionProviderInterfacePtr provider = _provider;
        MapAttributions values = m_values;

        for( const MapAttributions::value_type & entry : values )
        {
            if( StdAlgorithm::container_has( m_providers, provider ) == false )
            {
                return;
            }

            const AttributionDesc & attribution = entry.second;

            provider->onAttributionChanged( entry.first, attribution.type, attribution.value );
        }
    }
    //////////////////////////////////////////////////////////////////////////
    void AttributionService::removeProvider( const AttributionProviderInterfacePtr & _provider )
    {
        MENGINE_THREAD_MUTEX_SCOPE( m_mutex );

        VectorProviders::iterator it_found = StdAlgorithm::find( m_providers.begin(), m_providers.end(), _provider );
        if( it_found == m_providers.end() )
        {
            return;
        }

        m_providers.erase( it_found );
    }
    //////////////////////////////////////////////////////////////////////////
    void AttributionService::notifyProviders_( const ConstString & _name, EAttributionType _type, const ParamVariant & _value )
    {
        ParamVariant value = _value;
        VectorProviders providers = m_providers;

        for( const AttributionProviderInterfacePtr & provider : providers )
        {
            if( StdAlgorithm::container_has( m_providers, provider ) == false )
            {
                continue;
            }

            provider->onAttributionChanged( _name, _type, value );
        }
    }
    //////////////////////////////////////////////////////////////////////////
}
