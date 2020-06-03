#include "ServiceProvider.h"

#include "Interface/ServiceInterface.h"

#include "Kernel/ConstStringHelper.h"
#include "Kernel/Assertion.h"
#include "Kernel/AssertionMemoryPanic.h"

#include "Config/StdString.h"

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
        for( uint32_t index = 0; index != MENGINE_SERVICE_PROVIDER_COUNT; ++index )
        {
            ServiceDesc & desc = m_services[index];

            desc.name[0] = '\0';
            desc.service = nullptr;
        }

        for( uint32_t index = 0; index != MENGINE_SERVICE_PROVIDER_DEPENDENCY_COUNT; ++index )
        {
            DependencyDesc & desc = m_dependencies[index];

            desc.name[0] = '\0';
            desc.dependency[0] = '\0';
        }

        for( uint32_t index = 0; index != MENGINE_SERVICE_PROVIDER_MAX_WAIT; ++index )
        {
            WaitDesc & desc = m_waits[index];

            desc.name[0] = '\0';
            desc.lambda = nullptr;
        }
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

        MENGINE_ASSERTION_MEMORY_PANIC( service, nullptr );

#ifdef MENGINE_DEBUG
        service->setDocument( _doc );
#endif

        return ServiceInterfacePtr::from( service );
    }
    //////////////////////////////////////////////////////////////////////////
    bool ServiceProvider::createService( FServiceProviderGenerator _generator, bool _safe, const DocumentPtr & _doc )
    {
        MENGINE_RELEASE_UNUSED( _safe );

        MENGINE_ASSERTION_EXCEPTION( m_servicesCount != MENGINE_SERVICE_PROVIDER_COUNT, "overflow service count doc '%s'"
            , MENGINE_DOCUMENT_STR( _doc )
        );

        ServiceInterfacePtr service = this->generateService_( _generator, _doc );

        if( service == nullptr )
        {
            MENGINE_ASSERTION_EXCEPTION( _safe == true, "invalid generate service doc '%s'"
                , MENGINE_DOCUMENT_STR( _doc )
            );

            return false;
        }

        const Char * name = service->getServiceID();

#ifdef MENGINE_DEBUG
        MENGINE_ASSERTION_CRITICAL( m_initializeServiceName == nullptr );

        m_initializeServiceName = name;
#endif

        bool successful = service->initializeService();

#ifdef MENGINE_DEBUG
        m_initializeServiceName = nullptr;
#endif

        if( successful == false )
        {
            MENGINE_ASSERTION_EXCEPTION( _safe == true, "invalid initialize service '%s' doc '%s'"
                , name
                , MENGINE_DOCUMENT_STR( _doc )
            );

            return false;
        }

        MENGINE_ASSERTION( MENGINE_STRLEN( name ) < MENGINE_SERVICE_PROVIDER_NAME_SIZE, "invalid service name '%s' max size '%zu' >= '%u'"
            , name
            , MENGINE_STRLEN( name )
            , MENGINE_SERVICE_PROVIDER_NAME_SIZE
        );

        for( uint32_t index = 0; index != MENGINE_SERVICE_PROVIDER_COUNT; ++index )
        {
            ServiceDesc & desc = m_services[index];

            if( MENGINE_STRLEN( desc.name ) == 0 )
            {
                MENGINE_STRCPY( desc.name, name );
            }
            else if( MENGINE_STRCMP( desc.name, name ) != 0 )
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
                MENGINE_THROW_EXCEPTION("invalid initialize service '%s' doc '%s' (waits)"
                    , name
                    , MENGINE_DOCUMENT_STR( _doc )
                    );

                return false;
            }

            service->runService();

            return true;
        }

        MENGINE_THROW_EXCEPTION("invalid allocate service name '%s' max count '%d' (doc %s)"
            , name
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

            if( MENGINE_STRCMP( desc.name, _name ) != 0 )
            {
                continue;
            }

            LambdaLeaveService lambda = desc.lambda;

            desc.lambda = nullptr;
            desc.name[0] = '\0';

            lambda();
        }

        for( uint32_t index = 0; index != m_dependenciesCount; ++index )
        {
            const DependencyDesc & desc = m_dependencies[index];

            if( MENGINE_STRCMP( desc.dependency, _name ) != 0 )
            {
                continue;
            }

            this->finalizeService( desc.name );
        }

        for( uint32_t index = 0; index != m_servicesCount; ++index )
        {
            ServiceDesc & desc = m_services[index];

            if( MENGINE_STRCMP( desc.name, _name ) != 0 )
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

            if( MENGINE_STRCMP( desc.dependency, _name ) != 0 )
            {
                continue;
            }

            this->destroyService( desc.name );
        }

        for( uint32_t index = 0; index != m_servicesCount; ++index )
        {
            ServiceDesc & desc = m_services[index];

            if( MENGINE_STRCMP( desc.name, _name ) != 0 )
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
        MENGINE_ASSERTION( m_dependenciesCount < MENGINE_SERVICE_PROVIDER_DEPENDENCY_COUNT );
        MENGINE_ASSERTION( MENGINE_STRLEN( _name ) < MENGINE_SERVICE_PROVIDER_NAME_SIZE, "invalid service name '%s' max size '%zu' >= '%u'"
            , _name
            , MENGINE_STRLEN( _name )
            , MENGINE_SERVICE_PROVIDER_NAME_SIZE
            );

        DependencyDesc & desc = m_dependencies[m_dependenciesCount++];

        MENGINE_STRCPY( desc.name, _name );
        MENGINE_STRCPY( desc.dependency, _dependency );
    }
    //////////////////////////////////////////////////////////////////////////
    bool ServiceProvider::waitService( const Char * _owner, const Char * _name, const LambdaWaitService & _lambda )
    {
        MENGINE_ASSERTION( m_waitsCount < MENGINE_SERVICE_PROVIDER_MAX_WAIT );
        MENGINE_ASSERTION( MENGINE_STRLEN( _name ) < MENGINE_SERVICE_PROVIDER_NAME_SIZE, "invalid service name '%s' max size '%zu' >= '%u'"
            , _name
            , MENGINE_STRLEN( _name )
            , MENGINE_SERVICE_PROVIDER_NAME_SIZE
        );

        for( uint32_t index = 0; index != m_servicesCount; ++index )
        {
            ServiceDesc & desc = m_services[index];

            if( MENGINE_STRCMP( desc.name, _name ) != 0 )
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

        MENGINE_STRCPY( desc.owner, _owner );
        MENGINE_STRCPY( desc.name, _name );
        desc.lambda = _lambda;

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    bool ServiceProvider::leaveService( const Char * _owner, const Char * _name, const LambdaLeaveService & _lambda )
    {
        MENGINE_ASSERTION( m_leaveCount < MENGINE_SERVICE_PROVIDER_LEAVE_COUNT );
        MENGINE_ASSERTION( MENGINE_STRLEN( _name ) < MENGINE_SERVICE_PROVIDER_NAME_SIZE, "invalid service name '%s' max size '%zu' >= '%u'"
            , _name
            , MENGINE_STRLEN( _name )
            , MENGINE_SERVICE_PROVIDER_NAME_SIZE
        );

        LeaveDesc & desc = m_leaving[m_leaveCount++];

        MENGINE_STRCPY( desc.owner, _owner );
        MENGINE_STRCPY( desc.name, _name );
        desc.lambda = _lambda;

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    void ServiceProvider::unlinkService( const Char * _owner )
    {
        for( uint32_t index = 0; index != m_waitsCount; ++index )
        {
            WaitDesc & desc = m_waits[index];

            if( MENGINE_STRCMP( desc.owner, _owner ) != 0 )
            {
                continue;
            }

            desc.lambda = nullptr;
            desc.owner[0] = '\0';
        }

        for( uint32_t index = 0; index != m_leaveCount; ++index )
        {
            LeaveDesc & desc = m_leaving[index];

            if( MENGINE_STRCMP( desc.owner, _owner ) != 0 )
            {
                continue;
            }

            desc.lambda = nullptr;
            desc.name[0] = '\0';
            desc.owner[0] = '\0';
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

            if( MENGINE_STRCMP( desc.name, _name ) != 0 )
            {
                ++index;

                continue;
            }

            DependencyDesc & last_desc = m_dependencies[--m_dependenciesCount];

            MENGINE_STRCPY( desc.name, last_desc.name );
            MENGINE_STRCPY( desc.dependency, last_desc.dependency );
        }
    }
    //////////////////////////////////////////////////////////////////////////
    bool ServiceProvider::checkWaits_( const Char * _name )
    {
        for( uint32_t index = 0; index != m_waitsCount; )
        {
            WaitDesc & desc = m_waits[index];

            if( MENGINE_STRCMP( desc.name, _name ) != 0 )
            {
                ++index;

                continue;
            }

            LambdaWaitService lambda = desc.lambda;

            WaitDesc & last_desc = m_waits[--m_waitsCount];

            MENGINE_STRCPY( desc.name, last_desc.name );
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

            if( MENGINE_STRCMP( desc.name, _name ) != 0 )
            {
                continue;
            }

            return &desc.exist;
        }

        ServiceDesc & desc = m_services[m_servicesCount++];

        MENGINE_STRCPY( desc.name, _name );
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

            if( MENGINE_STRCMP( desc.name, _name ) != 0 )
            {
                continue;
            }

            return &desc.available;
        }

        ServiceDesc & desc = m_services[m_servicesCount++];

        MENGINE_STRCPY( desc.name, _name );
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

            if( MENGINE_STRCMP( desc.name, _name ) != 0 )
            {
                continue;
            }

            return &desc.initialize;
        }

        ServiceDesc & desc = m_services[m_servicesCount++];

        MENGINE_STRCPY( desc.name, _name );
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

            if( MENGINE_STRCMP( desc.name, _name ) != 0 )
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
                , desc.name
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
                , desc.name
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
}