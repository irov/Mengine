#include "ServiceProvider.h"

#include "Interface/ServiceInterface.h"

#include "Kernel/ConstStringHelper.h"
#include "Kernel/Assertion.h"
#include "Kernel/AssertionMemoryPanic.h"
#include "Kernel/SpinLockScope.h"

#include "Config/StdString.h"
#include "Config/StdAlgorithm.h"
#include "Config/StdException.h"

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
#if defined(MENGINE_DEBUG)
        , m_initializeServiceName( nullptr )
#endif
    {
        StdAlgorithm::fill_n( m_services, MENGINE_SERVICE_PROVIDER_COUNT, ServiceDesc() );
    }
    //////////////////////////////////////////////////////////////////////////
    ServiceProvider::~ServiceProvider()
    {
    }
    //////////////////////////////////////////////////////////////////////////
    ServiceInterfacePtr ServiceProvider::generateService_( FServiceProviderGenerator _generator, const DocumentInterfacePtr & _doc )
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

        MENGINE_ASSERTION_MEMORY_PANIC( service, "invalid generate service" );

        service->setUniqueIdentity( ~0U );

#if defined(MENGINE_DOCUMENT_ENABLE)
        service->setDocument( _doc );
#endif

        return ServiceInterfacePtr::from( service );
    }
    //////////////////////////////////////////////////////////////////////////
    bool ServiceProvider::createService( FServiceProviderGenerator _generator, bool _safe, const DocumentInterfacePtr & _doc )
    {
        MENGINE_RELEASE_UNUSED( _safe );

        MENGINE_ASSERTION_EXCEPTION( m_servicesCount != MENGINE_SERVICE_PROVIDER_COUNT, "overflow service count (doc: %s)"
            , MENGINE_DOCUMENT_STR( _doc )
        );

        MENGINE_SPINLOCK_SCOPE( m_futexServices );

        ServiceInterfacePtr service = this->generateService_( _generator, _doc );

        if( service == nullptr )
        {
            MENGINE_ASSERTION_EXCEPTION( _safe == true, "invalid generate service (doc: %s)"
                , MENGINE_DOCUMENT_STR( _doc )
            );

            return false;
        }

        MENGINE_ASSERTION_FATAL( StdString::strlen( service->getServiceId() ) < MENGINE_SERVICE_PROVIDER_NAME_SIZE, "invalid service name '%s' max size '%zu' >= '%u'"
            , service->getServiceId()
            , StdString::strlen( service->getServiceId() )
            , MENGINE_SERVICE_PROVIDER_NAME_SIZE
        );

        const Char * name = service->getServiceId();

        for( uint32_t index = 0; index != MENGINE_SERVICE_PROVIDER_COUNT; ++index )
        {
            ServiceDesc & desc = m_services[index];

            if( desc.name.size() == 0 )
            {
                desc.name.assign( name );

                ++m_servicesCount;
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

            desc.service = nullptr;
            desc.exist = true;
            desc.safe = _safe;
            desc.available = false;
            desc.initialize = false;
            desc.requiring = false;
            desc.available = false;

            if( service->availableService() == false )
            {
                return true;
            }

            desc.service = service;
            desc.available = true;

            const ServiceRequiredList & required = service->requiredServices();

            for( const Char * require_name : required )
            {
                desc.required[desc.required_count++] = require_name;
            }

            if( this->isRequired_( desc ) == false )
            {
                desc.requiring = true;

                return true;
            }

            bool result = true;
            this->initializeService_( &desc, _doc, &result );

            return result;
        }

        MENGINE_THROW_EXCEPTION( "invalid allocate service name '%s' max count [%u] (doc: %s)"
            , service->getServiceId()
            , MENGINE_SERVICE_PROVIDER_COUNT
            , MENGINE_DOCUMENT_STR( _doc )
        );

        return false;
    }
    //////////////////////////////////////////////////////////////////////////
    void ServiceProvider::initializeService_( ServiceDesc * const _desc, const DocumentInterfacePtr & _doc, bool * const _result )
    {
        const ServiceInterfacePtr & service = _desc->service;

#if defined(MENGINE_DEBUG)
        MENGINE_ASSERTION_CRITICAL( m_initializeServiceName == nullptr, "already initialize service '%s' (doc: %s)"
            , m_initializeServiceName
            , MENGINE_DOCUMENT_STR( _doc )
        );

        m_initializeServiceName = service->getServiceId();
#endif

        bool successful = false;

        try
        {
            successful = service->initializeService( this );
        }
        catch( const StdException::exception & ex )
        {
            MENGINE_UNUSED( ex );

#if defined(MENGINE_DEBUG)
            m_initializeServiceName = nullptr;
#endif

            *_result = false;

            MENGINE_ASSERTION_EXCEPTION( _desc->safe == true, "exception initialize service '%s' (doc: %s)\n%s"
                , service->getServiceId()
                , MENGINE_DOCUMENT_STR( _doc )
                , ex.what()
            );
            
            return;
        }

#if defined(MENGINE_DEBUG)
        m_initializeServiceName = nullptr;
#endif

        if( successful == false )
        {
            *_result = false;

            MENGINE_ASSERTION_EXCEPTION( _desc->safe == true, "invalid initialize service '%s' (doc: %s)"
                , service->getServiceId()
                , MENGINE_DOCUMENT_STR( _doc )
            );
            
            return;
        }

        _desc->initialize = true;

        if( this->checkWaits_( service ) == false )
        {
            *_result = false;

            MENGINE_THROW_EXCEPTION( "invalid initialize service '%s' (waits) (doc: %s)"
                , service->getServiceId()
                , MENGINE_DOCUMENT_STR( _doc )
            );
        }

        if( service->runService() == false )
        {
            *_result = false;

            MENGINE_THROW_EXCEPTION( "invalid run service '%s' (waits) (doc: %s)"
                , service->getServiceId()
                , MENGINE_DOCUMENT_STR( _doc )
            );
        }

        this->autoRegistration_( _desc );

        this->deferredRequiredInitialize_( _desc, _doc, _result );
    }
    //////////////////////////////////////////////////////////////////////////
    bool ServiceProvider::finalizeService( const Char * _name )
    {
        for( uint32_t index = 0; index != m_leaveCount; ++index )
        {
            LeaveDesc & desc = m_leaves[index];

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
            desc.available = false;
            desc.initialize = false;
            desc.service->finalizeService();

            this->autoUnregistration_( &desc );

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

            this->removeDependency_( desc.service );

            desc.exist = false;
            desc.available = false;
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
        MENGINE_ASSERTION_FATAL( m_dependenciesCount < MENGINE_SERVICE_PROVIDER_DEPENDENCY_COUNT, "overflow dependency count" );
        MENGINE_ASSERTION_FATAL( StdString::strlen( _name ) < MENGINE_SERVICE_PROVIDER_NAME_SIZE, "invalid service name '%s' max size '%zu' >= '%u'"
            , _name
            , StdString::strlen( _name )
            , MENGINE_SERVICE_PROVIDER_NAME_SIZE
        );

        MENGINE_SPINLOCK_SCOPE( m_futexDependencies );

        uint32_t id = m_dependenciesCount++;

        DependencyDesc & desc = m_dependencies[id];

        desc.name.assign( _name );
        desc.dependency.assign( _dependency );
    }
    //////////////////////////////////////////////////////////////////////////
    bool ServiceProvider::waitService( const Char * _owner, const Char * _name, const LambdaWaitService & _lambda )
    {
        MENGINE_ASSERTION_FATAL( m_waitsCount < MENGINE_SERVICE_PROVIDER_MAX_WAIT, "overflow wait count" );
        MENGINE_ASSERTION_FATAL( StdString::strlen( _name ) < MENGINE_SERVICE_PROVIDER_NAME_SIZE, "invalid service name '%s' max size '%zu' >= '%u'"
            , _name
            , StdString::strlen( _name )
            , MENGINE_SERVICE_PROVIDER_NAME_SIZE
        );

        MENGINE_SPINLOCK_SCOPE( m_futexWaits );

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

        uint32_t id = m_waitsCount++;

        WaitDesc & desc = m_waits[id];

        desc.owner.assign( _owner );
        desc.name.assign( _name );
        desc.lambda = _lambda;

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    bool ServiceProvider::leaveService( const Char * _owner, const Char * _name, const LambdaLeaveService & _lambda )
    {
        MENGINE_ASSERTION_FATAL( m_leaveCount < MENGINE_SERVICE_PROVIDER_LEAVE_COUNT, "overflow leave count" );
        MENGINE_ASSERTION_FATAL( StdString::strlen( _name ) < MENGINE_SERVICE_PROVIDER_NAME_SIZE, "invalid service name '%s' max size '%zu' >= '%u'"
            , _name
            , StdString::strlen( _name )
            , MENGINE_SERVICE_PROVIDER_NAME_SIZE
        );

        MENGINE_SPINLOCK_SCOPE( m_futexLeaves );

        uint32_t id = m_leaveCount++;

        LeaveDesc & desc = m_leaves[id];

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
            LeaveDesc & desc = m_leaves[index];

            if( desc.owner.compare( _owner ) != 0 )
            {
                continue;
            }

            if( desc.lambda == nullptr )
            {
                continue;
            }
            
            const Char * name = desc.name.c_str();
            
            const bool * initialized = this->isInitializeServiceProvider( name );
            
            LambdaLeaveService lambda = desc.lambda;

            desc.lambda = nullptr;
            desc.name.clear();
            desc.owner.clear();
            
            if( *initialized == false )
            {
                continue;
            }

            lambda();
        }
    }
    //////////////////////////////////////////////////////////////////////////
    void ServiceProvider::removeDependency_( const ServiceInterfacePtr & _service )
    {
        MENGINE_SPINLOCK_SCOPE( m_futexDependencies );

        const Char * name = _service->getServiceId();

        for( uint32_t index = 0; index != m_dependenciesCount; )
        {
            DependencyDesc & desc = m_dependencies[index];

            if( desc.name[0] != '\0' )
            {
                ++index;

                continue;
            }

            if( desc.name.compare( name ) != 0 )
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
    bool ServiceProvider::checkWaits_( const ServiceInterfacePtr & _service )
    {
        MENGINE_SPINLOCK_SCOPE( m_futexWaits );

        const Char * name = _service->getServiceId();

        for( uint32_t index = 0; index != m_waitsCount; )
        {
            WaitDesc & desc = m_waits[index];

            if( desc.name.compare( name ) != 0 )
            {
                ++index;

                continue;
            }

            LambdaWaitService lambda = desc.lambda;

            uint32_t last = --m_waitsCount;
            
            if( index != last )
            {
                WaitDesc & last_desc = m_waits[last];

                desc.name.assign( last_desc.name );
                desc.lambda = last_desc.lambda;
            }

            if( lambda() == false )
            {
                return false;
            }
        }

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    bool ServiceProvider::isRequired_( const ServiceDesc & _desc )
    {
        for( uint32_t index_required = 0; index_required != _desc.required_count; ++index_required )
        {
            const Char * name_required = _desc.required[index_required].c_str();

            bool required = false;

            for( uint32_t index_service = 0; index_service != m_servicesCount; ++index_service )
            {
                const ServiceDesc & service_desc = m_services[index_service];

                if( service_desc.name.compare( name_required ) != 0 )
                {
                    continue;
                }

                if( service_desc.initialize == false )
                {
                    return false;
                }

                required = true;
            }

            if( required == false )
            {
                return false;
            }
        }

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    void ServiceProvider::autoRegistration_( ServiceDesc * const _desc )
    {
        for( uint32_t index_service = 0; index_service != m_servicesCount; ++index_service )
        {
            ServiceDesc & desc = m_services[index_service];

            if( &desc == _desc )
            {
                continue;
            }

            if( desc.service == nullptr )
            {
                continue;
            }

            if( desc.initialize == false )
            {
                continue;
            }

            desc.service->registerService( _desc->service.get() );
        }
    }
    //////////////////////////////////////////////////////////////////////////
    void ServiceProvider::autoUnregistration_( ServiceDesc * const _desc )
    {
        for( uint32_t index_service = 0; index_service != m_servicesCount; ++index_service )
        {
            ServiceDesc & desc = m_services[index_service];

            if( &desc == _desc )
            {
                continue;
            }

            if( desc.service == nullptr )
            {
                continue;
            }

            if( desc.initialize == false )
            {
                continue;
            }

            desc.service->unregisterService( _desc->service.get() );
        }
    }
    //////////////////////////////////////////////////////////////////////////
    void ServiceProvider::deferredRequiredInitialize_( ServiceDesc * const _desc, const DocumentInterfacePtr & _doc, bool * const _result )
    {
        for( uint32_t index_service = 0; index_service != m_servicesCount; ++index_service )
        {
            ServiceDesc & desc = m_services[index_service];

            if( &desc == _desc )
            {
                continue;
            }

            if( desc.requiring == false )
            {
                continue;
            }

            if( this->isRequired_( desc ) == false )
            {
                continue;
            }

            desc.requiring = false;

            this->initializeService_( &desc, _doc, _result );
        }
    }
    //////////////////////////////////////////////////////////////////////////
    const bool * ServiceProvider::isExistServiceProvider( const Char * _name )
    {
#if defined(MENGINE_DEBUG)
        MENGINE_ASSERTION_CRITICAL( m_initializeServiceName == nullptr || StdString::strcmp( m_initializeServiceName, _name ) != 0, "service '%s' already initialize"
            , _name
        );
#endif

        MENGINE_SPINLOCK_SCOPE( m_futexServices );

        for( uint32_t index = 0; index != m_servicesCount; ++index )
        {
            const ServiceDesc & desc = m_services[index];

            if( desc.name.compare( _name ) != 0 )
            {
                continue;
            }

            return &desc.exist;
        }

        MENGINE_ASSERTION_EXCEPTION( m_servicesCount != MENGINE_SERVICE_PROVIDER_COUNT, "overflow service count [exist]" );

        uint32_t id = m_servicesCount++;

        ServiceDesc & desc = m_services[id];

        desc.name.assign( _name );
        desc.exist = false;
        desc.safe = false;
        desc.service = nullptr;
        desc.available = false;
        desc.initialize = false;
        desc.requiring = false;

        return &desc.exist;
    }
    //////////////////////////////////////////////////////////////////////////
    const bool * ServiceProvider::isAvailableServiceProvider( const Char * _name )
    {
#if defined(MENGINE_DEBUG)
        MENGINE_ASSERTION_CRITICAL( m_initializeServiceName == nullptr || StdString::strcmp( m_initializeServiceName, _name ) != 0, "service '%s' in initialize process"
            , _name
        );
#endif

        MENGINE_SPINLOCK_SCOPE( m_futexServices );

        for( uint32_t index = 0; index != m_servicesCount; ++index )
        {
            const ServiceDesc & desc = m_services[index];

            if( desc.name.compare( _name ) != 0 )
            {
                continue;
            }

            return &desc.available;
        }

        MENGINE_ASSERTION_EXCEPTION( m_servicesCount != MENGINE_SERVICE_PROVIDER_COUNT, "overflow service count [available]" );

        uint32_t id = m_servicesCount++;

        ServiceDesc & desc = m_services[id];

        desc.name.assign( _name );
        desc.exist = false;
        desc.safe = false;
        desc.service = nullptr;
        desc.available = false;
        desc.initialize = false;
        desc.requiring = false;

        return &desc.available;
    }
    //////////////////////////////////////////////////////////////////////////
    const bool * ServiceProvider::isInitializeServiceProvider( const Char * _name )
    {
#if defined(MENGINE_DEBUG)
        MENGINE_ASSERTION_CRITICAL( m_initializeServiceName == nullptr || StdString::strcmp( m_initializeServiceName, _name ) != 0, "service '%s' in initialize process"
            , _name
        );
#endif

        MENGINE_SPINLOCK_SCOPE( m_futexServices );

        for( uint32_t index = 0; index != m_servicesCount; ++index )
        {
            const ServiceDesc & desc = m_services[index];

            if( desc.name.compare( _name ) != 0 )
            {
                continue;
            }

            return &desc.initialize;
        }

        MENGINE_ASSERTION_EXCEPTION( m_servicesCount != MENGINE_SERVICE_PROVIDER_COUNT, "overflow service count [initialize]" );

        uint32_t id = m_servicesCount++;

        ServiceDesc & desc = m_services[id];

        desc.name.assign( _name );
        desc.exist = false;
        desc.safe = false;
        desc.service = nullptr;
        desc.available = false;
        desc.initialize = false;
        desc.requiring = false;

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
            LeaveDesc & desc = m_leaves[index];

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
