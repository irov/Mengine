#include "ServiceProvider.h"

#include "Interface/ServiceInterface.h"

#include "Kernel/ConstStringHelper.h"
#include "Kernel/Assertion.h"
#include "Kernel/AssertionMemoryPanic.h"
#include "Kernel/SpinLockScope.h"
#include "Kernel/Logger.h"

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
        : m_dependenciesCount( 0 )
        , m_servicesCount( 0 )
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

        service->setUniqueIdentity( MENGINE_UINT32_C(~0) );

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

        MENGINE_SPINLOCK_SCOPE( m_spinLockServices );

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
                if( desc.exist == true && desc.service != nullptr )
                {
                    desc.service->replaceService();
                    desc.service->finalizeService();

                    this->autoUnregistration_( &desc );

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
            desc.required_count = 0;

            if( service->availableService() == false )
            {
                return true;
            }

            desc.service = service;
            desc.available = true;

            const ServiceRequiredList & required = service->requiredServices();

            size_t requiredCount = required.size();

            if( requiredCount > MENGINE_SERVICE_PROVIDER_MAX_REQUIRED )
            {
                desc.service = nullptr;
                desc.exist = false;
                desc.available = false;

                MENGINE_ASSERTION_EXCEPTION( _safe == true, "service '%s' required service count '%zu' exceeds maximum '%u'"
                    , name
                    , requiredCount
                    , MENGINE_SERVICE_PROVIDER_MAX_REQUIRED
                );

                return false;
            }

            for( const Char * require_name : required )
            {
                desc.required[desc.required_count++] = require_name;
            }

            bool visited[MENGINE_SERVICE_PROVIDER_COUNT] = {};

            if( this->hasDependency_( desc, name, visited ) == true )
            {
                desc.service = nullptr;
                desc.exist = false;
                desc.available = false;
                desc.required_count = 0;

                MENGINE_ASSERTION_EXCEPTION( _safe == true, "cyclic required services for '%s'"
                    , name
                );

                return false;
            }

            if( this->findUninitializedRequiredService_( desc ) != nullptr )
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

        if( service->runService() == false )
        {
            *_result = false;
            _desc->initialize = false;

            MENGINE_THROW_EXCEPTION( "invalid run service '%s' (waits) (doc: %s)"
                , service->getServiceId()
                , MENGINE_DOCUMENT_STR( _doc )
            );
        }

        if( this->autoRegistration_( _desc ) == false )
        {
            *_result = false;
            _desc->initialize = false;

            this->autoUnregistration_( _desc );

            MENGINE_THROW_EXCEPTION( "invalid auto registration service '%s' (doc: %s)"
                , service->getServiceId()
                , MENGINE_DOCUMENT_STR( _doc )
            );
        }

        if( this->checkWaits_( service ) == false )
        {
            *_result = false;
            _desc->initialize = false;

            this->autoUnregistration_( _desc );

            MENGINE_THROW_EXCEPTION( "invalid initialize service '%s' (waits) (doc: %s)"
                , service->getServiceId()
                , MENGINE_DOCUMENT_STR( _doc )
            );
        }

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
            const DependencyDesc & dependency = m_dependencies[index];

            if( dependency.dependency.compare( _name ) != 0 )
            {
                continue;
            }

            this->finalizeService( dependency.name.c_str() );
        }

        ServiceDesc * desc = this->findServiceDesc_( _name );

        if( desc == nullptr )
        {
            return false;
        }

        if( desc->service == nullptr )
        {
            return false;
        }

        this->markServiceFinalized_( desc );

        this->autoUnregistration_( desc );

        desc->service->finalizeService();

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    bool ServiceProvider::destroyService( const Char * _name )
    {
        for( uint32_t index = 0; index != m_dependenciesCount; )
        {
            const DependencyDesc & dependency = m_dependencies[index];

            if( dependency.dependency.compare( _name ) != 0 )
            {
                ++index;

                continue;
            }

            StaticString<MENGINE_SERVICE_PROVIDER_NAME_SIZE> dependentName = dependency.name;

            this->destroyService( dependentName.c_str() );
            this->removeDependency_( dependentName.c_str() );
        }

        this->finalizeService( _name );

        ServiceDesc * desc = this->findServiceDesc_( _name );

        if( desc == nullptr )
        {
            return false;
        }

        if( desc->service == nullptr )
        {
            return false;
        }

        this->removeDependency_( _name );

        ServiceInterfacePtr service = this->detachService_( desc );

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    bool ServiceProvider::validateServices() const
    {
        for( uint32_t index = 0; index != m_servicesCount; ++index )
        {
            const ServiceDesc & desc = m_services[index];

            if( desc.service == nullptr )
            {
                continue;
            }

            if( desc.initialize == true )
            {
                continue;
            }

            const Char * serviceName = desc.name.c_str();
            const Char * requiredName = this->findUninitializedRequiredService_( desc );

            if( requiredName != nullptr )
            {
                LOGGER_ERROR( "service '%s' requires uninitialized service '%s'", serviceName, requiredName );

                return false;
            }

            LOGGER_ERROR( "service '%s' failed to initialize", serviceName );

            return false;
        }

        return true;
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

        MENGINE_SPINLOCK_SCOPE( m_spinLockDependencies );

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

        MENGINE_SPINLOCK_SCOPE( m_spinLockWaits );

        for( uint32_t index = 0; index != m_servicesCount; ++index )
        {
            ServiceDesc & desc = m_services[index];

            if( desc.name.compare( _name ) != 0 )
            {
                continue;
            }

            if( desc.initialize == false )
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

        MENGINE_SPINLOCK_SCOPE( m_spinLockLeaves );

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
    bool ServiceProvider::checkWaits_( const ServiceInterfacePtr & _service )
    {
        MENGINE_SPINLOCK_SCOPE( m_spinLockWaits );

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

                desc.owner.assign( last_desc.owner );
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
    ServiceProvider::ServiceDesc * ServiceProvider::findServiceDesc_( const Char * _name )
    {
        for( uint32_t index = 0; index != m_servicesCount; ++index )
        {
            ServiceDesc & desc = m_services[index];

            if( desc.name.compare( _name ) != 0 )
            {
                continue;
            }

            return &desc;
        }

        return nullptr;
    }
    //////////////////////////////////////////////////////////////////////////
    const ServiceProvider::ServiceDesc * ServiceProvider::findServiceDesc_( const Char * _name ) const
    {
        for( uint32_t index = 0; index != m_servicesCount; ++index )
        {
            const ServiceDesc & desc = m_services[index];

            if( desc.name.compare( _name ) != 0 )
            {
                continue;
            }

            return &desc;
        }

        return nullptr;
    }
    //////////////////////////////////////////////////////////////////////////
    const Char * ServiceProvider::findUninitializedRequiredService_( const ServiceDesc & _desc ) const
    {
        for( uint32_t index = 0; index != _desc.required_count; ++index )
        {
            const Char * requiredName = _desc.required[index].c_str();
            const ServiceDesc * required = this->findServiceDesc_( requiredName );

            if( required == nullptr )
            {
                return requiredName;
            }

            if( required->initialize == false )
            {
                return requiredName;
            }
        }

        return nullptr;
    }
    //////////////////////////////////////////////////////////////////////////
    bool ServiceProvider::hasRequiredService_( const ServiceDesc & _desc, const Char * _name ) const
    {
        for( uint32_t index = 0; index != _desc.required_count; ++index )
        {
            if( _desc.required[index].compare( _name ) == 0 )
            {
                return true;
            }
        }

        return false;
    }
    //////////////////////////////////////////////////////////////////////////
    bool ServiceProvider::hasDependency_( const ServiceDesc & _desc, const Char * _name, bool * const _visited ) const
    {
        if( this->hasRequiredService_( _desc, _name ) == true )
        {
            return true;
        }

        for( uint32_t index = 0; index != _desc.required_count; ++index )
        {
            const Char * requiredName = _desc.required[index].c_str();
            const ServiceDesc * required = this->findServiceDesc_( requiredName );

            if( required == nullptr )
            {
                continue;
            }

            uint32_t serviceIndex = static_cast<uint32_t>(required - m_services);

            if( _visited[serviceIndex] == true )
            {
                continue;
            }

            _visited[serviceIndex] = true;

            if( this->hasDependency_( *required, _name, _visited ) == true )
            {
                return true;
            }
        }

        return false;
    }
    //////////////////////////////////////////////////////////////////////////
    void ServiceProvider::removeDependency_( const Char * _name )
    {
        MENGINE_SPINLOCK_SCOPE( m_spinLockDependencies );

        for( uint32_t index = 0; index != m_dependenciesCount; )
        {
            DependencyDesc & dependency = m_dependencies[index];

            if( dependency.name.compare( _name ) != 0 )
            {
                ++index;

                continue;
            }

            dependency = m_dependencies[--m_dependenciesCount];
        }
    }
    //////////////////////////////////////////////////////////////////////////
    bool ServiceProvider::autoRegistration_( ServiceDesc * const _desc )
    {
        for( uint32_t index = 0; index != m_servicesCount; ++index )
        {
            ServiceDesc & desc = m_services[index];

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

            if( desc.service->registerService( _desc->service ) == false )
            {
                return false;
            }
        }

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    void ServiceProvider::autoUnregistration_( ServiceDesc * const _desc )
    {
        for( uint32_t index = 0; index != m_servicesCount; ++index )
        {
            ServiceDesc & desc = m_services[index];

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

            desc.service->unregisterService( _desc->service );
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

            if( this->findUninitializedRequiredService_( desc ) != nullptr )
            {
                continue;
            }

            desc.requiring = false;

            this->initializeService_( &desc, _doc, _result );
        }
    }
    //////////////////////////////////////////////////////////////////////////
    void ServiceProvider::markServiceFinalized_( ServiceDesc * const _desc )
    {
        MENGINE_SPINLOCK_SCOPE( m_spinLockServices );

        _desc->exist = false;
        _desc->available = false;
        _desc->initialize = false;
        _desc->requiring = false;
    }
    //////////////////////////////////////////////////////////////////////////
    ServiceInterfacePtr ServiceProvider::detachService_( ServiceDesc * const _desc )
    {
        MENGINE_SPINLOCK_SCOPE( m_spinLockServices );

        ServiceInterfacePtr service = _desc->service;

        _desc->service = nullptr;
        _desc->required_count = 0;

        return service;
    }
    //////////////////////////////////////////////////////////////////////////
    const bool * ServiceProvider::isExistServiceProvider( const Char * _name )
    {
#if defined(MENGINE_DEBUG)
        MENGINE_ASSERTION_CRITICAL( m_initializeServiceName == nullptr || StdString::strcmp( m_initializeServiceName, _name ) != 0, "service '%s' already initialize"
            , _name
        );
#endif

        MENGINE_SPINLOCK_SCOPE( m_spinLockServices );

        ServiceDesc * found = this->findServiceDesc_( _name );

        if( found != nullptr )
        {
            return &found->exist;
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

        MENGINE_SPINLOCK_SCOPE( m_spinLockServices );

        ServiceDesc * found = this->findServiceDesc_( _name );

        if( found != nullptr )
        {
            return &found->available;
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

        MENGINE_SPINLOCK_SCOPE( m_spinLockServices );

        ServiceDesc * found = this->findServiceDesc_( _name );

        if( found != nullptr )
        {
            return &found->initialize;
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
    void ServiceProvider::setConfiguration( const Configuration & _configuration )
    {
        m_configuration = _configuration;
    }
    //////////////////////////////////////////////////////////////////////////
    const Configuration & ServiceProvider::getConfiguration() const
    {
        return m_configuration;
    }
    //////////////////////////////////////////////////////////////////////////
    void ServiceProvider::destroy()
    {
        for( uint32_t index = m_servicesCount; index != 0; --index )
        {
            ServiceDesc & desc = m_services[index - 1];

            if( desc.service == nullptr )
            {
                continue;
            }

            const Char * serviceName = desc.name.c_str();
            this->destroyService( serviceName );
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

            if( desc.service->isInitializeService() == false )
            {
                continue;
            }

            desc.service->stopService();
        }
    }
    //////////////////////////////////////////////////////////////////////////
}
