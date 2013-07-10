#	pragma once

#	include "Config/Typedef.h"

#   include "Utils/Factory/Factory.h"
#   include "Utils/Factory/Factorable.h"

namespace Menge
{
    class ServiceProviderInterface;

	class ServiceInterface
	{
    public:
        virtual void setServiceProvider( ServiceProviderInterface * _serviceProvider ) = 0;
        virtual ServiceProviderInterface * getServiceProvider() const = 0;
	};

	class ServiceListenerInterface
	{
	public:
		virtual bool onRegistryService( ServiceProviderInterface * _serviceProvider, ServiceInterface * _service ) = 0;
        virtual void onUnregistryService( ServiceProviderInterface * _serviceProvider, ServiceInterface * _service ) = 0;
	};

	class ServiceProviderInterface
	{
	public:
		virtual ServiceInterface * getService( const char * _name ) const = 0;

    public:
		virtual bool registryService( const char * _name, ServiceInterface * _service ) = 0;
		virtual bool unregistryService( const char * _name ) = 0;
	};

    namespace Helper
    {
        template<class T>
        inline T * getService( ServiceProviderInterface * _serviceProvider )
        {
            static T * s_service = nullptr;

            if( s_service == nullptr )
            {
                const char * serviceName = T::getServiceName();

                ServiceInterface * service = _serviceProvider->getService( serviceName );

#   ifdef _DEBUG
                if( dynamic_cast<T*>(service) == nullptr )
                {
                    return nullptr;
                }
#   endif

                s_service = static_cast<T *>(service);
            }

            return s_service;
        }
    }

#   define SERVICE_DECLARE( NAME )\
    public:\
        inline static const char * getServiceName(){ return NAME; };\
    protected:

#   define SERVICE_FACTORY( Name, Service, Implement )\
    extern "C"\
    {\
    bool create##Name( Service ** _service )\
    {\
    if( _service == nullptr )\
    {\
    return false;\
    }\
    *_service = new Implement();\
    return true;\
    }\
    void destroy##Name( Service * _service )\
    {\
    delete static_cast<Implement *>(_service);\
    }\
    }\
    struct __mengine_dummy{}

#   define SERVICE_EXTERN( Name, Service )\
    extern "C"\
    {\
    extern bool create##Name( Service ** );\
    extern void destroy##Name( Service * );\
    }\
    struct __mengine_extern_dummy_##Name{}

#   define SERVICE_DUMMY( Name, Service )\
    extern "C"\
    {\
    bool create##Name( Service ** ){return false;};\
    void destroy##Name( Service * ){};\
    }\
    struct __mengine_extern_dummy_##Name{}

#   define SERVICE_REGISTRY( Provider, Service )\
    Provider->registryService(Service->getServiceName(), Service)

#   define SERVICE_CREATE( Name, Service )\
    create##Name( Service )

#   define SERVICE_DESTROY( Name, Service )\
    destroy##Name( Service )
}