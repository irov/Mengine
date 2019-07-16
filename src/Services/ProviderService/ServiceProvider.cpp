#include "ServiceProvider.h"

#include "Interface/ServiceInterface.h"

#include "Kernel/Assertion.h"

#include <string.h>

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
    ServiceInterfacePtr ServiceProvider::generateService_( FServiceProviderGenerator _generator )
    {
        if( _generator == nullptr )
        {
            return nullptr;
        }

        ServiceInterface * service;
        if( (*_generator)(&service) == false )
        {
            return nullptr;
        }

        return ServiceInterfacePtr::from( service );
    }
    //////////////////////////////////////////////////////////////////////////
    bool ServiceProvider::initializeService( FServiceProviderGenerator _generator, bool _safe, const Char * _doc, const Char * _file, uint32_t _line )
    {
        ServiceInterfacePtr service = this->generateService_( _generator );

        if( service == nullptr )
        {
            if( _safe == false )
            {
                MENGINE_THROW_EXCEPTION_FL( _file, _line )("invalid generate service doc '%s'"
                    , _doc
                    );
            }

            return false;
        }

        if( service->initializeService() == false )
        {
            if( _safe == false )
            {
                MENGINE_THROW_EXCEPTION_FL( _file, _line )("invalid initialize service doc '%s'"
                    , _doc
                    );
            }

            return false;
        }

        const Char * name = service->getServiceID();

        if( strlen( name ) + 1 > MENGINE_SERVICE_PROVIDER_NAME_SIZE )
        {
            MENGINE_THROW_EXCEPTION_FL( _file, _line )("invalid service name '%s' max size '%d' > '%d'"
                , name
                , strlen( name + 1 )
                , MENGINE_SERVICE_PROVIDER_NAME_SIZE
                );

            return false;
        }

        for( uint32_t index = 0; index != MENGINE_SERVICE_PROVIDER_COUNT; ++index )
        {
            ServiceDesc & desc = m_services[index];

            if( desc.service == nullptr )
            {
                strcpy( desc.name, name );
            }
            else if( strcmp( desc.name, name ) != 0 )
            {
                continue;
            }
            else
            {
                desc.service->finalizeService();
                desc.service = nullptr;
            }

            desc.service = service;

            ++m_servicesCount;

            if( this->checkWaits_( name ) == false )
            {
                MENGINE_THROW_EXCEPTION_FL( _file, _line )("invalid initialize service '%s' doc '%s' (waits)"
                    , name
                    , _doc
                    );

                return false;
            }

            return true;
        }

        MENGINE_THROW_EXCEPTION_FL( _file, _line )("invalid allocate service name '%s' max count '%d'"
            , name
            , MENGINE_SERVICE_PROVIDER_COUNT
            );

        return false;
    }
    //////////////////////////////////////////////////////////////////////////
    bool ServiceProvider::finalizeService( const Char * _name )
    {
        for( uint32_t index = 0; index != m_leaveCount; ++index )
        {
            LeaveDesc & desc = m_leaving[index];

            if( strcmp( desc.name, _name ) != 0 )
            {
                continue;
            }

            desc.lambda();

            desc.lambda = nullptr;
            desc.name[0] = '\0';
        }

        for( uint32_t index = 0; index != m_dependenciesCount; ++index )
        {
            const DependencyDesc & desc = m_dependencies[index];

            if( strcmp( desc.dependency, _name ) != 0 )
            {
                continue;
            }

            this->finalizeService( desc.name );
        }

        for( uint32_t index = 0; index != m_servicesCount; ++index )
        {
            ServiceDesc & desc = m_services[index];

            if( strcmp( desc.name, _name ) != 0 )
            {
                continue;
            }

            if( desc.service == nullptr )
            {
                break;
            }

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

            if( strcmp( desc.dependency, _name ) != 0 )
            {
                continue;
            }

            this->destroyService( desc.name );
        }

        for( uint32_t index = 0; index != m_servicesCount; ++index )
        {
            ServiceDesc & desc = m_services[index];

            if( strcmp( desc.name, _name ) != 0 )
            {
                continue;
            }

            if( desc.service == nullptr )
            {
                break;
            }

            this->removeDependency_( _name );

            desc.service->finalizeService();
            desc.service = nullptr;

            --m_servicesCount;

            return true;
        }

        return false;
    }
    //////////////////////////////////////////////////////////////////////////
    void ServiceProvider::dependencyService( const Char * _name, const Char * _dependency )
    {
        MENGINE_ASSERTION( m_dependenciesCount < MENGINE_SERVICE_PROVIDER_DEPENDENCY_COUNT );

        DependencyDesc & desc = m_dependencies[m_dependenciesCount++];

        strcpy( desc.name, _name );
        strcpy( desc.dependency, _dependency );
    }
    //////////////////////////////////////////////////////////////////////////
    bool ServiceProvider::waitService( const Char * _name, const LambdaWaitService & _lambda )
    {
        MENGINE_ASSERTION( m_waitsCount < MENGINE_SERVICE_PROVIDER_MAX_WAIT );

        for( uint32_t index = 0; index != m_servicesCount; ++index )
        {
            ServiceDesc & desc = m_services[index];

            if( strcmp( desc.name, _name ) != 0 )
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

        strcpy( desc.name, _name );
        desc.lambda = _lambda;

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    bool ServiceProvider::leaveService( const Char * _name, const LambdaLeaveService & _lambda )
    {
        MENGINE_ASSERTION( m_leaveCount < MENGINE_SERVICE_PROVIDER_LEAVE_COUNT );

        LeaveDesc & desc = m_leaving[m_leaveCount++];

        strcpy( desc.name, _name );
        desc.lambda = _lambda;

        return true;
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

            if( strcmp( desc.name, _name ) != 0 )
            {
                ++index;

                continue;
            }

            DependencyDesc & last_desc = m_dependencies[--m_dependenciesCount];

            strcpy( desc.name, last_desc.name );
            strcpy( desc.dependency, last_desc.dependency );
        }
    }
    //////////////////////////////////////////////////////////////////////////
    bool ServiceProvider::checkWaits_( const Char * _name )
    {
        for( uint32_t index = 0; index != m_waitsCount; )
        {
            WaitDesc & desc = m_waits[index];

            if( strcmp( desc.name, _name ) != 0 )
            {
                ++index;

                continue;
            }

            if( desc.lambda() == false )
            {
                return false;
            }

            WaitDesc & last_desc = m_waits[--m_waitsCount];

            strcpy( desc.name, last_desc.name );
            desc.lambda = last_desc.lambda;
        }

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    bool ServiceProvider::existService( const Char * _name ) const
    {
        for( uint32_t index = 0; index != m_servicesCount; ++index )
        {
            const ServiceDesc & desc = m_services[index];

            if( desc.service == nullptr )
            {
                continue;
            }

            if( strcmp( desc.name, _name ) != 0 )
            {
                continue;
            }

            return true;
        }

        return false;
    }
    //////////////////////////////////////////////////////////////////////////
    const ServiceInterfacePtr & ServiceProvider::getService( const Char * _name ) const
    {
        for( uint32_t index = 0; index != m_servicesCount; ++index )
        {
            const ServiceDesc & desc = m_services[index];

            if( strcmp( desc.name, _name ) != 0 )
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
        for( uint32_t index = 0; index != m_waitsCount; ++index )
        {
            WaitDesc & desc = m_waits[index];

            desc.lambda = nullptr;
        }

        for( uint32_t index = 0; index != m_servicesCount; ++index )
        {
            ServiceDesc & desc = m_services[index];

            if( desc.service == nullptr )
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
    }
    //////////////////////////////////////////////////////////////////////////
    void ServiceProvider::stopServices()
    {
        for( uint32_t index = 0; index != MENGINE_SERVICE_PROVIDER_MAX_WAIT; ++index )
        {
            WaitDesc & desc = m_waits[index];
                        
            desc.lambda = nullptr;
            desc.name[0] = '\0';
        }

        for( uint32_t index = 0; index != m_servicesCount; ++index )
        {
            ServiceDesc & desc = m_services[index];

            if( desc.service == nullptr )
            {
                continue;
            }

            desc.service->stopService();
        }
    }
}