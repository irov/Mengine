#include "ServiceProvider.h"

#include "Interface/ServiceInterface.h"

#include "Kernel/ConstStringHelper.h"
#include "Kernel/Assertion.h"
#include "Kernel/AssertionMemoryPanic.h"

#include "Config/StdString.h"
#include "Config/Algorithm.h"

//////////////////////////////////////////////////////////////////////////
SERVICE_PROVIDER_FACTORY( ServiceProvider, Mengine::ServiceProvider );
//////////////////////////////////////////////////////////////////////////
namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    ServiceProvider::ServiceProvider()
        : m_servicesCount( 0 )
        , m_dependenciesCount( 0 )
        , m_leaveCount( 0 )
        , m_waitsCount( 0 )
#ifdef MENGINE_DEBUG
        , m_initializeServiceName( nullptr )
#endif
    {
        Algorithm::fill_n( m_services, MENGINE_SERVICE_PROVIDER_COUNT, ServiceDesc() );
    }
    //////////////////////////////////////////////////////////////////////////
    ServiceProvider::~ServiceProvider()
    {
    }
    //////////////////////////////////////////////////////////////////////////
    ServiceInterfacePtr ServiceProvider::generateService_( FServiceProviderGenerator _generator, const DocumentPtr & _doc )
    {
        MENGINE_UNUSED( _doc );

        if( _generator == nullptr )
        {
            return nullptr;
        }

        ServiceInterface * service;
        if( (*_generator)(&service) == false )
        {
            return nullptr;
        }

        MENGINE_ASSERTION_MEMORY_PANIC( service );

#if MENGINE_DOCUMENT_ENABLE
        service->setDocument( _doc );
#endif

        return ServiceInterfacePtr::from( service );
    }
    //////////////////////////////////////////////////////////////////////////
    bool ServiceProvider::createService( FServiceProviderGenerator _generator, bool _safe, const DocumentPtr & _doc )
    {
        MENGINE_RELEASE_UNUSED( _safe );

        MENGINE_ASSERTION_EXCEPTION( m_servicesCount != MENGINE_SERVICE_PROVIDER_COUNT, "overflow service count (doc: %s)"
            , MENGINE_DOCUMENT_STR( _doc )
        );

        ServiceInterfacePtr service = this->generateService_( _generator, _doc );

        if( service == nullptr )
        {
            MENGINE_ASSERTION_EXCEPTION( _safe == true, "invalid generate service (doc: %s)"
                , MENGINE_DOCUMENT_STR( _doc )
            );

            return false;
        }

        const Char * name = service->getServiceID();

#ifdef MENGINE_DEBUG
        MENGINE_ASSERTION_CRITICAL( m_initializeServiceName == nullptr );

        m_initializeServiceName = name;
#endif

        bool successful = false;

        try
        {
            successful = service->initializeService();
        }
        catch( const std::exception & ex )
        {
            MENGINE_UNUSED( ex );

#ifdef MENGINE_DEBUG
            m_initializeServiceName = nullptr;
#endif

            MENGINE_ASSERTION_EXCEPTION( _safe == true, "exception initialize service '%s' (doc: %s)\n%s"
                , service->getServiceID()
                , MENGINE_DOCUMENT_STR( _doc )
                , ex.what()
            );
        }

#ifdef MENGINE_DEBUG
        m_initializeServiceName = nullptr;
#endif

        if( successful == false )
        {
            MENGINE_ASSERTION_EXCEPTION( _safe == true, "invalid initialize service '%s' (doc: %s)"
                , service->getServiceID()
                , MENGINE_DOCUMENT_STR( _doc )
            );

            return false;
        }

        MENGINE_ASSERTION_FATAL( MENGINE_STRLEN( service->getServiceID() ) < MENGINE_SERVICE_PROVIDER_NAME_SIZE, "invalid service name '%s' max size '%zu' >= '%u'"
            , service->getServiceID()
            , MENGINE_STRLEN( service->getServiceID() )
            , MENGINE_SERVICE_PROVIDER_NAME_SIZE
        );

        for( uint32_t index = 0; index != MENGINE_SERVICE_PROVIDER_COUNT; ++index )
        {
            ServiceDesc & desc = m_services[index];

            if( desc.name.size() == 0 )
            {
                desc.name.assign( name );
            }
            else if( desc.name.compare( name ) != 0 )
            {
                continue;
            }
            else
            {
                if( desc.exist == true )
                {
                    desc.service->replaceService();
                    desc.service->finalizeService();
                    desc.service = nullptr;
                }
            }

            desc.exist = true;
            desc.available = service->isAvailableService();
            desc.initialize = service->isInitializeService();
            desc.service = service;

            ++m_servicesCount;

            if( this->checkWaits_( name ) == false )
            {
                MENGINE_THROW_EXCEPTION( "invalid initialize service '%s' (waits) (doc: %s)"
                    , service->getServiceID()
                    , MENGINE_DOCUMENT_STR( _doc )
                );

                return false;
            }

            if( service->runService() == false )
            {
                MENGINE_THROW_EXCEPTION( "invalid run service '%s' (waits) (doc: %s)"
                    , service->getServiceID()
                    , MENGINE_DOCUMENT_STR( _doc )
                );

                return false;
            }

            return true;
        }

        MENGINE_THROW_EXCEPTION( "invalid allocate service name '%s' max count [%u] (doc: %s)"
            , service->getServiceID()
            , MENGINE_SERVICE_PROVIDER_COUNT
            , MENGINE_DOCUMENT_STR( _doc )
        );

        return false;
    }
    //////////////////////////////////////////////////////////////////////////
    bool ServiceProvider::finalizeService( const Char * _name )
    {
        for( uint32_t index = 0; index != m_leaveCount; ++index )
        {
            LeaveDesc & desc = m_leaving[index];

            if( desc.name.compare( _name ) != 0 )
            {
                continue;
            }

            LambdaLeaveService lambda = desc.lambda;

            desc.lambda = nullptr;
            desc.name.clear();
            desc.owner.clear();

            lambda();
        }

        for( uint32_t index = 0; index != m_dependenciesCount; ++index )
        {
            const DependencyDesc & desc = m_dependencies[index];

            if( desc.dependency.compare( _name ) != 0 )
            {
                continue;
            }

            this->finalizeService( desc.name.c_str() );
        }

        for( uint32_t index = 0; index != m_servicesCount; ++index )
        {
            ServiceDesc & desc = m_services[index];

            if( desc.name.compare( _name ) != 0 )
            {
                continue;
            }

            if( desc.service == nullptr )
            {
                break;
            }

            desc.exist = false;
            desc.initialize = false;
            desc.service->finalizeService();

            return true;
        }

        return false;
    }
    //////////////////////////////////////////////////////////////////////////
    bool ServiceProvider::destroyService( const Char * _name )
    {
        for( uint32_t index = 0; index != m_dependenciesCount; ++index )
        {
            const DependencyDesc & desc = m_dependencies[index];

            if( desc.dependency.compare( _name ) != 0 )
            {
                continue;
            }

            this->destroyService( desc.name.c_str() );
        }

        for( uint32_t index = 0; index != m_servicesCount; ++index )
        {
            ServiceDesc & desc = m_services[index];

            if( desc.name.compare( _name ) != 0 )
            {
                continue;
            }

            if( desc.service == nullptr )
            {
                break;
            }

            this->removeDependency_( _name );

            desc.exist = false;
            desc.initialize = false;
            desc.service->finalizeService();
            desc.service = nullptr;

            return true;
        }

        return false;
    }
    //////////////////////////////////////////////////////////////////////////
    void ServiceProvider::dependencyService( const Char * _name, const Char * _dependency )
    {
        MENGINE_ASSERTION_FATAL( m_dependenciesCount < MENGINE_SERVICE_PROVIDER_DEPENDENCY_COUNT );
        MENGINE_ASSERTION_FATAL( MENGINE_STRLEN( _name ) < MENGINE_SERVICE_PROVIDER_NAME_SIZE, "invalid service name '%s' max size '%zu' >= '%u'"
            , _name
            , MENGINE_STRLEN( _name )
            , MENGINE_SERVICE_PROVIDER_NAME_SIZE
        );

        DependencyDesc & desc = m_dependencies[m_dependenciesCount++];

        desc.name.assign( _name );
        desc.dependency.assign( _dependency );
    }
    //////////////////////////////////////////////////////////////////////////
    bool ServiceProvider::waitService( const Char * _owner, const Char * _name, const LambdaWaitService & _lambda )
    {
        MENGINE_ASSERTION_FATAL( m_waitsCount < MENGINE_SERVICE_PROVIDER_MAX_WAIT );
        MENGINE_ASSERTION_FATAL( MENGINE_STRLEN( _name ) < MENGINE_SERVICE_PROVIDER_NAME_SIZE, "invalid service name '%s' max size '%zu' >= '%u'"
            , _name
            , MENGINE_STRLEN( _name )
            , MENGINE_SERVICE_PROVIDER_NAME_SIZE
        );

        for( uint32_t index = 0; index != m_servicesCount; ++index )
        {
            ServiceDesc & desc = m_services[index];

            if( desc.name.compare( _name ) != 0 )
            {
                continue;
            }

            if( desc.service == nullptr )
            {
                break;
            }

            if( _lambda() == false )
            {
                return false;
            }

            return true;
        }

        WaitDesc & desc = m_waits[m_waitsCount++];

        desc.owner.assign( _owner );
        desc.name.assign( _name );
        desc.lambda = _lambda;

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    bool ServiceProvider::leaveService( const Char * _owner, const Char * _name, const LambdaLeaveService & _lambda )
    {
        MENGINE_ASSERTION_FATAL( m_leaveCount < MENGINE_SERVICE_PROVIDER_LEAVE_COUNT );
        MENGINE_ASSERTION_FATAL( MENGINE_STRLEN( _name ) < MENGINE_SERVICE_PROVIDER_NAME_SIZE, "invalid service name '%s' max size '%zu' >= '%u'"
            , _name
            , MENGINE_STRLEN( _name )
            , MENGINE_SERVICE_PROVIDER_NAME_SIZE
        );

        LeaveDesc & desc = m_leaving[m_leaveCount++];

        desc.owner.assign( _owner );
        desc.name.assign( _name );
        desc.lambda = _lambda;

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    void ServiceProvider::unlinkService( const Char * _owner )
    {
        for( uint32_t index = 0; index != m_waitsCount; ++index )
        {
            WaitDesc & desc = m_waits[index];

            if( desc.owner.compare( _owner ) != 0 )
            {
                continue;
            }

            desc.lambda = nullptr;
            desc.name.clear();
            desc.owner.clear();
        }

        for( uint32_t index = 0; index != m_leaveCount; ++index )
        {
            LeaveDesc & desc = m_leaving[index];

            if( desc.owner.compare( _owner ) != 0 )
            {
                continue;
            }

            if( desc.lambda == nullptr )
            {
                continue;
            }

            LambdaLeaveService lambda = desc.lambda;

            desc.lambda = nullptr;
            desc.name.clear();
            desc.owner.clear();

            lambda();
        }
    }
    //////////////////////////////////////////////////////////////////////////
    void ServiceProvider::removeDependency_( const Char * _name )
    {
        for( uint32_t index = 0; index != m_dependenciesCount; )
        {
            DependencyDesc & desc = m_dependencies[index];

            if( desc.name[0] != '\0' )
            {
                ++index;

                continue;
            }

            if( desc.name.compare( _name ) != 0 )
            {
                ++index;

                continue;
            }

            DependencyDesc & last_desc = m_dependencies[--m_dependenciesCount];

            desc.name.assign( last_desc.name );
            desc.dependency.assign( last_desc.dependency );
        }
    }
    //////////////////////////////////////////////////////////////////////////
    bool ServiceProvider::checkWaits_( const Char * _name )
    {
        for( uint32_t index = 0; index != m_waitsCount; )
        {
            WaitDesc & desc = m_waits[index];

            if( desc.name.compare( _name ) != 0 )
            {
                ++index;

                continue;
            }

            LambdaWaitService lambda = desc.lambda;

            WaitDesc & last_desc = m_waits[--m_waitsCount];

            desc.name.assign( last_desc.name );
            desc.lambda = last_desc.lambda;

            if( lambda() == false )
            {
                return false;
            }
        }

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    const bool * ServiceProvider::isExistServiceProvider( const Char * _name )
    {
#ifdef MENGINE_DEBUG
        MENGINE_ASSERTION_CRITICAL( m_initializeServiceName == nullptr || MENGINE_STRCMP( m_initializeServiceName, _name ) != 0 );
#endif

        for( uint32_t index = 0; index != m_servicesCount; ++index )
        {
            const ServiceDesc & desc = m_services[index];

            if( desc.service == nullptr )
            {
                continue;
            }

            if( desc.name.compare( _name ) != 0 )
            {
                continue;
            }

            return &desc.exist;
        }

        ServiceDesc & desc = m_services[m_servicesCount++];

        desc.name.assign( _name );
        desc.service = nullptr;
        desc.exist = false;
        desc.available = false;
        desc.initialize = false;

        return &desc.exist;
    }
    //////////////////////////////////////////////////////////////////////////
    const bool * ServiceProvider::isAvailableServiceProvider( const Char * _name )
    {
#ifdef MENGINE_DEBUG
        MENGINE_ASSERTION_CRITICAL( m_initializeServiceName == nullptr || MENGINE_STRCMP( m_initializeServiceName, _name ) != 0 );
#endif

        for( uint32_t index = 0; index != m_servicesCount; ++index )
        {
            const ServiceDesc & desc = m_services[index];

            if( desc.service == nullptr )
            {
                continue;
            }

            if( desc.name.compare( _name ) != 0 )
            {
                continue;
            }

            return &desc.available;
        }

        ServiceDesc & desc = m_services[m_servicesCount++];

        desc.name.assign( _name );
        desc.service = nullptr;
        desc.exist = false;
        desc.available = false;
        desc.initialize = false;

        return &desc.available;
    }
    //////////////////////////////////////////////////////////////////////////
    const bool * ServiceProvider::isInitializeServiceProvider( const Char * _name )
    {
#ifdef MENGINE_DEBUG
        MENGINE_ASSERTION_CRITICAL( m_initializeServiceName == nullptr || MENGINE_STRCMP( m_initializeServiceName, _name ) != 0 );
#endif

        for( uint32_t index = 0; index != m_servicesCount; ++index )
        {
            const ServiceDesc & desc = m_services[index];

            if( desc.service == nullptr )
            {
                continue;
            }

            if( desc.name.compare( _name ) != 0 )
            {
                continue;
            }

            return &desc.initialize;
        }

        ServiceDesc & desc = m_services[m_servicesCount++];

        desc.name.assign( _name );
        desc.service = nullptr;
        desc.exist = false;
        desc.available = false;
        desc.initialize = false;

        return &desc.initialize;
    }
    //////////////////////////////////////////////////////////////////////////
    const ServiceInterfacePtr & ServiceProvider::getService( const Char * _name ) const
    {
        for( uint32_t index = 0; index != m_servicesCount; ++index )
        {
            const ServiceDesc & desc = m_services[index];

            if( desc.name.compare( _name ) != 0 )
            {
                continue;
            }

            if( desc.service == nullptr )
            {
                break;
            }

            return desc.service;
        }

        return ServiceInterfacePtr::none();
    }
    //////////////////////////////////////////////////////////////////////////
    void ServiceProvider::destroy()
    {
        for( uint32_t index = 0; index != m_servicesCount; ++index )
        {
            ServiceDesc & desc = m_services[index];

            if( desc.service == nullptr )
            {
                continue;
            }

            if( desc.service->isInitializeService() == false )
            {
                continue;
            }

            desc.service->finalizeService();
        }

        for( uint32_t index = m_servicesCount; index != 0; --index )
        {
            ServiceDesc & desc = m_services[index - 1];

            if( desc.service == nullptr )
            {
                continue;
            }

            desc.service = nullptr;
        }

        for( uint32_t index = 0; index != m_waitsCount; ++index )
        {
            WaitDesc & desc = m_waits[index];

            if( desc.lambda == nullptr )
            {
                continue;
            }

            MENGINE_ASSERTION_FATAL( false, "service forgot wait '%s'"
                , desc.name.c_str()
            );
        }

        for( uint32_t index = 0; index != m_leaveCount; ++index )
        {
            LeaveDesc & desc = m_leaving[index];

            if( desc.lambda == nullptr )
            {
                continue;
            }

            MENGINE_ASSERTION_FATAL( false, "service forgot leave '%s'"
                , desc.name.c_str()
            );
        }
    }
    //////////////////////////////////////////////////////////////////////////
    void ServiceProvider::stopServices()
    {
        for( uint32_t index = m_servicesCount; index != 0; --index )
        {
            ServiceDesc & desc = m_services[index - 1];

            if( desc.service == nullptr )
            {
                continue;
            }

            desc.service->stopService();
        }
    }
    //////////////////////////////////////////////////////////////////////////
}
